////////////////////////////////////////////////////////
/***                             [Peach Engine]                              ***/
////////////////////////////////////////////////////////
/***                            [Version 0.0.01]                             ***/
////////////////////////////////////////////////////////
/***  Copyright(c) 2024-present Ranyodh Singh Mandur  ***/
/***                               MIT License                                ***/
/***         Documentation: TBD                                          ***/
/*** GitHub: https://github.com/iLoveJohnFish/Peach-E ***/
/////////////////////////////////////////////////////////
#pragma once

#include <SFML/Graphics.hpp>

#include "../../../Peach-core/GOATS/sfml-imgui/imgui-SFML.h"
#include <imgui.h>

#include "PeachEngineManager.h"

#include "InternalLogManager.h"
#include "PeachEngineResourceLoadingManager.h"

#include "../../../Peach-core/include/Peach-Core2D/Rendering2D/PeachCamera2D.h"

#include "../Peach-E Objects/PeachEConsole.h"

#include <variant>
#include <string>
#include <map>
#include <atomic> //should be used for communicating whether the scene execution thread is currently running or not

using namespace std;

namespace PeachEditor {

    //holds all relevant information that the renderer needs to know
    struct DrawableObject
    {
        string ObjectID;

        glm::vec2 Position;
        uint32_t LayerNumber;

        bool IsVisible;
        bool IsQueuedForRemoval = false;

        variant //using unique ptrs to avoid any hanging ptrs and to make garbage collection easier/simpler
            <
            unique_ptr<unsigned char>, //used for parsing raw byte information, mainly for audio at the moment
            unique_ptr<sf::Texture>, //SFML loads textures in as a single wrapped unit
            unique_ptr<string> //used for parsing JSON metadata if required

            > DrawableResourceData; //actual data for graphic

        PeachCore::Drawable GraphicsType;
        //ShaderProgram Shaders;
    };

    class PeachEngineRenderingManager 
    {
    public:
        static PeachEngineRenderingManager& PeachEngineRenderer()
        {
            static PeachEngineRenderingManager peach_engine_renderer;
            return peach_engine_renderer;
        }

        ~PeachEngineRenderingManager();

    private:
        PeachEngineRenderingManager();
        PeachEngineRenderingManager(const PeachEngineRenderingManager&) = delete;
        PeachEngineRenderingManager& operator=(const PeachEngineRenderingManager&) = delete;

    private:
        inline const float Lerp(const float fp_Start, const float fp_End, const float fp_Rate)
            const
        {

        }

        inline const glm::vec2 Lerp(const glm::vec2& fp_Start, const glm::vec2& fp_End, const glm::vec2& fp_Rate)
            const
        {

        }

    public:

        void ProcessCommands();
        void ProcessLoadedResourcePackages();

        bool SetupRenderTexture(const unsigned int width, const unsigned int height, bool IsNearestNeighbour = false);
        bool ResizeRenderTexture(const unsigned int fp_Width, const unsigned int fp_Height, bool IsNearestNeighbour = false);

        const sf::RenderTexture& GetRenderTexture()
            const;


        shared_ptr<PeachCore::CommandQueue> Initialize(const string& fp_Title = "Peach Engine", const int fp_Width = 800, const int fp_Height = 600);
        void ResizeWindow();
        string GetRendererType() const;

        void RenderFrame();
        void BeginFrame();
        void EndFrame();
        void Clear();

        void CreatePeachEConsole();
        void CreateSceneTreeViewPanel();

        void GetCurrentViewPort();

        unsigned int GetFrameRateLimit() const;

        void SetFrameRateLimit(unsigned int fp_Limit);
        void SetVSync(const bool fp_IsEnabled);

        bool IsVSyncEnabled() const;

        void RunCurrentScene();

        atomic<bool> m_IsSceneCurrentlyRunning = false; //tracks whether the current working scene in the current peach project, is running in the editor


    private:
        bool pm_HasViewportRenderTextureBeenInitialized = false;
        bool pm_HasBeenInitialized = false; //set to false intially, and will be set to true once intialized to prevent more than one initialization

        unsigned int pm_FrameRateLimit = 60;
        bool pm_IsVSyncEnabled = false;

        string pm_RendererType = "None";

    private:
        unsigned long int pm_CurrentFrame = 0;
        shared_ptr<PeachCore::CommandQueue> pm_CommandQueue;
        shared_ptr<PeachCore::LoadingQueue> pm_LoadedResourceQueue;

        // Object ID : CurrentPosition
        map<string, glm::vec2> pm_CurrentPositionOfAllDrawables; //not sure if theres a better way to not use two dicts since lerping will require persistent storage across frames until the next physics update
        //but i could't give less of a fuck right now
// Object ID : DeltaPosition
        map<string, glm::vec2> pm_DeltaPositionForAllDrawablesThisFrame;
        // DrawableObject.ObjectID : DrawableObject dict
        map<string, DrawableObject> pm_ListOfAllDrawables;

        unique_ptr<sf::Texture> m_TestTexture = nullptr;

        PeachCamera2D* pm_Camera2D = nullptr;
        sf::RenderWindow* pm_CurrentWindow = nullptr;

        unique_ptr<PeachEConsole> pm_EditorConsole = nullptr;
        unique_ptr<sf::RenderTexture> pm_ViewportRenderTexture = nullptr;
    };

}