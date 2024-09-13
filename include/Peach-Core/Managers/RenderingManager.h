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

#include <SDL2/SDL.h>

#include "LogManager.h"
#include "ResourceLoadingManager.h"
#include "../General/ShaderProgram.h"
#include "../General/CommandQueue.h"
#include "../General/LoadingQueue.h"

#include "../General/PeachNode.h"
#include "../General/OpenGLRenderer.h"

#include "../2D/PeachTexture2D.h"

#include <variant>
#include <string>
#include <map>

#include "../2D/PeachCamera2D.h"

using namespace std;

namespace PeachCore {

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

            unique_ptr<PeachTexture2D>, //SFML loads textures in as a single wrapped unit
            unique_ptr<string> //used for parsing JSON metadata if required

            > DrawableResourceData; //actual data for graphic

        Drawable GraphicsType; 
        ShaderProgram Shaders;
    };

    class RenderingManager 
    {
    public:
        static RenderingManager& Renderer() 
        {
            static RenderingManager instance;
            return instance;
        }
        ~RenderingManager();

    private:
        explicit RenderingManager() = default;
        RenderingManager(const RenderingManager&) = delete;
        RenderingManager& operator=(const RenderingManager&) = delete;

    private:
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


    public:

        bool CreateWindowAndCamera2D(const string& fp_Title = "Peach Engine", const int fp_Width = 800, const int fp_Height = 600);
        void ProcessCommands();
        void ProcessLoadedResourcePackages();
        
        void SetupRenderTexture(unsigned int width, unsigned int height);

        const PeachTexture2D& 
            GetRenderTexture()
            const;

        bool
            CreateSDLWindow(const char* fp_WindowTitle, const int fp_WindowWidth, const int fp_WindowHeight);

        shared_ptr<CommandQueue> 
            Initialize();


        void 
            ResizeWindow();

        string 
            GetRendererType() 
            const;

        void RenderFrame(bool fp_IsStressTest = false);
        void BeginFrame();
        void EndFrame();
        void Clear();

        void Shutdown();

        void GetCurrentViewPort();
        //void Draw(const sf::Drawable& fp_Drawable);
        //void DrawTextToScreen(const std::string& fp_Text, const sf::Font& fp_Font, unsigned int fp_Size, const sf::Vector2f& fp_Position, const sf::Color& fp_Color = sf::Color::White);

        unsigned int GetFrameRateLimit() const;

        void SetFrameRateLimit(unsigned int fp_Limit);
        void SetVSync(const bool fp_IsEnabled);

        bool IsVSyncEnabled() const;

        void ForceQuit()
        {
            pm_IsShutDown = true;
        }

    private:
        unsigned int pm_FrameRateLimit = 60;
        bool pm_IsVSyncEnabled = false;
        bool pm_IsShutDown = false;
    private:
        unsigned long int pm_CurrentFrame = 0;

        // Object ID : CurrentPosition
        map<string, glm::vec2> pm_CurrentPositionOfAllDrawables; //not sure if theres a better way to not use two dicts since lerping will require persistent storage across frames until the next physics update
                                                                                                    //but i could't give less of a fuck right now
        // Object ID : DeltaPosition
        map<string, glm::vec2> pm_DeltaPositionForAllDrawablesThisFrame;
        // DrawableObject.ObjectID : DrawableObject dict
        map<string, DrawableObject> pm_ListOfAllDrawables;

        shared_ptr<CommandQueue> pm_CommandQueue = nullptr;
        shared_ptr<LoadingQueue> pm_LoadedResourceQueue = nullptr;

        unique_ptr<PeachTexture2D> m_TestTexture = nullptr;

        OpenGLRenderer* pm_OpenGLRenderer = nullptr;
        PeachCamera2D* pm_CurrentCamera2D = nullptr;
        SDL_Window* pm_CurrentWindow = nullptr;
    };

}