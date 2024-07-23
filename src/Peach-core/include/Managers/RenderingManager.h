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

#include <SFML/Graphics.hpp>

#include "LogManager.h"
#include "ResourceLoadingManager.h"
#include "../Unsorted/ShaderProgram.h"
#include "../Unsorted/CommandQueue.h"
#include "../Unsorted/LoadingQueue.h"


#include <variant>
#include <string>
#include <map>

#include "../Peach-Core2D/Rendering2D/PeachCamera2D.h"

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
            unique_ptr<unsigned char>, //used for parsing raw byte information, mainly for audio at the moment
            unique_ptr<sf::Texture>, //SFML loads textures in as a single wrapped unit
            unique_ptr<string> //used for parsing JSON metadata if required

            > DrawableResourceData; //actual data for graphic

        Drawable GraphicsType; 
        ShaderProgram Shaders;
    };

    class RenderingManager {
    public:
        static RenderingManager& Renderer() 
        {
            static RenderingManager instance;
            return instance;
        }
        ~RenderingManager();

    private:
        RenderingManager();
        RenderingManager(const RenderingManager&) = delete;
        RenderingManager& operator=(const RenderingManager&) = delete;

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

        bool CreateWindowAndCamera2D(const string& fp_Title = "Peach Engine", const int fp_Width = 800, const int fp_Height = 600);
        void ProcessCommands();
        void ProcessLoadedResourcePackages();
        
        void SetupRenderTexture(unsigned int width, unsigned int height);

        const sf::Texture& GetRenderTexture()
            const;


        shared_ptr<CommandQueue> Initialize();
        void ResizeWindow();
        string GetRendererType() const;

        void RenderFrame();
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
        shared_ptr<CommandQueue> pm_CommandQueue;
        shared_ptr<LoadingQueue> pm_LoadedResourceQueue;

        // Object ID : CurrentPosition
        map<string, glm::vec2> pm_CurrentPositionOfAllDrawables; //not sure if theres a better way to not use two dicts since lerping will require persistent storage across frames until the next physics update
                                                                                                    //but i could't give less of a fuck right now
        // Object ID : DeltaPosition
        map<string, glm::vec2> pm_DeltaPositionForAllDrawablesThisFrame;
        // DrawableObject.ObjectID : DrawableObject dict
        map<string, DrawableObject> pm_ListOfAllDrawables;

        unique_ptr<sf::Texture> m_TestTexture;

        PeachCamera2D* pm_CurrentCamera2D;
        sf::RenderWindow* pm_CurrentWindow = nullptr;

    };

}