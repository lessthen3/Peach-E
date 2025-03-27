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

#include <GL/glew.h>

#include "PeachEditorResourceLoadingManager.h"

#include "../../include/Peach-Core/Managers/RenderingManager.h"

#include <SDL3/SDL_opengl.h>

#include <atomic> //should be used for communicating whether the scene execution thread is currently running or not
#include <unordered_set>
#include <filesystem>

using namespace std; 
namespace PC = PeachCore;
namespace fs = filesystem;

namespace PeachEditor {

    constexpr float MAIN_MENU_BAR_SCALE = 0.03f;
    constexpr unsigned int NUMBER_OF_HORIZONTAL_MAIN_MENU_BAR_ELEMENTS = 7;

    //////////////////////////////////////////////
    // Viewport Struct
    //////////////////////////////////////////////

    struct Viewport
    {

        Viewport() = default;

        void
            SetupViewport
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height,
                PC::PeachRenderer* fp_Renderer,
                shared_ptr<PC::LogManager> fp_EditorRenderingLogger
            );

        void
            ResizeViewport
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height
            );

        void
            RenderViewport
            (
                const glm::vec2& fp_Position,
                const unsigned int fp_Width,
                const unsigned int fp_Height
            );

    private:
        GLuint pm_RenderTexture = -1; //i initialize these to -1 so that i know they've been unitialized ig idk
        GLuint pm_FrameBuffer = -1;
        GLuint pm_DepthRenderBuffer = -1;

        GLuint pm_VAO = -1;

        PC::ShaderProgram* pm_ViewportShader = nullptr;

        unsigned int pm_CurrentViewportHeight = 0;
        unsigned int pm_CurrentViewportWidth = 0;

        vector<SDL_Event> pm_CurrentPolledEvents;

        PC::PeachRenderer* pm_Render = nullptr;

        shared_ptr<PC::LogManager> editor_rendering_logger = nullptr;

        bool
            CreateRenderTexture
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height
            );
    };

    //////////////////////////////////////////////
    // PeachEditorRenderingManager Class
    //////////////////////////////////////////////

    class PeachEditorRenderingManager 
    {
    //////////////////////////////////////////////
    // Destructor
    //////////////////////////////////////////////
    public:
        ~PeachEditorRenderingManager();

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static PeachEditorRenderingManager& PeachEditorRenderer()
        {
            static PeachEditorRenderingManager peach_editor_renderer;
            return peach_editor_renderer;
        }
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

        unsigned long int pm_CurrentFrame = 0;

        // Object ID : CurrentPosition
        map<string, glm::vec2> pm_CurrentPositionOfAllDrawables; //not sure if theres a better way to not use two dicts since lerping will require persistent storage across frames until the next physics update
        
        //but i could't give less of a fuck right now
        // Object ID : DeltaPosition
        map<string, glm::vec2> pm_DeltaPositionForAllDrawablesThisFrame;

        // DrawableObject.ObjectID : DrawableObject dict
        map<string, PC::DrawableObject> pm_ListOfAllDrawables;

        shared_ptr<PC::CommandQueue> pm_CommandQueue = nullptr;
        shared_ptr<PC::LoadingQueue> pm_LoadedResourceQueue = nullptr;

        SDL_Window* pm_MainWindow = nullptr;

        struct nk_context* pm_NuklearCtx = nullptr;

        shared_ptr<PC::LogManager> rendering_logger = nullptr;

        Viewport pm_Viewport;

        SDL_Window* pm_GameInstanceWindow = nullptr;

        const glm::vec4 pm_ClearColour = { 0.10f, 0.18f, 0.24f, 1.0f };

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        atomic<bool> m_IsSceneCurrentlyRunning = false; //tracks whether the current working scene in the current peach project, is running in the editor

        struct nk_colorf pm_BackgroundColour = { 0.10f, 0.18f, 0.24f, 1.0f };

        unordered_set<string> pm_CurrentlyOpenDirectories;

        struct FileSelectionState
        {
            unordered_set<string> SelectedFiles;
            string LastSelectedItem;
        };

        FileSelectionState pm_SelectionState;

        bool pm_IsCtrlPressed = false;
        bool pm_IsShiftPressed = false;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        void
            ProcessCommands();
        void
            ProcessLoadedResourcePackages();

        shared_ptr<PC::CommandQueue>
            InitializeQueues();

        //WIP
        bool
            Initialize
            (
                const string& fp_LogOutputDirectory,
                shared_ptr<PC::Console> fp_EditorConsole
            );

        void
            RenderFrame
            (
                bool* fp_IsProgramRuntimeOver
            );

        void
            Shutdown();

        void
            CreateSceneTreeViewPanel();

        uint32_t
            GetFrameRateLimit()
            const;

        SDL_Window*&
            GetMainWindow();

        Viewport*
            GetViewport();

        //WIP NOT SURE IF I SHOULD HAVE THIS BUT WHATEVER I NEED IT FOR TESTING W THE CURRENT THREAD SETUP UWU
        //shared_ptr<PC::LogManager>
        //    GetRenderingLogger()
        //{

        //}

        void
            SetFrameRateLimit(uint32_t fp_Limit);
        void
            SetVSync(const bool fp_IsEnabled);

        bool
            IsVSyncEnabled()
            const;

        void
            RunCurrentScene();

        SDL_Window*&
            GetGameInstanceWindow()
        {
            return pm_GameInstanceWindow;
        }

        void
            SetGameInstanceWindow(SDL_Window* fp_GameWindow)
        {
            pm_GameInstanceWindow = fp_GameWindow;
        }

        void
            RenderDirectory
            (
                struct nk_context* ctx, 
                const fs::path& path
            );

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        void
            RenderFileBrowser
            (
                const filesystem::path& directory, 
                float x, 
                float y, 
                float width, 
                float height,
                struct nk_context* ctx
            );

        void
            HandleFileSelection
            (
                const string& fp_FileItemSelected
            );

        void
            HandleRangeSelection
            (
                const string& fp_FirstItemSelected,
                const string& fp_SecondItemSelected
            );

        void
            RenderConsole
            (
                struct nk_context* ctx
            );
    };

}