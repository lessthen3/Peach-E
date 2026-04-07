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
#pragma once

#ifdef PEACH_RENDERER_VULKAN

///PeachCore
#include "VulkanShaderProgram.h" //THIS NEEDS TO BE FIRST DO NOT MOVE OWO

///Vulkan
#include <vma/vk_mem_alloc.h>

///SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>

///STL
#include <unordered_map>

///PUI
#include "../../Scene-Items/UI/Button.h"

constexpr uint32_t MINIMUM_SWAPCHAIN_SIZE = 2;

namespace PeachCore::Vulkan{

    struct PhysicalDeviceInfo
    {
        ////////////////////// Device Identity //////////////////////

        string         m_DeviceName;
        uint32_t       m_ApiVersion = 0;
        uint32_t       m_DriverVersion = 0;
        uint32_t       m_VendorID = 0;
        uint32_t       m_DeviceID = 0;
        VkDeviceSize   m_VRAMBytes = 0;

        VkPhysicalDeviceType m_DeviceType = VK_PHYSICAL_DEVICE_TYPE_OTHER;

        ////////////////////// Feature Flags //////////////////////

        bool m_HasGeometryShader = false;
        bool m_HasTessellationShader = false;
        bool m_HasAnisotropy = false;
        bool m_HasWideLines = false;
        bool m_HasFillModeNonSolid = false;
        bool m_HasMultiViewport = false;

        ////////////////////// Construction //////////////////////

        void
            ReadDevice(VkPhysicalDevice fp_Device)
        {
            VkPhysicalDeviceProperties f_Props;
            vkGetPhysicalDeviceProperties(fp_Device, &f_Props);

            VkPhysicalDeviceFeatures f_Features;
            vkGetPhysicalDeviceFeatures(fp_Device, &f_Features);

            VkPhysicalDeviceMemoryProperties f_MemProps;
            vkGetPhysicalDeviceMemoryProperties(fp_Device, &f_MemProps);

            m_DeviceName = string(f_Props.deviceName);
            m_ApiVersion = f_Props.apiVersion;
            m_DriverVersion = f_Props.driverVersion;
            m_VendorID = f_Props.vendorID;
            m_DeviceID = f_Props.deviceID;
            m_DeviceType = f_Props.deviceType;

            for (uint32_t lv_Idx = 0; lv_Idx < f_MemProps.memoryHeapCount; lv_Idx++)
            {
                if (f_MemProps.memoryHeaps[lv_Idx].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    m_VRAMBytes += f_MemProps.memoryHeaps[lv_Idx].size;
                }
            }

            m_HasGeometryShader = f_Features.geometryShader;
            m_HasTessellationShader = f_Features.tessellationShader;
            m_HasAnisotropy = f_Features.samplerAnisotropy;
            m_HasWideLines = f_Features.wideLines;
            m_HasFillModeNonSolid = f_Features.fillModeNonSolid;
            m_HasMultiViewport = f_Features.multiViewport;
        }

        ////////////////////// String Formatting //////////////////////

        [[nodiscard]] string
            DeviceTypeString() 
            const
        {
            switch (m_DeviceType)
            {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "Discrete GPU";
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "Virtual GPU";
            case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "CPU";
            default:                                     return "Unknown";
            }
        }

        [[nodiscard]] string
            FormatAsString() 
            const
        {
            const uint32_t lv_Major = VK_API_VERSION_MAJOR(m_ApiVersion);
            const uint32_t lv_Minor = VK_API_VERSION_MINOR(m_ApiVersion);
            const uint32_t lv_Patch = VK_API_VERSION_PATCH(m_ApiVersion);

            const float lv_VRAMMb = static_cast<float>(m_VRAMBytes) / (1024.0f * 1024.0f);

            auto lv_Bool = [](bool fp_Val) -> const char* { return fp_Val ? "yes" : "no"; };

            return
                "=== Physical Device ===\n"
                "  Name:           " + m_DeviceName + "\n"
                "  Type:           " + DeviceTypeString() + "\n"
                "  Vulkan API:     " + to_string(lv_Major) + "." + to_string(lv_Minor) + "." + to_string(lv_Patch) + "\n"
                "  VRAM:           " + to_string(static_cast<uint32_t>(lv_VRAMMb)) + " MB\n"
                "  VendorID:       " + to_string(m_VendorID) + "\n"
                "  DeviceID:       " + to_string(m_DeviceID) + "\n"
                "=== Features ===\n"
                "  Geometry Shader:     " + lv_Bool(m_HasGeometryShader) + "\n"
                "  Tessellation Shader: " + lv_Bool(m_HasTessellationShader) + "\n"
                "  Anisotropy:          " + lv_Bool(m_HasAnisotropy) + "\n"
                "  Wide Lines:          " + lv_Bool(m_HasWideLines) + "\n"
                "  Fill Mode Non-Solid: " + lv_Bool(m_HasFillModeNonSolid) + "\n"
                "  Multi Viewport:      " + lv_Bool(m_HasMultiViewport) + "\n";
        }
    };

    struct Renderer 
    {
    public:
        Renderer() = default; //default construction, use initialize function so we can avoid unecessary copy semantics

        ~Renderer() = default; //idrc ab RAII here since VulkanRenderer will live for the entire program runtime, so cleaning it up is an after thought
        //we can just let the OS or driver handle it idrc

        PhysicalDeviceInfo m_DeviceInfo;

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

        struct TextureSlot //wrapped in the PeachCore::Vulkan namespace so is kosher >~<
        {
            VkImage Image = VK_NULL_HANDLE;
            VkImageView ImageView = VK_NULL_HANDLE;
            VkDeviceMemory Memory = VK_NULL_HANDLE;

            uint32_t Generation = 0; // bumped on free
            bool InUse = false;
        };

        struct MeshSlot
        {
            VkBuffer VertexBuffer;
            VkBuffer IndexBuffer;
            VkDeviceMemory Memory;

            uint32_t Generation = 0; // bumped on free
            bool InUse = false;
        };

        struct RenderObject
        {

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

        vector<TextureSlot> pm_TextureSlots;
        vector<MeshSlot> pm_MeshSlots;

    public:
        //////////////////// Initialize Method ////////////////////

        bool
            Initialize //used for lazy initialization and for default constructor support without needing to define an explicit move constructor UwU
            (
                SDL_Window* fp_MainWindow,
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
            CreateGraphicsPipeline(ShaderProgram& fp_ShaderProgram);

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
}//namespace PeachCore::Vulkan

#endif /*PEACH_RENDERER_VULKAN*/