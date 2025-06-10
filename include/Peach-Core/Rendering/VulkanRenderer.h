/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
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

#include <physfs.h> //this shouldnt be her but is for testing UWU

constexpr uint32_t MINIMUM_SWAPCHAIN_SIZE = 2;

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

        shared_ptr<LogManager> rendering_logger = nullptr;

    public:
        bool
            Initialize //used for lazy initialization and for default constructor support without needing to define an explicit move constructor UwU
            (
                SDL_Window* fp_MainWindow,
                ShaderUtils::BakedPipelineData& fp_BakedPipeline,
                shared_ptr<LogManager> fp_RenderingLogger
            );

        ////////////////////////////////////////////////
        // Setup Nuklear GUI
        ////////////////////////////////////////////////
        bool
            InitializePeachUI();

        bool
            BeginFrame();

        bool
            DrawFrame();

        bool
            EndFrame();

        VkShaderModule
            CreateShaderModule
            (
                const vector<uint32_t>& fp_SpirvBytecode
            );

    private:
        
        void
            CleanUp();

        bool
            InitializeDevice();

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
