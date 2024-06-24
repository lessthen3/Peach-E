#include <iostream>
#include "../Peach-core/Managers/Managers.h"
#include "SFML/Graphics.hpp"

static void LoadPluginsFromConfigs(const std::vector<std::string>& fp_ListOfPluginsToLoad)
{
    for (int index = 0; index < fp_ListOfPluginsToLoad.size(); index++)
    {
        PluginManager::ManagePlugins().LoadPlugin(fp_ListOfPluginsToLoad[index]);
    }
}

static void SetupRenderer()
{

}

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int main()
{
    std::vector<std::string> ListOfWindowsPluginsToLoad = { "D:/Game Development/Peach-E/src/Peach-E-Core/SimplePlugin.dll" };
    std::vector<std::string> ListOfUnixPluginsToLoad = { };

    LogManager::Logger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE");
    LogManager::Logger().Debug("LogManager successfully initialized", "LogManager");
    std::cout << "Hello World!\n";
    LogManager::Logger().Warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

    LogManager::Logger().Trace("Success! This Built Correctly", "Peach-E");

    ////////////////////////////////////////////////
    //// Plugin Loader Step
    ////////////////////////////////////////////////

    #if defined(_WIN32) || defined(_WIN64)
        LoadPluginsFromConfigs(ListOfWindowsPluginsToLoad); // Windows
    #else
        LoadPluginsFromConfigs(ListOfUnixPluginsToLoad); // Linux/Unix
    #endif

    PluginManager::ManagePlugins().InitializePlugins();
    PluginManager::ManagePlugins().UpdatePlugins(0.1f);
    PluginManager::ManagePlugins().ShutdownPlugins();

    ////////////////////////////////////////////////
    //// Setting Up Renderer For Engine
    ////////////////////////////////////////////////

   /*RenderingManager::Renderer().Initialize("OpenGL");

   sf::RenderWindow& m_CurrentRenderWindow = RenderingManager::Renderer().GetWindow();*/

    // Set global settings
   //RenderingManager::Renderer().SetFramerateLimit(120);
   //RenderingManager::Renderer().SetVSync(true);

   //std::string f_PathToFontFile = "D:/Game Development/Fonts I guess/Comic Sans MS";

   // sf::Font font;
   // if (!font.loadFromFile(f_PathToFontFile)) {
   //     LogManager::Logger().Warn("Load Error: Unable to load font file from specified path: " + f_PathToFontFile, "Peach-E");
   // }

   // while (m_CurrentRenderWindow.isOpen()) {
   //     sf::Event event;
   //     while (m_CurrentRenderWindow.pollEvent(event)) {
   //         if (event.type == sf::Event::Closed) {
   //             m_CurrentRenderWindow.close();
   //         }
   //     }

   //     RenderingManager::Renderer().BeginFrame();
   //     RenderingManager::Renderer().Clear();

   //     // Example drawing calls
   //     sf::CircleShape shape(50);
   //     shape.setFillColor(sf::Color::Green);
   //     shape.setPosition(100, 100);
   //     RenderingManager::Renderer().Draw(shape);

   //     RenderingManager::Renderer().DrawText("Hello, Peach Engine!", font, 24, sf::Vector2f(100, 200), sf::Color::White);

   //     RenderingManager::Renderer().EndFrame();
 //   }

    return EXIT_SUCCESS;
}
