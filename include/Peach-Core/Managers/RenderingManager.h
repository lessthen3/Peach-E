////////////////////////////////////////////////////////
/***                             [Peach Core]                                ***/
////////////////////////////////////////////////////////
/***                            [Version 0.0.01]                             ***/
////////////////////////////////////////////////////////
/***  Copyright(c) 2024-present Ranyodh Singh Mandur  ***/
/***                               MIT License                                ***/
/***         Documentation: TBD                                          ***/
/*** GitHub: https://github.com/iLoveJohnFish/Peach-E ***/
/////////////////////////////////////////////////////////
#pragma once

///PeachCore
#include "ResourceManager.h"

#ifndef __APPLE__
#include <Rendering/OpenGLRenderer.h>
#endif

#include <Rendering/VulkanRenderer.h>

#include "../Scene-Items/2D/PeachTexture2D.h"
#include "../Scene-Items/PeachNode.h"

#include "InputManager.h"

namespace PeachCore {

    //////////////////////////////////////////////
    // Drawable Object Struct
    //////////////////////////////////////////////
    //holds all relevant information that the renderer needs to know
    struct DrawableObject2D
    {
        string ObjectID;

        //used for lerping when the FPS > physics ticks per frame
        glm::vec2 CurrentFramePosition;
        glm::vec2 PreviousFramePosition;

        uint32_t LayerNumber = 0; //can't imagine there'll be more than 4 billion drawing layers, at that point integer overflow is the least of ur worries lmfao

        bool IsVisible = true;
        bool IsQueuedForRemoval = false;
        
        //TODO: fix this, need to just hold handles and metadata that maps -> descriptor sets, pipeline info
        uint32_t TextureHandle; //actual data for graphic //used for parsing raw byte information, mainly for audio at the moment

        glm::vec2 Offset; //handles texture offset for atlas stuff and maybe others idfk
            //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
        //Drawable GraphicsType; 
        //WARNING THIS NEEDS TO BE SWITCHED OFF FOR APPLE BUILDS SINCE TIM APPLE DECIDED NOT TO SUPPORT OPENGL ANYMORE UWU
        // OpenGLShaderProgram Shaders; //Contains multiple shaders relevant to drawing the object
    };

    //////////////////////////////////////////////
    // Rendering backend helper Enum
    //////////////////////////////////////////////

    enum class RendererType : int
    {
        OpenGL,
        Vulkan
    };

    //////////////////////////////////////////////
    // Rendering word size
    //////////////////////////////////////////////

    struct RenderCommand 
    {
        uint32_t node_id;       // 4 bytes
        uint16_t opcode;        // 2 bytes
        uint16_t reserved;      // 2 bytes (alignment or flags)
        uint64_t operand;       // 8 bytes
    };

    //////////////////////////////////////////////
    // Rendering OPCODES
    //////////////////////////////////////////////

/*                            | Name                 | ID |   Operand                | 
                               | ---------------- - | ---- | ---------------------- | 
*/
    constexpr uint8_t RENDER_NO_OP = 0x00;
    constexpr uint8_t RENDER_CREATE_NODE_OP = 0x01;//| pointer to shape def |
    constexpr uint8_t RENDER_DESTROY_NODE_OP = 0x02; // | — |
    constexpr uint8_t RENDER_UPDATE_POSITION_OP = 0x03; // | packed vec2 |
    constexpr uint8_t RENDER_UPDATE_SCALE_OP = 0x04;// | packed vec2 |
    constexpr uint8_t RENDER_SET_COLOR_OP = 0x05;// | 32 - bit RGBA |
    constexpr uint8_t RENDER_DONT_DRAW_OP = 0x06;// | — |
    constexpr uint8_t RENDER_SET_TEXTURE_OP = 0x07;// | texture handle |
    constexpr uint8_t RENDER_LERP_POSITION_OP = 0x08;// | pointer to lerp config |
    constexpr uint8_t RENDER_SET_TRANSFORM_OP = 0x09;// | pointer to mat4 |
    constexpr uint8_t RENDER_PUSH_STATE_OP = 0x0A;// | — |
    constexpr uint8_t RENDER_POP_STATE_OP = 0x0B;// | — |

    constexpr uint8_t RENDER_CLOSE_WINDOW = 0x0C; // | Used for closing a window being rendered to by rendering manager

    constexpr uint8_t RENDER_SHUTDOWN_THREAD = 0x0D; //used for shutting down render thread appropriately uwu
        

    //////////////////////////////////////////////
    // Rendering Manager Class
    //////////////////////////////////////////////
    class RenderingManager 
    {
    //////////////////////////////////////////////
    // Private Destructor
    //////////////////////////////////////////////
    private:
        ~RenderingManager();

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static RenderingManager& get_single() 
        {
            static RenderingManager rendering_manager;
            return rendering_manager;
        }
    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        explicit RenderingManager() = default; //explicitly nothing UwU >O<
        RenderingManager(const RenderingManager&) = delete;
        RenderingManager& operator=(const RenderingManager&) = delete;

        RenderingManager(RenderingManager&&) = delete;
        RenderingManager& operator=(RenderingManager&&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        unique_ptr<VulkanRenderer> pm_VulkanRenderer = nullptr;

        #ifndef __APPLE__ //OpenGL not supported on mac anymore fuck you tim apple
            unique_ptr<OpenGLRenderer> pm_OpenGLRenderer = nullptr;
        #endif

        uint64_t pm_FrameRateLimit = 60;
        uint64_t pm_CurrentFrame = 0;

        bool pm_IsVSyncEnabled = false;

        // DrawableObject.ObjectID : DrawableObject dict
        map<string, DrawableObject2D> pm_ListOfAllDrawables2D;

        //////////////////// Command/Resource Queue ////////////////////

        shared_ptr<moodycamel::ReaderWriterQueue<RenderCommand, TESTING_CAMEL_QUEUE_SIZE>> pm_RenderCommandQueue = nullptr;
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, TESTING_CAMEL_QUEUE_SIZE>> pm_LoadedResourceQueue = nullptr;

        //////////////////// Window Stuff ////////////////////

        SDL_Window* pm_MainWindow = nullptr;
        unordered_map<SDL_WindowID, SDL_Window*> pm_CurrentlyActiveWindows;
        vector<SDL_WindowID> pm_CloseWindowRequests;

        //////////////////// Logger ////////////////////

        shared_ptr<Logger> rendering_logger = nullptr;

    public: 
        atomic<bool> pm_IsRunning = true; //this doesn't need to be atomic but whatevs, or even needed tbh but probs helpful for the while loop maybes
        atomic<bool> pm_IsInitialized = false;

        condition_variable m_RenderCV;

        atomic<bool>  pm_ShouldRender{ false };

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        void
            RenderLoop
            (
                const RendererType fp_DesiredRenderer,
                const string& fp_LogOutputDirectory
            );

        bool 
            Initialize
        (
            const RendererType fp_DesiredRenderer,
            const string& fp_LogOutputDirectory
        );

        bool
            InitializeLoadingQueue();

        bool
            InitializeDrawCommandQueue();

        [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<RenderCommand, TESTING_CAMEL_QUEUE_SIZE>>
            GetDrawCommandQueue();

        [[nodiscard]] bool
            ProcessCommands();

        bool
            PresentFrame();

        void 
            ProcessLoadedResourcePackages();

        [[nodiscard]] bool
            CreateSDLWindow
            (
                SDL_Window** fp_SDLWindow,
                const RendererType fp_RenderingBackend,
                const string& fp_WindowTitle,
                const unsigned int fp_WindowWidth,
                const unsigned int fp_WindowHeight
            );
        
        #ifndef __APPLE__ //OpenGL stuff again fuck u tim apple dumb ahh mfer
            bool
                CreateOpenGLRenderer
                (
                    SDL_Window* fp_Window
                );

            void
                DestroyOpenGLRenderer();

            [[nodiscard]] OpenGLRenderer*
                GetOpenGLRenderer();
        #endif

        void 
            ResizeWindow();

        void 
            Shutdown();

        void 
            GetCurrentViewPort();

        [[nodiscard]] bool
            IsActive()
            const noexcept
        {
            return pm_IsRunning.load(std::memory_order_acquire);
        }

        void
            PollUserInputEvents()
        {
            InputManager::get_single().PollEvents();

            InputManager::get_single().GetWindowCloseRequests(pm_CloseWindowRequests);

            if (pm_CloseWindowRequests.size() > 0)
            {
                for (const auto& lv_Window : pm_CloseWindowRequests)
                {
                    if (SDL_GetWindowID(pm_MainWindow) == lv_Window)
                    {
                        pm_IsRunning.store(false);
                        //pm_VulkanRenderer->CleanUp();
                    }

                    SDL_DestroyWindow(SDL_GetWindowFromID(lv_Window)); //WARNING DO NOT CLOSE WINDOW HERE SEND A REQUEST TO THE RENDERING MANAGER FOR THAT
                }
            }

            glm::vec2 f_MousePos = InputManager::get_single().GetCurrentMousePosition();

            Print(format("mouse x : {}, y: {}", f_MousePos.x, f_MousePos.y), Colours::Green);
        }

        unsigned int GetFrameRateLimit() const;

        void SetFrameRateLimit(unsigned int fp_Limit);
        void SetVSync(const bool fp_IsEnabled);

        bool IsVSyncEnabled() const;

        void 
            ForceQuit()
        {
            pm_IsRunning = false;
        }

        SDL_Window*
            GetMainWindow()
        {
            return pm_MainWindow;
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        /*
            These LERP functions are used for interpolating sprite positions between physics update frames if the rendering fps is > 60 since physics
            will always update at a constant update interval of 60 times a second, equally spaced apart. This way you'll get "smoother" graphics if u wanna
            crank up the fps uwu



            The idea is that the most recent position will be used, and if the last used position idfk idk if thisll work since i cant predict the next frame, and if i use the current frame data and last frames, then the visuals will be outta sync
            with the current real position which is no good for gameplay, and tryna do predictions like that could be a bad route if the render time oversteps its processing tiime.
        */
        inline const float 
            Lerp(const float fp_Start, const float fp_End, const float fp_Rate)
            const noexcept
        {

        }

        inline const glm::vec2
            Lerp(const glm::vec2& fp_Start, const glm::vec2& fp_End, const glm::vec2& fp_Rate)
            const noexcept
        {

        }

        PeachNode* CreateNode(); //used for instantiating a rendering related node

        //wip? future me: WORKING BITCH
        bool
            InitializeOpenGL();

        bool
            InitializeVulkan();
    };
}
