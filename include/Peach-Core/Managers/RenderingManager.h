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

#include "../Rendering/PeachRenderer.h"
#include "../2D/PeachTexture2D.h"

#include "ResourceManager.h"
#include "../Rendering/ShaderProgram.h"
#include "../General/CommandQueue.h"
#include "../General/LoadingQueue.h"

#include "../General/PeachNode.h"


namespace PeachCore {

    //////////////////////////////////////////////
    // Drawable Object Struct
    //////////////////////////////////////////////
    //holds all relevant information that the renderer needs to know
    struct DrawableObject
    {
        string ObjectID;

        glm::vec2 Position;
        uint32_t LayerNumber = 0; //can't imagine there'll be more than 4 billion drawing layers, at that point integer overflow is the least of ur worries lmfao

        bool IsVisible = true;
        bool IsQueuedForRemoval = false;
            
        TextureData DrawableResourceData; //actual data for graphic //used for parsing raw byte information, mainly for audio at the moment
            //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
        Drawable GraphicsType; 
        ShaderProgram Shaders; //Contains multiple shaders relevant to drawing the object
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
        static RenderingManager& Renderer() 
        {
            static RenderingManager instance;
            return instance;
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
        VulkanRenderer pm_VulkanRenderer;

        unsigned int pm_FrameRateLimit = 60;
        unsigned long int pm_CurrentFrame = 0;

        bool pm_IsVSyncEnabled = false;
        bool pm_IsShutDown = false;

        bool pm_IsRenderingInitialized = false;
        bool pm_AreQueuesInitialized = false;
        bool pm_IsInitialized = false;

        // Object ID : CurrentPosition
        map<string, glm::vec2> pm_CurrentPositionOfAllDrawables; //not sure if theres a better way to not use two dicts since lerping will require persistent storage across frames until the next physics update
        //but i could't give less of a fuck right now
        // Object ID : DeltaPosition
        map<string, glm::vec2> pm_DeltaPositionForAllDrawablesThisFrame;
        // DrawableObject.ObjectID : DrawableObject dict
        map<string, DrawableObject> pm_ListOfAllDrawables;

        shared_ptr<CommandQueue> pm_CommandQueue = nullptr;
        shared_ptr<LoadingQueue> pm_LoadedResourceQueue = nullptr;

        unique_ptr<PeachRenderer> pm_Renderer = nullptr;

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

        shared_ptr<CommandQueue>
            InitializeQueues();

        void 
            ProcessCommands();
        void 
            ProcessLoadedResourcePackages();

        bool
            CreateSDLWindow
            (
                SDL_Window** fp_SDLWindow,
                const RendererType fp_RenderingBackend,
                const string& fp_WindowTitle,
                const unsigned int fp_WindowWidth,
                const unsigned int fp_WindowHeight
            )
            const;

        bool
            CreatePeachRenderer
            (
                SDL_Window* fp_Window
            );

        void
            DestroyPeachRenderer();

        void 
            ResizeWindow();

        void 
            RenderFrame(bool fp_IsStressTest = false);

        void 
            Shutdown();

        void 
            GetCurrentViewPort();

        PeachRenderer*
            GetPeachRenderer();

        unsigned int GetFrameRateLimit() const;

        void SetFrameRateLimit(unsigned int fp_Limit);
        void SetVSync(const bool fp_IsEnabled);

        bool IsVSyncEnabled() const;

        void ForceQuit()
        {
            pm_IsShutDown = true;
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