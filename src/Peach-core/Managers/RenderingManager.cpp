#include "RenderingManager.h"


namespace PeachCore {

    void RenderingManager::Initialize(const std::string fp_RendererType, const std::string& fp_Title, const int fp_Width, const int fp_Height)
    {
        if (hasBeenInitialized)
        {
            LogManager::Logger().Warn("RenderingManager has already been initialized, LogManager is only allowed to initialize once per run", "RenderingManager");
            return;
        }
        else
        {
            hasBeenInitialized = true;
        }

        sf::RenderWindow pm_CurrentRenderWindow = sf::RenderWindow(sf::VideoMode(fp_Width, fp_Height), fp_Title);

        pm_FrameRateLimit = (60);
        pm_IsVSyncEnabled = (false);

        pm_CurrentRenderWindow.setFramerateLimit(pm_FrameRateLimit);
    }

    void RenderingManager::DrawTextToScreen(const std::string& fp_Text, const sf::Font& fp_Font, unsigned int fp_Size, const sf::Vector2f& fp_Position, const sf::Color& fp_Color) {
        sf::Text sfText;
        sfText.setFont(fp_Font);
        sfText.setString(fp_Text);
        sfText.setCharacterSize(fp_Size);
        sfText.setFillColor(fp_Color);
        sfText.setPosition(fp_Position);
        pm_CurrentRenderWindow.draw(sfText);
    }

    void RenderingManager::Draw(const sf::Drawable& fp_Drawable)
    {
        pm_CurrentRenderWindow.draw(fp_Drawable);
    }

    void RenderingManager::Clear(const sf::Color& fp_Color)
    {
        pm_CurrentRenderWindow.clear(fp_Color);
    }

    void RenderingManager::ResizeWindow()
    {

    }

    void RenderingManager::BeginFrame()
    {

    }


    void RenderingManager::EndFrame()
    {
        pm_CurrentRenderWindow.display();
    }

    std::string RenderingManager::GetRendererType() const
    {
        return pm_RendererType;
    }

    sf::RenderWindow& RenderingManager::GetRenderWindow()
    {
        return pm_CurrentRenderWindow;
    }

    unsigned int RenderingManager::GetFrameRateLimit() const
    {
        return pm_FrameRateLimit;
    }

    bool RenderingManager::IsVSyncEnabled() const
    {
        return pm_IsVSyncEnabled;
    }

    void RenderingManager::SetVSync(const bool fp_IsEnabled)
    {
        if (fp_IsEnabled)
        {
            pm_IsVSyncEnabled = fp_IsEnabled;
        }
    }

    void RenderingManager::SetFrameRateLimit(unsigned int fp_Limit)
    {
        pm_FrameRateLimit = fp_Limit;
    }

}