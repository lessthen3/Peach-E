#include <iostream>
#include "../Peach-core/Managers/Managers.h"
#include "../Language-Support/Princess/Parsers/PythonScriptParser.h"
#include "SFML/Graphics.hpp"

static void LoadPluginsFromConfigs(const std::vector<std::string>& fp_ListOfPluginsToLoad)
{
    for (int index = 0; index < fp_ListOfPluginsToLoad.size(); index++)
    {
        PeachCore::PluginManager::ManagePlugins().LoadPlugin(fp_ListOfPluginsToLoad[index]);
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
    Princess::PythonScriptParser::Parser().ExtractFunctionInformationFromPythonModule();

    std::vector<std::string> ListOfWindowsPluginsToLoad = { "D:/Game Development/Peach-E/src/Peach-E-Core/SimplePlugin.dll",
                                                                                             "D:/Game Development/Peach-E/src/Peach-E-Core/SimplePlugin2.dll" };
    std::vector<std::string> ListOfUnixPluginsToLoad = { };

    PeachCore::LogManager::Logger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE");
    PeachCore::LogManager::Logger().Initialize("D:/Game Development/Random Junk I Like to Keep/Bigga");

    PeachCore::LogManager::Logger().Debug("LogManager successfully initialized", "LogManager");
    std::cout << "Hello World!\n";
    PeachCore::LogManager::Logger().Warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

    PeachCore::LogManager::Logger().Trace("Success! This Built Correctly", "Peach-E");

    ////////////////////////////////////////////////
    //// Plugin Loader Step
    ////////////////////////////////////////////////

    #if defined(_WIN32) || defined(_WIN64)
        LoadPluginsFromConfigs(ListOfWindowsPluginsToLoad); // Windows
    #else
        LoadPluginsFromConfigs(ListOfUnixPluginsToLoad); // Linux/Unix
    #endif

    PeachCore::PluginManager::ManagePlugins().InitializePlugins();
    PeachCore::PluginManager::ManagePlugins().UpdatePlugins(0.1f);
    PeachCore::PluginManager::ManagePlugins().ShutdownPlugins();

    ////////////////////////////////////////////////
    //// Setting Up Renderer For Engine
    ////////////////////////////////////////////////
    
   PeachCore::RenderingManager::Renderer().Initialize("OpenGL");

   sf::RenderWindow& m_CurrentRenderWindow = PeachCore::RenderingManager::Renderer().GetRenderWindow();

    // Set global settings
   PeachCore::RenderingManager::Renderer().SetFrameRateLimit(120);
   PeachCore::RenderingManager::Renderer().SetVSync(true);

   std::string f_PathToFontFile = "D:/Game Development/Fonts I guess/Comic Sans MS";

    sf::Font font;
    if (!font.loadFromFile(f_PathToFontFile)) {
        PeachCore::LogManager::Logger().Warn("Load Error: Unable to load font file from specified path: " + f_PathToFontFile, "Peach-E");
    }


    while (m_CurrentRenderWindow.isOpen()) {
        sf::Event event;
        while (m_CurrentRenderWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_CurrentRenderWindow.close();
            }
            //break;
        }

        PeachCore::RenderingManager::Renderer().BeginFrame();
        PeachCore::RenderingManager::Renderer().Clear();

        // Example drawing calls
        sf::CircleShape shape(50);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(100, 100);
        PeachCore::RenderingManager::Renderer().Draw(shape);

        PeachCore::RenderingManager::Renderer().DrawTextToScreen("Hello, Peach Engine!", font, 24, sf::Vector2f(100, 200), sf::Color::White);

        PeachCore::RenderingManager::Renderer().EndFrame();
        //break;
    }
    PeachCore::LogManager::Logger().Debug("Exit Success!", "Peach-E");


    return EXIT_SUCCESS;
}
