/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///PeachCore
#include "../Utils/ShaderUtils.h"

///Vulkan
#include <volk.h>
#include <vma/vk_mem_alloc.h>
#include "vk-bootstrap/VkBootstrap.h"

///SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>

///STL
#include <unordered_map>

///PUI
#include "../Scene-Items/UI/Button.h"

#include <physfs.h> //this shouldnt be her but is for testing UWU

constexpr uint32_t MINIMUM_SWAPCHAIN_SIZE = 2;

namespace PeachCore{

    struct VulkanRenderer 
    {
    public:
        VulkanRenderer() = default; //default construction, use initialize function so we can avoid unecessary copy semantics

        ~VulkanRenderer() = default; //idrc ab RAII here since VulkanRenderer will live for the entire program runtime, so cleaning it up is an after thought
        //we can just let the OS or driver handle it idrc

        enum StatusCode : uint32_t
        {
            OK = 1 << 0U, //ok uwu

            BEGIN_FRAME_CALLED_WHILE_FRAME_IS_ALREADY_STARTED = 1 << 1U,
            NO_VALID_RENDERING_SURFACE = 1 << 2U,
            RECREATED_SWAPCHAIN_SUCCESSFULLY = 1 << 3U,

            FAILED_TO_RECREATE_SWAPCHAIN_ERROR = 1 << 4U,
            DRAW_FRAME_BEFORE_BEGIN_FRAME_ERROR = 1 << 5U,
            FAILED_TO_ACQUIRE_NEXT_SWAPCHAIN_IMAGE_ERROR = 1 << 6U,
            FAILED_TO_BEGIN_COMMAND_BUFFER_ERROR = 1 << 7U,
            FAILED_TO_SUBMIT_DRAW_COMMAND_BUFFER = 1 << 8U,

            NOT_VULKAN_SUCCESS = 1 << 9U,
            SUBOPTIMAL_VULKAN_KHR = 1 << 10U,
            OUT_OF_DATE_VULKAN_KHR = 1 << 11U,

            FAILED_TO_END_COMMAND_BUFFER = 1 << 12U,
            END_FRAME_CALLED_WHEN_FRAME_WASNT_STARTED_ERROR = 1 << 13U
        };

        struct VulkanShaderAsset
        {
            VkPipeline Pipeline;
            VkPipelineLayout Layout;
            VkShaderModule VertShader;
            VkShaderModule FragShader;

            // Optionally: descriptorSetLayout, pipelineCache, etc.
            string Name; // For debugging/user selection
        };

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

        struct GraphicsPipe
        {
            //unordered_map<string, VkPipelineLayout> PipelineLayouts;
            //unordered_map<string, VkPipeline> GraphicsPipelines;
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

            uint32_t CurrentFrameCycle = 0; //used for cycling available resources independent of swapchain image
            uint32_t CurrentSwapchainImageIndex = 0;

            int CurrentWindowWidth = 0;
            int CurrentWindowHeight = 0;

            //bool WasSwapchainRecreatedLastFrame = false;
        };

    private:
        Initializer pm_Init;
        RenderData pm_RenderData;

        VmaAllocator pm_Allocator;

        VkDescriptorPool pm_DescriptorPool;

        bool pm_IsFrameStarted = false;

        shared_ptr<Logger> rendering_logger = nullptr;

    public:
        //////////////////// Initialize Method ////////////////////

        bool
            Initialize //used for lazy initialization and for default constructor support without needing to define an explicit move constructor UwU
            (
                SDL_Window* fp_MainWindow,
                ShaderUtils::BakedPipelineData& fp_BakedPipeline,
                shared_ptr<Logger> fp_RenderingLogger
            );

        //////////////////// Setup Peach UI ////////////////////

        bool
            InitializePeachUI();

        //////////////////// Frame Rendering Functions ////////////////////

        uint32_t
            BeginFrame();

        uint32_t
            DrawFrame();

        uint32_t
            EndFrame();

        //////////////////// Shader Stuff ////////////////////

        VkShaderModule
            CreateShaderModule
            (
                const vector<uint32_t>& fp_SpirvBytecode
            );

        //////////////////// Clean Up Resources ////////////////////

        void
            CleanUp();
    private:

        bool
            InitializeDevice(const string& fp_AppName);

        bool
            CreateSwapChain();

        bool
            InitializeQueues();

        bool
            CreateRenderPass();

        bool
            CreateGraphicsPipeline(ShaderUtils::BakedPipelineData& fp_BakedPipeline);

        bool
            CreateFrameBuffer();

        bool
            CreateCommandPool();

        bool
            CreateCommandBuffers();

        bool
            CreateSyncObjects();

        bool
            RecreateSwapChain();
    };
}

namespace PeachCore{

    // -- Peach UI System --
class PeachUIManager
{
    // public:
    //     unique_ptr<PeachUserInterfaceNode> root;
    //     shared_ptr<Logger> pui_logger = nullptr;

    //     PeachUIManager() 
    //     {
    //         root = make_unique<PeachUserInterfaceNode>(NodeType::Root);
    //         root->m_Rectangle = {0, 0, 1920, 1080}; // Example
    //     }

        // Walk tree, collect visible nodes, output draw data for batching
        // void collectDrawCommands(vector<YourDrawCommand>& outCmds) 
        // {
        //     collectDrawCommandsRecursive(root.get(), outCmds);
        // }

        // // Recursive collection (do layout/visibility/etc)
        // void collectDrawCommandsRecursive(PeachUINode* node, vector<YourDrawCommand>& outCmds)
        //  {
        //     // Build YourDrawCommand from node (rect, style, text, image, etc)
        //     // For each child:
        //     for (auto& child : node->children) {
        //         collectDrawCommandsRecursive(child.get(), outCmds);
        //     }
        // }

        // Hit-testing for input
        // PeachUserInterfaceNode* HitTest(float x, float y)
        // {
        //     // Walk tree, return node under point (for mouse events)
        //     return nullptr;
        // }
    };
}