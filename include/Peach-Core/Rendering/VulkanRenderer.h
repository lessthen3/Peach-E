/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR

///PeachCore
#include "ShaderUtils.h"

///Vulkan
#include <vma/vk_mem_alloc.h>

///SDL
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_vulkan.h>

///STL
#include <unordered_map>

constexpr const int MAX_FRAMES_IN_FLIGHT = 2;

namespace PeachCore{

    struct VulkanRenderer 
    {
    public:
        VulkanRenderer() = default; //default construction, use initialize function so we can avoid unecessary copy semantics

        ~VulkanRenderer() = default; //idrc ab RAII here since VulkanRenderer will live for the entire program runtime, so cleaning it up is an after thought
        //we can just let the OS or driver handle it idrc
    private:
        struct Initializer //POD that contains all initialized data
        {
            SDL_Window* MainWindow = nullptr; //safe ptr use UwU
            vkb::Instance Instance;
            vkb::InstanceDispatchTable InstanceDispatchTable;
            VkSurfaceKHR Surface;
            vkb::Device Device;
            vkb::DispatchTable Dispatch;
            vkb::Swapchain SwapChain;
        };

        struct RenderData
        {
            VkQueue GraphicsQueue = {};
            VkQueue PresentQueue = {};

            vector<VkImage> SwapChainImages;
            vector<VkImageView> SwapChainImageViews;
            vector<VkFramebuffer> FrameBuffers;

            VkRenderPass RenderPass;
            unordered_map<string, VkPipelineLayout> PipelineLayouts;
            unordered_map<string, VkPipeline> GraphicsPipelines;

            unordered_map<string, VkDescriptorSetLayout> DescriptorSetLayouts;
            unordered_map<string, VkDescriptorSet> DescriptorSets;

            //Used for drawing a default texture if for some reason the texture is missing or failed to load
            VkSampler DefaultSampler = {};
            VkImageView DefaultTextureView = {};
            VkImage DefaultTexture = {};

            VkImage DepthImage = {};
            VkImageView DepthImageView = {};
            VkDeviceMemory DepthImageMemory = {};

            VkCommandPool CommandPool;
            vector<VkCommandBuffer> CommandBuffers;

            vector<VkSemaphore> AvailableSemaphores;
            vector<VkSemaphore> FinishedSemaphores;
            vector<VkFence> InFlightFences;
            vector<VkFence> ImageInFlight;
            size_t CurrentFrameNumber = 0;
        };

    private:
        Initializer pm_Init;
        RenderData pm_RenderData;

        VmaAllocator pm_Allocator;
        VkPhysicalDevice pm_PhysicalDevice;
        VkDescriptorPool pm_DescriptorPool;

        shared_ptr<LogManager> rendering_logger = nullptr;

    public:
        bool
            Initialize //used for lazy initialization and for default constructor support without needing to define an explicit move constructor UwU
            (
                SDL_Window* fp_MainWindow,
                ShaderUtils::BakedPipelineData& fp_BakedPipeline,
                shared_ptr<LogManager> fp_RenderingLogger
            )
        {
            if (not fp_RenderingLogger) //MAYBE: maybe we should just create a new logger actually nvm that involves getting a reference to the console lmfao
            {
                PrintError("Tried to initialize VulkanRenderer with a nullptr for the Rendering Logger doofus, Ending program execution immediately since no valid logger was found");
                return false;
            }

            rendering_logger = fp_RenderingLogger;

            if (not fp_MainWindow)
            {
                rendering_logger->LogAndPrint("Tried to initialize VulkanRenderer with a nullptr for the SDL Window doofus, Ending program execution immediately since no valid SDL Window was found", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            pm_Init.MainWindow = fp_MainWindow;

            if (not InitializeDevice())
            {
                rendering_logger->LogAndPrint("Failed to create Vulkan device, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
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
            else if (not CreateGraphicsPipeline(fp_BakedPipeline))
            {

                return false; //>w<
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

            return true; //rawr UwU forgot this return path zzzzzzzzzzzz
        }

        bool 
            DrawFrame() 
        {
            pm_Init.Dispatch.waitForFences(1, &pm_RenderData.InFlightFences[pm_RenderData.CurrentFrameNumber], VK_TRUE, UINT64_MAX);

            uint32_t image_index = 0;
            VkResult result = pm_Init.Dispatch.acquireNextImageKHR(pm_Init.SwapChain, UINT64_MAX, pm_RenderData.AvailableSemaphores[pm_RenderData.CurrentFrameNumber], VK_NULL_HANDLE, &image_index);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) 
            {
                return RecreateSwapChain();
            }
            else if (result != VK_SUCCESS and result != VK_SUBOPTIMAL_KHR) 
            {
                rendering_logger->LogAndPrint(format("failed to acquire swapchain image. Error: {} ", static_cast<int>(result)), "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            if (pm_RenderData.ImageInFlight[image_index] != VK_NULL_HANDLE)
            {
                pm_Init.Dispatch.waitForFences(1, &pm_RenderData.ImageInFlight[image_index], VK_TRUE, UINT64_MAX);
            }

            pm_RenderData.ImageInFlight[image_index] = pm_RenderData.InFlightFences[pm_RenderData.CurrentFrameNumber];

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore wait_semaphores[] = { pm_RenderData.AvailableSemaphores[pm_RenderData.CurrentFrameNumber] };
            VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = wait_semaphores;
            submitInfo.pWaitDstStageMask = wait_stages;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &pm_RenderData.CommandBuffers[image_index];

            VkSemaphore signal_semaphores[] = { pm_RenderData.FinishedSemaphores[pm_RenderData.CurrentFrameNumber] };
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signal_semaphores;

            pm_Init.Dispatch.resetFences(1, &pm_RenderData.InFlightFences[pm_RenderData.CurrentFrameNumber]);

            if (pm_Init.Dispatch.queueSubmit(pm_RenderData.GraphicsQueue, 1, &submitInfo, pm_RenderData.InFlightFences[pm_RenderData.CurrentFrameNumber]) != VK_SUCCESS) 
            {
                rendering_logger->LogAndPrint("Failed to submit draw command buffer", "VulkanRenderer", LogManager::LogLevel::Error);
                return false; //"failed to submit draw command buffer
            }

            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = signal_semaphores;

            VkSwapchainKHR swapChains[] = { pm_Init.SwapChain };
            present_info.swapchainCount = 1;
            present_info.pSwapchains = swapChains;

            present_info.pImageIndices = &image_index;

            result = pm_Init.Dispatch.queuePresentKHR(pm_RenderData.PresentQueue, &present_info);
            if (result == VK_ERROR_OUT_OF_DATE_KHR or result == VK_SUBOPTIMAL_KHR) 
            {
                return RecreateSwapChain();
            }
            else if (result != VK_SUCCESS) 
            {
                rendering_logger->LogAndPrint("Failed to present swapchain image", "VulkanRenderer", LogManager::LogLevel::Error);
                return false;
            }

            pm_RenderData.CurrentFrameNumber = (pm_RenderData.CurrentFrameNumber + 1) % MAX_FRAMES_IN_FLIGHT;

            return true;
        }

    private:
        
        void
            CleanUp()
        {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
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

            for(auto& __pipeline : pm_RenderData.GraphicsPipelines)
            {
                pm_Init.Dispatch.destroyPipeline(__pipeline.second, nullptr);
            }
            pm_RenderData.GraphicsPipelines.clear();

            for(auto& __layout : pm_RenderData.PipelineLayouts)
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
            CreateShaderModule
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
                return VK_NULL_HANDLE; // failed to create shader module
            }

            return f_ShaderModule;
        }

        bool
            InitializeDevice()
        {
            vkb::InstanceBuilder builder;

            auto inst_ret = builder
                .set_app_name("Game")
                .set_engine_name("Peach-E")
                .request_validation_layers(true)
                .require_api_version(1, 2, 0)
                .use_default_debug_messenger()  // Optional, but great for debugging
                .build();

            if (not inst_ret)
            {
                rendering_logger->LogAndPrint("Failed to create Vulkan instance. Error: " + inst_ret.error().message(), "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            pm_Init.Instance = inst_ret.value();

            pm_Init.InstanceDispatchTable = pm_Init.Instance.make_table();

            //VkAllocationCallbacks callbacks; //idk what to do w this rn so ill just pass NULL -> SDL and figure it out later eh

            if (not SDL_Vulkan_CreateSurface(pm_Init.MainWindow, pm_Init.Instance.instance, nullptr, &pm_Init.Surface))
            {
                rendering_logger->LogAndPrint("Failed to create SDL Vulkan Surface: " + inst_ret.error().message(), "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            vkb::PhysicalDeviceSelector phys_device_selector(pm_Init.Instance);

            auto phys_device_ret = phys_device_selector
                .set_surface(pm_Init.Surface)
                .set_minimum_version(1, 1)
                .require_dedicated_transfer_queue()
                .select();

            if (not phys_device_ret)
            {
                rendering_logger->LogAndPrint("Failed to select Vulkan Physical Device. Error: " + phys_device_ret.error().message(), "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            vkb::DeviceBuilder device_builder{ phys_device_ret.value() };
            auto device_ret = device_builder.build();

            if (not device_ret)
            {
                rendering_logger->LogAndPrint("Failed to create Vulkan device. Error: " + device_ret.error().message(), "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            pm_Init.Device = device_ret.value();
            pm_Init.Dispatch = pm_Init.Device.make_table();

            return true;
        }

        bool
            CreateSwapChain()
        {
            vkb::SwapchainBuilder swapchain_builder{ pm_Init.Device };
            auto swap_ret = swapchain_builder.set_old_swapchain(pm_Init.SwapChain).build();

            if (not swap_ret) 
            {
                rendering_logger->LogAndPrint(format("SwapChain builder error: {}, with result: {}", swap_ret.error().message(), static_cast<int>(swap_ret.vk_result())), "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            vkb::destroy_swapchain(pm_Init.SwapChain);
            pm_Init.SwapChain = swap_ret.value();

            return true;
        }

        bool
            InitializeQueues()
        {
            auto graphics_queue = pm_Init.Device.get_queue(vkb::QueueType::graphics);

            if (not graphics_queue.has_value())
            {
                rendering_logger->LogAndPrint(format("failed to get graphics queue: {}", graphics_queue.error().message()), "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            pm_RenderData.GraphicsQueue = graphics_queue.value();

            auto present_queue = pm_Init.Device.get_queue(vkb::QueueType::present);

            if (not present_queue.has_value())
            {
                rendering_logger->LogAndPrint(format("failed to get present queue: {}", present_queue.error().message()), "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            pm_RenderData.PresentQueue = present_queue.value();

            return true;
        }

        bool 
            CreateRenderPass() 
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
                rendering_logger->LogAndPrint("Failed to create render pass, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false;
            }

            return true;
        }

        bool
            CreateGraphicsPipeline(ShaderUtils::BakedPipelineData& fp_BakedPipeline)
        {
            VkShaderModule vert_module = CreateShaderModule(fp_BakedPipeline.VertexSPV);
            VkShaderModule frag_module = CreateShaderModule(fp_BakedPipeline.FragmentSPV);

            if (vert_module == VK_NULL_HANDLE or frag_module == VK_NULL_HANDLE) 
            {
                rendering_logger->LogAndPrint("failed to create shader module, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false; // failed to create shader modules
            }
           
            fp_BakedPipeline.VertexStageInfo.module = vert_module;
            fp_BakedPipeline.FragStageInfo.module = frag_module;

            VkPipelineShaderStageCreateInfo shader_stages[] = { fp_BakedPipeline.VertexStageInfo, fp_BakedPipeline.FragStageInfo };

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

            if (pm_Init.Dispatch.createPipelineLayout(&fp_BakedPipeline.PipelineLayoutInfo, nullptr, &f_TempLayout) != VK_SUCCESS)
            {
                rendering_logger->LogAndPrint("failed to create pipeline layout, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false; // failed to create pipeline layout
            }

            pm_RenderData.PipelineLayouts.emplace("name", f_TempLayout);

            fp_BakedPipeline.PipelineInfo.pStages = shader_stages;
            fp_BakedPipeline.PipelineInfo.pViewportState = &viewport_state;
            fp_BakedPipeline.PipelineInfo.layout = f_TempLayout;
            fp_BakedPipeline.PipelineInfo.renderPass = pm_RenderData.RenderPass;

            VkPipeline f_TempGraphicsPipeline;

            if (pm_Init.Dispatch.createGraphicsPipelines(VK_NULL_HANDLE, 1, &fp_BakedPipeline.PipelineInfo, nullptr, &f_TempGraphicsPipeline) != VK_SUCCESS)
            {
                rendering_logger->LogAndPrint("failed to create pipline, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false; // failed to create graphics pipeline
            }
            pm_RenderData.GraphicsPipelines.emplace("name", f_TempGraphicsPipeline);

            pm_Init.Dispatch.destroyShaderModule(frag_module, nullptr);
            pm_Init.Dispatch.destroyShaderModule(vert_module, nullptr);

            return true;
        }

        bool 
            CreateFrameBuffer() 
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
                    rendering_logger->LogAndPrint("failed to create default framebuffers, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                    return false; // failed to create framebuffer
                }
            }

            return true;
        }

        bool 
            CreateCommandPool() 
        {
            VkCommandPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            pool_info.queueFamilyIndex = pm_Init.Device.get_queue_index(vkb::QueueType::graphics).value();

            if (pm_Init.Dispatch.createCommandPool(&pool_info, nullptr, &pm_RenderData.CommandPool) != VK_SUCCESS) 
            {
                rendering_logger->LogAndPrint("failed to create command pool, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false; // failed to create command pool
            }

            return true;
        }

        bool 
            CreateCommandBuffers() 
        {
            pm_RenderData.CommandBuffers.resize(pm_RenderData.FrameBuffers.size());

            VkCommandBufferAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = pm_RenderData.CommandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = static_cast<uint32_t>(pm_RenderData.CommandBuffers.size());

            if (pm_Init.Dispatch.allocateCommandBuffers(&allocInfo, pm_RenderData.CommandBuffers.data()) != VK_SUCCESS) 
            {
                rendering_logger->LogAndPrint("failed to allocate command buffers, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false; // failed to allocate command buffers;
            }

            for (size_t i = 0; i < pm_RenderData.CommandBuffers.size(); i++) 
            {
                VkCommandBufferBeginInfo begin_info = {};
                begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                if (pm_Init.Dispatch.beginCommandBuffer(pm_RenderData.CommandBuffers[i], &begin_info) != VK_SUCCESS)
                {
                    rendering_logger->LogAndPrint("failed to begin recording command buffer, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                    return false; // failed to begin recording command buffer
                }

                VkRenderPassBeginInfo render_pass_info = {};
                render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                render_pass_info.renderPass = pm_RenderData.RenderPass;
                render_pass_info.framebuffer = pm_RenderData.FrameBuffers[i];
                render_pass_info.renderArea.offset = { 0, 0 };
                render_pass_info.renderArea.extent = pm_Init.SwapChain.extent;
                VkClearValue clearColor{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };
                render_pass_info.clearValueCount = 1;
                render_pass_info.pClearValues = &clearColor;

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

                pm_Init.Dispatch.cmdSetViewport(pm_RenderData.CommandBuffers[i], 0, 1, &viewport);
                pm_Init.Dispatch.cmdSetScissor(pm_RenderData.CommandBuffers[i], 0, 1, &scissor);

                pm_Init.Dispatch.cmdBeginRenderPass(pm_RenderData.CommandBuffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

                pm_Init.Dispatch.cmdBindPipeline(pm_RenderData.CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pm_RenderData.GraphicsPipelines.at("name"));

                pm_Init.Dispatch.cmdDraw(pm_RenderData.CommandBuffers[i], 3, 1, 0, 0);

                pm_Init.Dispatch.cmdEndRenderPass(pm_RenderData.CommandBuffers[i]);

                if (pm_Init.Dispatch.endCommandBuffer(pm_RenderData.CommandBuffers[i]) != VK_SUCCESS)
                {
                    rendering_logger->LogAndPrint("failed to record command buffer, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                    return false; // failed to record command buffer!
                }
            }

            return true;
        }

        bool 
            CreateSyncObjects() 
        {
            pm_RenderData.AvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            pm_RenderData.FinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            pm_RenderData.InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
            pm_RenderData.ImageInFlight.resize(pm_Init.SwapChain.image_count, VK_NULL_HANDLE);

            VkSemaphoreCreateInfo semaphore_info = {};
            semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fence_info = {};
            fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
            {
                if (
                    pm_Init.Dispatch.createSemaphore(&semaphore_info, nullptr, &pm_RenderData.AvailableSemaphores[i]) != VK_SUCCESS or
                    pm_Init.Dispatch.createSemaphore(&semaphore_info, nullptr, &pm_RenderData.FinishedSemaphores[i]) != VK_SUCCESS or
                    pm_Init.Dispatch.createFence(&fence_info, nullptr, &pm_RenderData.InFlightFences[i]) != VK_SUCCESS
                   )
                {
                    rendering_logger->LogAndPrint("failed to create sync objects, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                    return false; // failed to create synchronization objects for a frame
                }
            }

            return true;
        }

        bool 
            RecreateSwapChain() 
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
    };
}