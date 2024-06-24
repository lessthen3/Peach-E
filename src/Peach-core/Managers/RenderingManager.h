#pragma once
#include <string>
#include "LogManager.h"
#include "SFML/Graphics.hpp"

class RenderingManager {
public:
    static RenderingManager& Renderer() {
        static RenderingManager instance;
        return instance;
    }

public:
    void Initialize(std::string p_RendererType, const std::string& title = "Peach Engine", int width = 800, int height = 600);
    void ResideWindow();
    std::string GetRendererType() const;

    void BeginFrame();
    void EndFrame();
    void Clear(const sf::Color& fp_Color = sf::Color::Black);

    void Draw(const sf::Drawable& fp_Drawable);
    void DrawText(const std::string& fp_Text, const sf::Font& sp_Font, unsigned int sp_Size, const sf::Vector2f& fp_Position, const sf::Color& fp_Color = sf::Color::White);

    sf::RenderWindow& GetWindow() const;

    void SetFramerateLimit(unsigned int fp_Limit);
    unsigned int GetFrameLimit() const;

    void SetVSync(bool fp_IsEnabled);
    bool IsVSyncEnabled() const;


private:
    RenderingManager() = default;
    ~RenderingManager() = default;

    RenderingManager(const RenderingManager&) = delete;
    RenderingManager& operator=(const RenderingManager&) = delete;


private:
    bool hasBeenInitialized = false; //set to false intially, and will be set to true once intialized to prevent more than one initialization
    std::string pm_RendererType = "None";

    sf::RenderWindow window;

    unsigned int framerateLimit = 60;
    bool vsyncEnabled = false;
};