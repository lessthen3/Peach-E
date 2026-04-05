/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#ifdef PEACH_RENDERER_VULKAN

#include "VulkanRenderer.h"

namespace PeachCore::Vulkan {

    bool
        Renderer::Initialize //used for lazy initialization and for default constructor support without needing to define an explicit move constructor UwU
        (
            SDL_Window* fp_MainWindow,
            shared_ptr<Logger> fp_RenderingLogger
        )
    {
        if (not fp_RenderingLogger) //MAYBE: maybe we should just create a new logger actually nvm that involves getting a reference to the console lmfao
        {
            PRINT_ERROR("Tried to initialize VulkanRenderer with a nullptr for the Rendering Logger doofus, Ending program execution immediately since no valid logger was found");
            return false;
        }

        rendering_logger = fp_RenderingLogger;

        if (not fp_MainWindow)
        {
            rendering_logger->Fatal("Tried to initialize VulkanRenderer with a nullptr for the SDL Window doofus, Ending program execution immediately since no valid SDL Window was found", "VulkanRenderer");
            return false;
        }

        pm_Init.MainWindow = fp_MainWindow;

        SDL_GetWindowSize(pm_Init.MainWindow, &pm_RenderData.CurrentWindowWidth, &pm_RenderData.CurrentWindowHeight); //grab window size so render loop can start properly and not be thwarted at beginframe()

        if (not InitializeDevice("Game"))
        {
            rendering_logger->Fatal("Failed to create Vulkan device, exiting program execution immediately", "VulkanRenderer");
            return false;
        }
        else if (not CreateSwapChain())
        {

            return false; //;w;
        }
        else if (not InitializeQueues())
        {

            return false; //;o;
        }
        else if (not CreateRenderPass())
        {

            return false; //>O<
        }
        else if (not CreateFrameBuffer())
        {

            return false; //UoU
        }
        else if (not CreateCommandPool())
        {

            return false; //OuO
        }
        else if (not CreateCommandBuffers())
        {

            return false; //&_&
        }
        else if (not CreateSyncObjects())
        {

            return false; //^_^
        }

        InitializePeachUI(); //fingers crossed

        return true; //rawr UwU forgot this return path zzzzzzzzzzzz
    }

    bool
        Renderer::InitializePeachUI()
    {
        m_DeviceInfo.ReadDevice(pm_Init.Device.physical_device.physical_device);

        return true;
    }

    uint32_t
        Renderer::BeginFrame()
    {
        //////////////////// Get Current Window Size Before Starting Render Frame ////////////////////
        SDL_GetWindowSize(pm_Init.MainWindow, &pm_RenderData.CurrentWindowWidth, &pm_RenderData.CurrentWindowHeight);

        //Check if window is minimized
        if ((SDL_GetWindowFlags(pm_Init.MainWindow) & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN | SDL_WINDOW_OCCLUDED))) 
        {
            //maybe print smth idk gotta log it once not a million times ever uwu 
            return Renderer::StatusCode::NO_VALID_RENDERING_SURFACE;
        }
        else if (pm_RenderData.CurrentWindowWidth == 0 or pm_RenderData.CurrentWindowHeight == 0) //this executes first, then when the window is resized properly to be visible, it will trigger the regular swapchain recreation >O<
        {
            rendering_logger->Info("Won't start rendering when window size is 0", "VulkanRenderer");
            return Renderer::StatusCode::NO_VALID_RENDERING_SURFACE; //>w<
        }
        else if (pm_IsFrameStarted)
        {
            rendering_logger->Warning("Frame already began, please only call BeginFrame() once ya done goofed", "VulkanRenderer");
            return Renderer::StatusCode::BEGIN_FRAME_CALLED_WHILE_FRAME_IS_ALREADY_STARTED;
        }

        // Wait for the current frame to finish
        pm_Init.Dispatch.waitForFences(1, &pm_RenderData.InFlightFences[pm_RenderData.CurrentFrameCycle], VK_TRUE, UINT64_MAX);

        // Acquire next image from swapchain
        VkResult result = pm_Init.Dispatch.acquireNextImageKHR
        (
            pm_Init.SwapChain, 
            UINT64_MAX, 
            pm_RenderData.AvailableSemaphores[pm_RenderData.CurrentFrameCycle], 
            VK_NULL_HANDLE, 
            &pm_RenderData.CurrentSwapchainImageIndex
        );

        PRINT("acquired image index: " + std::to_string(pm_RenderData.CurrentSwapchainImageIndex) + " currentframe: " + to_string(pm_RenderData.CurrentFrameCycle), Colours::Magenta);

        if (result != VK_SUCCESS and result != VK_SUBOPTIMAL_KHR)
        {
            rendering_logger->Error(fmt::format("failed to acquire swapchain image. Error: {} ", static_cast<int>(result)), "VulkanRenderer");
            return Renderer::StatusCode::FAILED_TO_ACQUIRE_NEXT_SWAPCHAIN_IMAGE_ERROR;
        }

        // Fence ownership tracking
        if (pm_RenderData.ImageInFlight[pm_RenderData.CurrentSwapchainImageIndex] != VK_NULL_HANDLE)
        {
            pm_Init.Dispatch.waitForFences(1, &pm_RenderData.ImageInFlight[pm_RenderData.CurrentSwapchainImageIndex], VK_TRUE, UINT64_MAX);
        }

        pm_RenderData.ImageInFlight[pm_RenderData.CurrentSwapchainImageIndex] = pm_RenderData.InFlightFences[pm_RenderData.CurrentFrameCycle];

        // Reset fence
        pm_Init.Dispatch.resetFences(1, &pm_RenderData.InFlightFences[pm_RenderData.CurrentFrameCycle]);

        //grab reference to current command buffer
        VkCommandBuffer cmd = pm_RenderData.CommandBuffers[pm_RenderData.CurrentSwapchainImageIndex];

        // Begin command buffer
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (pm_Init.Dispatch.beginCommandBuffer(cmd, &begin_info) != VK_SUCCESS)
        {
            rendering_logger->Info("Failed to begin command buffer", "VulkanRenderer");
            return Renderer::StatusCode::FAILED_TO_BEGIN_COMMAND_BUFFER_ERROR;
        }

        // Begin render pass
        VkRenderPassBeginInfo rp_begin{};
        rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin.renderPass = pm_RenderData.RenderPass;
        rp_begin.framebuffer = pm_RenderData.FrameBuffers[pm_RenderData.CurrentSwapchainImageIndex];
        rp_begin.renderArea.offset = { 0, 0 };
        rp_begin.renderArea.extent = pm_Init.SwapChain.extent;
        VkClearValue clearColor = { {{0.2f, 0.0f, 0.4f, 1.0f}} }; //purrrrple
        rp_begin.clearValueCount = 1;
        rp_begin.pClearValues = &clearColor;

        pm_Init.Dispatch.cmdBeginRenderPass(cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

        //////////////////// Frame Has Successfully Started ////////////////////

        pm_IsFrameStarted = true;
        
        //////////////////// ok ////////////////////

        return Renderer::StatusCode::OK;
    }

    uint32_t
        Renderer::DrawFrame()
    {
        if (not pm_IsFrameStarted)
        {
            rendering_logger->Warning("Tried calling DrawFrame() before any valid call to BeginFrame() tf are ya doing m8", "VulkanRenderer");
            return Renderer::StatusCode::DRAW_FRAME_BEFORE_BEGIN_FRAME_ERROR;
        }

        VkCommandBuffer cmd = pm_RenderData.CommandBuffers[pm_RenderData.CurrentSwapchainImageIndex];

        // Set viewport and scissor
        VkViewport viewport =
        {
            0.0f, 0.0f,
            (float)pm_Init.SwapChain.extent.width,
            (float)pm_Init.SwapChain.extent.height,
            0.0f, 1.0f
        };

        VkRect2D scissor = 
        {
            { 0, 0 },
            pm_Init.SwapChain.extent
        };

        pm_Init.Dispatch.cmdSetViewport(cmd, 0, 1, &viewport);
        pm_Init.Dispatch.cmdSetScissor(cmd, 0, 1, &scissor);

        auto it_GraphicsPipeline = pm_RenderData.GraphicsPipelines.begin();
        auto end_GraphicsPipeline = pm_RenderData.GraphicsPipelines.end();

        while (it_GraphicsPipeline != end_GraphicsPipeline) //hopefully the two lists stay the same maybe i should struct that
        {
            // Bind pipeline and issue draw
            VkPipeline pipeline = it_GraphicsPipeline->second;
            VkPipelineLayout layout = pm_RenderData.PipelineLayouts.at(it_GraphicsPipeline->first);

            pm_Init.Dispatch.cmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            // 🔥 🔥 🔥 this is where you dynamically bind descriptor sets / push constants / vertex buffers etc

            pm_Init.Dispatch.cmdDraw(cmd, 3, 1, 0, 0);
            ++it_GraphicsPipeline;
        }

        return Renderer::StatusCode::OK;
    }

    uint32_t
        Renderer::EndFrame()
    {
        //////////////////// Check if Frame Began Properly ////////////////////

        if (not pm_IsFrameStarted)
        {
            rendering_logger->Warning("Tried calling EndFrame() before any valid call to BeginFrame() tf are ya doing m8", "VulkanRenderer");
            return Renderer::StatusCode::END_FRAME_CALLED_WHEN_FRAME_WASNT_STARTED_ERROR;
        }

        //////////////////// Obtain Command Buffer and End it ////////////////////

        VkCommandBuffer cmd = pm_RenderData.CommandBuffers[pm_RenderData.CurrentSwapchainImageIndex];

        pm_Init.Dispatch.cmdEndRenderPass(cmd);

        if (pm_Init.Dispatch.endCommandBuffer(cmd) != VK_SUCCESS)
        {
            rendering_logger->Fatal("Failed to end command buffer", "VulkanRenderer");
            return Renderer::StatusCode::FAILED_TO_END_COMMAND_BUFFER;
        }

        //////////////////// Submit Command Buffer ////////////////////

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] = { pm_RenderData.AvailableSemaphores[pm_RenderData.CurrentFrameCycle] };
        VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd;

        VkSemaphore signal_semaphores[] = { pm_RenderData.FinishedSemaphores[pm_RenderData.CurrentFrameCycle] };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        if (pm_Init.Dispatch.queueSubmit(pm_RenderData.GraphicsQueue, 1, &submit_info, pm_RenderData.InFlightFences[pm_RenderData.CurrentFrameCycle]) != VK_SUCCESS)
        {
            rendering_logger->Error("Failed to submit draw command buffer", "VulkanRenderer");
            return Renderer::StatusCode::FAILED_TO_SUBMIT_DRAW_COMMAND_BUFFER;
        }

        //////////////////// Present Swapchain Image ////////////////////

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapChains[] = { pm_Init.SwapChain };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapChains;

        present_info.pImageIndices = &pm_RenderData.CurrentSwapchainImageIndex;

        VkResult result = pm_Init.Dispatch.queuePresentKHR(pm_RenderData.PresentQueue, &present_info);

        //////////////////// Frame Over So Signal as Such ////////////////////

        pm_IsFrameStarted = false;

        //////////////////// Check for Window Resize ////////////////////
        
        //random dude on forums said only use windowing size uwu idk what ab surface uwu and said recreating swapchains extra times is never a bad thing uwu only perf hit
        if (pm_RenderData.CurrentWindowWidth != pm_Init.SwapChain.extent.width or pm_RenderData.CurrentWindowHeight != pm_Init.SwapChain.extent.height) 
        {
            rendering_logger->Info("Attempting to recreate swapchain due to window resize", "VulkanRenderer");

            if(RecreateSwapChain())
            {
                pm_RenderData.CurrentFrameCycle = 0; //reset image to 0th index since the recreated swapchain starts on 0th, NOTE: should be done if swapchain recreation isnt successful uwu
                return Renderer::RECREATED_SWAPCHAIN_SUCCESSFULLY & Renderer::OK;
            }
            else
            {
                rendering_logger->Error("Failed to recreate swapchain after window resize event", "VulkanRenderer");
                return Renderer::StatusCode::FAILED_TO_RECREATE_SWAPCHAIN_ERROR; //WARNING: this approach always assumes the swapchain can successfully be recreated, gotta handle if it fails somehow but idk lemme read the docs some more
            }
        }
        //idfk
        else if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            rendering_logger->Error("VK_ERROR_OUT_OF_DATE_KHR happened idk y", "VulkanRenderer");
            return Renderer::StatusCode::OUT_OF_DATE_VULKAN_KHR;
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            rendering_logger->Warning("VK_SUBOPTIMAL_KHR happened idk y", "VulkanRenderer");
            return Renderer::StatusCode::SUBOPTIMAL_VULKAN_KHR;
        }
        else if (result != VK_SUCCESS)
        {
            rendering_logger->Error("Failed to present swapchain image", "VulkanRenderer");
            return Renderer::StatusCode::NOT_VULKAN_SUCCESS;
        }
        else
        {
            pm_RenderData.CurrentFrameCycle = (pm_RenderData.CurrentFrameCycle + 1) % pm_Init.SwapChain.image_count;
            return Renderer::StatusCode::OK;
        }
    }

    void
        Renderer::CleanUp()
    {
        for (size_t i = 0; i < pm_Init.SwapChain.image_count; i++)
        {
            pm_Init.Dispatch.destroySemaphore(pm_RenderData.FinishedSemaphores[i], nullptr);
            pm_Init.Dispatch.destroySemaphore(pm_RenderData.AvailableSemaphores[i], nullptr);
            pm_Init.Dispatch.destroyFence(pm_RenderData.InFlightFences[i], nullptr);
        }

        pm_Init.Dispatch.destroyCommandPool(pm_RenderData.CommandPool, nullptr);

        for (auto framebuffer : pm_RenderData.FrameBuffers)
        {
            pm_Init.Dispatch.destroyFramebuffer(framebuffer, nullptr);
        }

        for (auto& __pipeline : pm_RenderData.GraphicsPipelines)
        {
            pm_Init.Dispatch.destroyPipeline(__pipeline.second, nullptr);
        }
        pm_RenderData.GraphicsPipelines.clear();

        for (auto& __layout : pm_RenderData.PipelineLayouts)
        {
            pm_Init.Dispatch.destroyPipelineLayout(__layout.second, nullptr);
        }
        pm_RenderData.PipelineLayouts.clear();

        pm_Init.Dispatch.destroyRenderPass(pm_RenderData.RenderPass, nullptr);

        pm_Init.SwapChain.destroy_image_views(pm_RenderData.SwapChainImageViews);

        vkb::destroy_swapchain(pm_Init.SwapChain);
        vkb::destroy_device(pm_Init.Device);
        vkb::destroy_surface(pm_Init.Instance, pm_Init.Surface);
        vkb::destroy_instance(pm_Init.Instance);

        if (pm_Init.MainWindow) //RenderingManager handles bookeeping and creation, after though VulkanRenderer takes exclusive control over its SDL window
        {
            SDL_DestroyWindow(pm_Init.MainWindow);
            pm_Init.MainWindow = nullptr;
        }
    }

    VkShaderModule
        Renderer::CreateShaderModule
        (
            const vector<uint32_t>& fp_SpirvBytecode
        )
    {
        VkShaderModuleCreateInfo f_CreateInfo{};
        f_CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        f_CreateInfo.codeSize = fp_SpirvBytecode.size() * sizeof(uint32_t);
        f_CreateInfo.pCode = fp_SpirvBytecode.data();

        VkShaderModule f_ShaderModule;
        if (pm_Init.Dispatch.createShaderModule(&f_CreateInfo, nullptr, &f_ShaderModule) != VK_SUCCESS)
        {
            rendering_logger->Error("Failed to create shader module ywy", "VulkanRenderer");
            return VK_NULL_HANDLE; // failed to create shader module
        }

        return f_ShaderModule;
    }

    bool
        Renderer::InitializeDevice(const string& fp_AppName)
    {
        //////////////////// Build Instance ////////////////////

        vkb::InstanceBuilder builder;

        auto inst_ret = builder
            .set_app_name(fp_AppName.c_str())
            .set_engine_name("Peach-E")
            .request_validation_layers(true)
            .require_api_version(1, 2, 0)
            .use_default_debug_messenger()  // Optional, but great for debugging
            .build();

        if (not inst_ret)
        {
            rendering_logger->Fatal("Failed to create Vulkan instance. Error: " + inst_ret.error().message(), "VulkanRenderer");
            return false;
        }

        pm_Init.Instance = inst_ret.value();

        //////////////////// Load Pointers via Volk ////////////////////

        volkLoadInstance(pm_Init.Instance.instance); //ilysm volk ur my goat, all my homies hate manually loading proc addresses

        //////////////////// Create Dispatch Table ////////////////////

        pm_Init.InstanceDispatchTable = pm_Init.Instance.make_table();

        //////////////////// Create SDL Vulkan Surface for Main Window ////////////////////

        //VkAllocationCallbacks callbacks; //idk what to do w this rn so ill just pass NULL -> SDL and figure it out later eh

        if (not SDL_Vulkan_CreateSurface(pm_Init.MainWindow, pm_Init.Instance.instance, nullptr, &pm_Init.Surface))
        {
            rendering_logger->Fatal("Failed to create SDL Vulkan Surface: " + inst_ret.error().message(), "VulkanRenderer");
            return false;
        }

        //////////////////// Create Physical Device ////////////////////

        vkb::PhysicalDeviceSelector phys_device_selector(pm_Init.Instance);

        auto phys_device_ret = phys_device_selector
            .set_surface(pm_Init.Surface)
            .set_minimum_version(1, 2)
            //.add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
            //.add_required_extension(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)
            // .require_dedicated_transfer_queue()
            .select();

        if (not phys_device_ret)
        {
            rendering_logger->Fatal("Failed to select Vulkan Physical Device. Error: " + phys_device_ret.error().message(), "VulkanRenderer");
            return false;
        }

        vkb::DeviceBuilder device_builder{ phys_device_ret.value() };
        auto device_ret = device_builder.build();

        if (not device_ret)
        {
            rendering_logger->Fatal("Failed to create Vulkan device. Error: " + device_ret.error().message(), "VulkanRenderer");
            return false;
        }

        pm_Init.Device = device_ret.value();

        if (pm_Init.Device.physical_device.physical_device == VK_NULL_HANDLE)
        {
            rendering_logger->Fatal("Physical device is null before Nuklear init", "VulkanRenderer");
            return false;
        }

        pm_Init.Dispatch = pm_Init.Device.make_table();

        //////////////////// Return Success ////////////////////

        return true;
    }

    bool
        Renderer::CreateSwapChain()
    {
        vkb::SwapchainBuilder swapchain_builder{ pm_Init.Device };
        auto swap_ret = swapchain_builder
            .set_old_swapchain(pm_Init.SwapChain)
            .set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
            .set_desired_min_image_count(MINIMUM_SWAPCHAIN_SIZE)
            .build();

        if (not swap_ret)
        {
            rendering_logger->Fatal(fmt::format("SwapChain builder error: {}, with result: {}", swap_ret.error().message(), static_cast<int>(swap_ret.vk_result())), "VulkanRenderer");
            return false;
        }

        vkb::destroy_swapchain(pm_Init.SwapChain);
        pm_Init.SwapChain = swap_ret.value();

        return true;
    }

    bool
        Renderer::InitializeQueues()
    {
        auto graphics_queue = pm_Init.Device.get_queue(vkb::QueueType::graphics);

        if (not graphics_queue.has_value())
        {
            rendering_logger->Fatal(fmt::format("failed to get graphics queue: {}", graphics_queue.error().message()), "VulkanRenderer");
            return false;
        }

        pm_RenderData.GraphicsQueue = graphics_queue.value();

        auto present_queue = pm_Init.Device.get_queue(vkb::QueueType::present);

        if (not present_queue.has_value())
        {
            rendering_logger->Fatal(fmt::format("failed to get present queue: {}", present_queue.error().message()), "VulkanRenderer");
            return false;
        }

        pm_RenderData.PresentQueue = present_queue.value();

        return true;
    }

    bool
        Renderer::CreateRenderPass()
    {
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = pm_Init.SwapChain.image_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo f_RenderPassInfo = {};
        f_RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        f_RenderPassInfo.attachmentCount = 1;
        f_RenderPassInfo.pAttachments = &color_attachment;
        f_RenderPassInfo.subpassCount = 1;
        f_RenderPassInfo.pSubpasses = &subpass;
        f_RenderPassInfo.dependencyCount = 1;
        f_RenderPassInfo.pDependencies = &dependency;

        if (pm_Init.Dispatch.createRenderPass(&f_RenderPassInfo, nullptr, &pm_RenderData.RenderPass) != VK_SUCCESS)
        {
            rendering_logger->Fatal("Failed to create render pass, exiting program execution immediately", "VulkanRenderer");
            return false;
        }

        return true;
    }

    bool
        Renderer::CreateGraphicsPipeline(ShaderProgram& fp_ShaderProgram)
    {
        VkShaderModule vert_module = CreateShaderModule(fp_ShaderProgram.pm_GraphicsPipe.ShaderData.Vertex.Bytecode);
        VkShaderModule frag_module = CreateShaderModule(fp_ShaderProgram.pm_GraphicsPipe.ShaderData.Fragment.Bytecode);

        if (vert_module == VK_NULL_HANDLE or frag_module == VK_NULL_HANDLE)
        {
            rendering_logger->Fatal("failed to create shader module, exiting program execution immediately", "VulkanRenderer");
            return false; // failed to create shader modules
        }

        fp_ShaderProgram.pm_GraphicsPipe.VertexStageInfo.module = vert_module;
        fp_ShaderProgram.pm_GraphicsPipe.FragStageInfo.module = frag_module;

        VkPipelineShaderStageCreateInfo shader_stages[] = { fp_ShaderProgram.pm_GraphicsPipe.VertexStageInfo, fp_ShaderProgram.pm_GraphicsPipe.FragStageInfo };

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(pm_Init.SwapChain.extent.width);
        viewport.height = static_cast<float>(pm_Init.SwapChain.extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = pm_Init.SwapChain.extent;

        VkPipelineViewportStateCreateInfo viewport_state = {};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineLayout f_TempLayout;

        if (pm_Init.Dispatch.createPipelineLayout(&fp_ShaderProgram.pm_GraphicsPipe.PipelineLayoutInfo, nullptr, &f_TempLayout) != VK_SUCCESS)
        {
            rendering_logger->Fatal("failed to create pipeline layout, exiting program execution immediately", "VulkanRenderer");
            return false; // failed to create pipeline layout
        }

        pm_RenderData.PipelineLayouts.emplace(fp_ShaderProgram.pm_GraphicsPipe.PipelineName, f_TempLayout);

        fp_ShaderProgram.pm_GraphicsPipe.PipelineInfo.pStages = shader_stages;
        fp_ShaderProgram.pm_GraphicsPipe.PipelineInfo.pViewportState = &viewport_state;
        fp_ShaderProgram.pm_GraphicsPipe.PipelineInfo.layout = f_TempLayout;
        fp_ShaderProgram.pm_GraphicsPipe.PipelineInfo.renderPass = pm_RenderData.RenderPass;

        VkPipeline f_TempGraphicsPipeline;

        if (pm_Init.Dispatch.createGraphicsPipelines(VK_NULL_HANDLE, 1, &fp_ShaderProgram.pm_GraphicsPipe.PipelineInfo, nullptr, &f_TempGraphicsPipeline) != VK_SUCCESS)
        {
            rendering_logger->Fatal("failed to create pipline, exiting program execution immediately", "VulkanRenderer");
            return false; // failed to create graphics pipeline
        }
        pm_RenderData.GraphicsPipelines.emplace(fp_ShaderProgram.pm_GraphicsPipe.PipelineName, f_TempGraphicsPipeline);

        pm_Init.Dispatch.destroyShaderModule(frag_module, nullptr);
        pm_Init.Dispatch.destroyShaderModule(vert_module, nullptr);

        return true;
    }

    bool
        Renderer::CreateFrameBuffer()
    {
        pm_RenderData.SwapChainImages = pm_Init.SwapChain.get_images().value();
        pm_RenderData.SwapChainImageViews = pm_Init.SwapChain.get_image_views().value();

        pm_RenderData.FrameBuffers.resize(pm_RenderData.SwapChainImageViews.size());

        for (size_t i = 0; i < pm_RenderData.SwapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = { pm_RenderData.SwapChainImageViews[i] };

            VkFramebufferCreateInfo framebuffer_info = {};
            framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass = pm_RenderData.RenderPass;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.pAttachments = attachments;
            framebuffer_info.width = pm_Init.SwapChain.extent.width;
            framebuffer_info.height = pm_Init.SwapChain.extent.height;
            framebuffer_info.layers = 1;

            if (pm_Init.Dispatch.createFramebuffer(&framebuffer_info, nullptr, &pm_RenderData.FrameBuffers[i]) != VK_SUCCESS)
            {
                rendering_logger->Fatal("failed to create default framebuffers, exiting program execution immediately", "VulkanRenderer");
                return false; // failed to create framebuffer
            }
        }

        return true;
    }

    bool
        Renderer::CreateCommandPool()
    {
        VkCommandPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = pm_Init.Device.get_queue_index(vkb::QueueType::graphics).value();

        if (pm_Init.Dispatch.createCommandPool(&pool_info, nullptr, &pm_RenderData.CommandPool) != VK_SUCCESS)
        {
            rendering_logger->Fatal("failed to create command pool, exiting program execution immediately", "VulkanRenderer");
            return false; // failed to create command pool
        }

        return true;
    }

    bool
        Renderer::CreateCommandBuffers()
    {
        pm_RenderData.CommandBuffers.resize(pm_RenderData.FrameBuffers.size());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = pm_RenderData.CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(pm_RenderData.CommandBuffers.size());

        if (pm_Init.Dispatch.allocateCommandBuffers(&allocInfo, pm_RenderData.CommandBuffers.data()) != VK_SUCCESS)
        {
            rendering_logger->Fatal("failed to allocate command buffers, exiting program execution immediately", "VulkanRenderer");
            return false; // failed to allocate command buffers;
        }

        return true;
    }

    bool
        Renderer::CreateSyncObjects()
    {
        pm_RenderData.AvailableSemaphores.resize(pm_Init.SwapChain.image_count);
        pm_RenderData.FinishedSemaphores.resize(pm_Init.SwapChain.image_count);
        pm_RenderData.InFlightFences.resize(pm_Init.SwapChain.image_count);
        pm_RenderData.ImageInFlight.resize(pm_Init.SwapChain.image_count, VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info = {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < pm_Init.SwapChain.image_count; i++)
        {
            if (
                pm_Init.Dispatch.createSemaphore(&semaphore_info, nullptr, &pm_RenderData.AvailableSemaphores[i]) != VK_SUCCESS or
                pm_Init.Dispatch.createSemaphore(&semaphore_info, nullptr, &pm_RenderData.FinishedSemaphores[i]) != VK_SUCCESS or
                pm_Init.Dispatch.createFence(&fence_info, nullptr, &pm_RenderData.InFlightFences[i]) != VK_SUCCESS
                )
            {
                rendering_logger->Fatal("failed to create sync objects, exiting program execution immediately", "VulkanRenderer");
                return false; // failed to create synchronization objects for a frame
            }
        }

        return true;
    }

    bool
        Renderer::RecreateSwapChain()
    {
        pm_Init.Dispatch.deviceWaitIdle();

        pm_Init.Dispatch.destroyCommandPool(pm_RenderData.CommandPool, nullptr);

        for (auto framebuffer : pm_RenderData.FrameBuffers)
        {
            pm_Init.Dispatch.destroyFramebuffer(framebuffer, nullptr);
        }

        pm_Init.SwapChain.destroy_image_views(pm_RenderData.SwapChainImageViews);

        if (not CreateSwapChain()) return false;
        if (not CreateFrameBuffer()) return false;
        if (not CreateCommandPool()) return false;
        if (not CreateCommandBuffers()) return false;

        return true;
    }
}//namespace PeachCore::Vulkan

#endif /*PEACH_RENDERER_VULKAN*/