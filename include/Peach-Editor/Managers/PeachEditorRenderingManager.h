/*******************************************************************
 *                                        Peach Editor v0.0.7
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                 Peach Editor is an open source editor for Peach-E
********************************************************************/
#pragma once

//

#include "PeachEditorManager.h"
#include "PeachEditorResourceLoadingManager.h"

#include <atomic> //should be used for communicating whether the scene execution thread is currently running or not

using namespace std; 

namespace PeachEditor {

    //holds all relevant information that the renderer needs to know
    struct DrawableObject
    {
        string ObjectID;

        glm::vec2 Position;
        uint32_t LayerNumber = 0;

        bool IsVisible = true;
        bool IsQueuedForRemoval = false;

        variant //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
            <
            PeachCore::TextureData, //used for parsing raw byte information, mainly for audio at the moment
            unique_ptr<nlohmann::json> //used for parsing JSON metadata if required

            > DrawableResourceData; //actual data for graphic

        PeachCore::Drawable GraphicsType;
        //ShaderProgram Shaders;
    };

    class PeachEditorRenderingManager 
    {
    //////////////////////////////////////////////
    // Destructor
    //////////////////////////////////////////////
    public:
        ~PeachEditorRenderingManager();

    //////////////////////////////////////////////
    // Constructor
    //////////////////////////////////////////////
    private:
        PeachEditorRenderingManager() = default;
        PeachEditorRenderingManager(const PeachEditorRenderingManager&) = delete;
        PeachEditorRenderingManager& operator=(const PeachEditorRenderingManager&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        bool pm_HasViewportRenderTextureBeenInitialized = false;
        bool pm_AreQueuesInitialized = false; //set to false intially, and will be set to true once intialized to prevent more than one initialization
        bool pm_IsRenderingInitialized = false;

        bool pm_IsVSyncEnabled = false;

        uint32_t pm_FrameRateLimit = 60;

        string pm_RendererType = "None";

        unsigned long int pm_CurrentFrame = 0;

        // Object ID : CurrentPosition
        map<string, glm::vec2> pm_CurrentPositionOfAllDrawables; //not sure if theres a better way to not use two dicts since lerping will require persistent storage across frames until the next physics update
        
        //but i could't give less of a fuck right now
        // Object ID : DeltaPosition
        map<string, glm::vec2> pm_DeltaPositionForAllDrawablesThisFrame;

        // DrawableObject.ObjectID : DrawableObject dict
        map<string, DrawableObject> pm_ListOfAllDrawables;

        shared_ptr<PeachCore::CommandQueue> pm_CommandQueue = nullptr;
        shared_ptr<PeachCore::LoadingQueue> pm_LoadedResourceQueue = nullptr;

        unique_ptr<PeachCore::PeachRenderer> pm_PeachRenderer = nullptr;
        SDL_Window* pm_MainWindow = nullptr;

        //unique_ptr<sf::RenderTexture> pm_ViewportRenderTexture = nullptr;

        InternalLogManager* rendering_logger = nullptr;
        PeachEditorManager* peach_editor = nullptr;

        ImGuiWindowFlags pm_FileSystemWindowFlags;
        ImGuiWindowFlags pm_ViewportWindowFlags;
        ImGuiWindowFlags pm_SceneTreeViewWindowFlags;
        ImGuiWindowFlags pm_PeachConsoleWindowFlags;


    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        atomic<bool> m_IsSceneCurrentlyRunning = false; //tracks whether the current working scene in the current peach project, is running in the editor

        static PeachEditorRenderingManager& PeachEngineRenderer()
        {
            static PeachEditorRenderingManager peach_engine_renderer;
            return peach_engine_renderer;
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        void
            RenderFileBrowser
            (
                const std::filesystem::path& directory
            );

        void 
            UpdateAndRenderFileBrowser
            (
                const fs::path& path
            );

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:

        void ProcessCommands();
        void ProcessLoadedResourcePackages();

        bool 
            SetupRenderTexture
            (
                const uint32_t width, 
                const uint32_t height, 
                const bool IsNearestNeighbour = false
            );

        bool 
            ResizeRenderTexture
            (
                const uint32_t fp_Width, 
                const uint32_t fp_Height, 
                const bool IsNearestNeighbour = false
            );

        bool
            CreateSDLWindow
            (
                const char* fp_Title = "Peach Engine",
                const uint32_t fp_Width = 800,
                const uint32_t fp_Height = 600
            );

        shared_ptr<PeachCore::CommandQueue> 
            InitializeQueues();

        //WIP
        bool
            InitializeOpenGL();

        string 
            GetRendererType() 
            const;

        void 
            RenderFrame
            (
                bool* fp_IsProgramRuntimeOver
            );
        void 
            EndFrame();
        void 
            Clear();

        void 
            Shutdown();

        void 
            CreateSceneTreeViewPanel();

        void 
            GetCurrentViewPort();

        uint32_t 
            GetFrameRateLimit() 
            const;

        void 
            SetFrameRateLimit(uint32_t fp_Limit);
        void 
            SetVSync(const bool fp_IsEnabled);

        bool 
            IsVSyncEnabled() 
            const;

        void 
            RunCurrentScene();
    };

}