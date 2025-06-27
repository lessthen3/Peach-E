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

///PUI
#include "../Scene-Items/UI/Button.h"


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
        TextureData DrawableResourceData; //actual data for graphic //used for parsing raw byte information, mainly for audio at the moment
            //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
        Drawable GraphicsType; 
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

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        unique_ptr<VulkanRenderer> pm_VulkanRenderer = nullptr;

        #ifndef __APPLE__ //OpenGL not supported on mac anymore fuck you tim apple
            unique_ptr<OpenGLRenderer> pm_OpenGLRenderer = nullptr;
        #endif

        unsigned int pm_FrameRateLimit = 60;
        unsigned long int pm_CurrentFrame = 0;

        bool pm_IsVSyncEnabled = false;
        bool pm_IsShutDown = false;

        bool pm_IsInitialized = false;

        // DrawableObject.ObjectID : DrawableObject dict
        map<string, DrawableObject2D> pm_ListOfAllDrawables2D;

        shared_ptr<CommandQueue> pm_DrawCommandQueue = nullptr;
        shared_ptr<LoadingQueue> pm_LoadedResourceQueue = nullptr;

        SDL_Window* pm_MainWindow = nullptr;

    public: //DOING THIS FOR NOW TO TEST RUNNING GAME INSTANCE FROM EDITOR NEEDS TO BE PRIVATE IN MY OPINION
        shared_ptr<LogManager> rendering_logger = nullptr;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool 
            Initialize
        (
            const RendererType fp_DesiredRenderer,
            const string& fp_LogOutputDirectory,
            shared_ptr<Console> fp_Console
        );

        bool
            InitializeLoadingQueue();

        bool
            InitializeDrawCommandQueue();

        [[nodiscard]] shared_ptr<CommandQueue>
            GetDrawCommandQueue();

        void 
            ProcessDrawCommands();

        void 
            ProcessLoadedResourcePackages();

        inline bool
            CreateSDLWindow
            (
                SDL_Window** fp_SDLWindow,
                const RendererType fp_RenderingBackend,
                const string& fp_WindowTitle,
                const unsigned int fp_WindowWidth,
                const unsigned int fp_WindowHeight
            )
            const;
        
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
            RenderFrame(bool fp_IsStressTest = false);

        void 
            Shutdown();

        void 
            GetCurrentViewPort();

        [[nodiscard]] VulkanRenderer*
            GetVulkanRenderer();

        unsigned int GetFrameRateLimit() const;

        void SetFrameRateLimit(unsigned int fp_Limit);
        void SetVSync(const bool fp_IsEnabled);

        bool IsVSyncEnabled() const;

        void ForceQuit()
        {
            pm_IsShutDown = true;
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
            */
        inline const float 
            Lerp(const float fp_Start, const float fp_End, const float fp_Rate)
            const
        {

        }

        inline const glm::vec2
            Lerp(const glm::vec2& fp_Start, const glm::vec2& fp_End, const glm::vec2& fp_Rate)
            const
        {

        }

        PeachNode* CreateNode(); //used for instantiating a rendering related node

        //wip?
        bool
            InitializeOpenGL();

        bool
            InitializeVulkan();
    };
}

namespace PeachCore{

    // -- Peach UI System --
class PeachUIManager
{
    // public:
    //     unique_ptr<PeachUserInterfaceNode> root;
    //     shared_ptr<LogManager> pui_logger = nullptr;

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