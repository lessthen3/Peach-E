////////////////////////////////////////////////////////
/***                             [Peach Core]                                ***/
////////////////////////////////////////////////////////
/***                            [Version 0.0.01]                             ***/
////////////////////////////////////////////////////////
/***  Copyright(c) 2024-present Ranyodh Singh Mandur  ***/
/***                               MIT License                                ***/
/***         Documentation: http://www.yourwebsite.com    ***/
/*** GitHub: https://github.com/iLoveJohnFish/Peach-E ***/
/////////////////////////////////////////////////////////

#pragma once
#include <string>
#include "LogManager.h"
#include "../Peach-Core2D/Rendering2D/Renderer2D.h"

//#define SDL_MAIN_HANDLED
//#include <SDL2/SDL.h>


namespace PeachCore {

    class RenderingManager {
    public:
        static RenderingManager& Renderer() {
            static RenderingManager instance;
            return instance;
        }
        ~RenderingManager();

    private:
        RenderingManager() = default;// Private constructor for singleton

        SDL_Window* pm_CurrentWindow = nullptr;
        Renderer2D* pm_Renderer2D = nullptr;
        bgfx::RendererType::Enum pm_PreferredType = bgfx::RendererType::Count; // Default to auto-selection

        bool hasBeenInitialized = false; //set to false intially, and will be set to true once intialized to prevent more than one initialization
        std::string pm_RendererType = "None";

        unsigned int pm_FrameRateLimit = 60;
        bool pm_IsVSyncEnabled = false;


    public:
        void SetRendererType(bgfx::RendererType::Enum type);
        void CreateSDLWindow(const char* title, int width, int height);

        void CreateRenderer2D();

        void Initialize(const std::string fp_RendererType, const std::string& fp_Title = "Peach Engine", const int fp_Width = 800, const int fp_Height = 600);
        void ResizeWindow();
        std::string GetRendererType() const;

        void renderFrame();
        void BeginFrame();
        void EndFrame();
        void Clear();

        void GetCurrentViewPort();

        //void Draw(const sf::Drawable& fp_Drawable);
        //void DrawTextToScreen(const std::string& fp_Text, const sf::Font& fp_Font, unsigned int fp_Size, const sf::Vector2f& fp_Position, const sf::Color& fp_Color = sf::Color::White);

        unsigned int GetFrameRateLimit() const;

        void SetFrameRateLimit(unsigned int fp_Limit);
        void SetVSync(const bool fp_IsEnabled);

        bool IsVSyncEnabled() const;

        //(const std::string fp_RendererType, const std::string& fp_Title = "Peach Engine", const int fp_Width = 800 , const int fp_Height = 600);
    private:

        RenderingManager(const RenderingManager&) = delete;
        RenderingManager& operator=(const RenderingManager&) = delete;

    };

}