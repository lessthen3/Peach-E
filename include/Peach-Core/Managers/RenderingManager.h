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

#include "../General/PeachRenderer.h"
#include "../2D/PeachTexture2D.h"

#include "ResourceLoadingManager.h"
#include "../General/ShaderProgram.h"
#include "../General/CommandQueue.h"
#include "../General/LoadingQueue.h"

#include "../General/PeachNode.h"

#include <variant>

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
        unsigned int pm_FrameRateLimit = 60;
        unsigned long int pm_CurrentFrame = 0;

        bool pm_IsVSyncEnabled = false;
        bool pm_IsShutDown = false;
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

        unique_ptr<PeachTexture2D> m_TestTexture = nullptr;

        PeachRenderer* pm_PeachRenderer = nullptr;
        SDL_Window* pm_CurrentWindow = nullptr;


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

        void ProcessCommands();
        void ProcessLoadedResourcePackages();
        
        void SetupRenderTexture(unsigned int width, unsigned int height);

        const PeachTexture2D& 
            GetRenderTexture()
            const;

        bool
            CreateSDLWindow
            (
                const char* fp_WindowTitle, 
                const unsigned int fp_WindowWidth, 
                const unsigned int fp_WindowHeight
            );

        bool
            CreatePeachRenderer
            (

            );

        shared_ptr<CommandQueue> 
            InitializeQueues();


        void 
            ResizeWindow();

        string 
            GetRendererType() 
            const;

        void 
            RenderFrame(bool fp_IsStressTest = false);

        void 
            Shutdown();

        void 
            GetCurrentViewPort();
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
        //WIP
        bool
            InitializeOpenGL();
    };

}


//
//if (not InitOpenGL(f_WindowWidth, f_WindowHeight))
//{
//    LogManager::RenderingLogger().Fatal("Failed to initialize OpenGL.", "PeachRenderer");
//    throw runtime_error("Failed to initialize OpenGL.");
//}