/*
    Copyright(c) 2024-present Ranyodh Singh Mandur.
*/
#include "angelscript.h"
#include <iostream>
#include <string>
#include "../Peach-core/Peach-core.hpp"
#include "../Princess/include/Parsers/PythonScriptParser.h"
#include <pybind11/pybind11.h>

#include <chrono>
#include <thread>
#include <atomic>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>




static void LoadPluginsFromConfigs(const std::vector<std::string>& fp_ListOfPluginsToLoad)
{
    for (int index = 0; index < fp_ListOfPluginsToLoad.size(); index++)
    {
        PeachCore::PluginManager::ManagePlugins().LoadPlugin(fp_ListOfPluginsToLoad[index]);
    }
}

void test()
{

}

std::atomic<bool> running(true);

void GameLogicThread() {
    while (running) {
        // Process user input
        // Update game state
        // Handle physics, AI, etc.
        std::cout << "Updating game logic...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work
    }
}

void RenderThread() {
    while (running) {
        // Render the game scene
        std::cout << "Rendering frame...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work
    }
}

void AudioThread() {
    while (running) {
        // Play audio
        std::cout << "Playing audio...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work
    }
}

void ResourceLoadingThread() {
    while (running) {
        // Load resources
        std::cout << "Loading resources...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    }
}

static void NetworkThread() {
    while (running) {
        // Handle network communication
        std::cout << "Handling network...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work
    }
}


//////////////////////////////////////////////
// Setting Up Renderer For Engine
//////////////////////////////////////////////

static void SetupRenderer()
{
    return;
    //PeachCore::RenderingManager::Renderer().Initialize("OpenGL");

   //sf::RenderWindow& m_CurrentRenderWindow = PeachCore::RenderingManager::Renderer().GetRenderWindow();

   // // Set global settings
   //PeachCore::RenderingManager::Renderer().SetFrameRateLimit(120);
   //PeachCore::RenderingManager::Renderer().SetVSync(true);

   //std::string f_PathToFontFile = "D:/Game Development/Fonts I guess/Comic Sans MS";

   // sf::Font font;
   // if (!font.loadFromFile(f_PathToFontFile)) {
   //     PeachCore::LogManager::Logger().Warn("Load Error: Unable to load font file from specified path: " + f_PathToFontFile, "Peach-E");
   // }
}

//////////////////////////////////////////////
// Setting Up SDL Context
//////////////////////////////////////////////

static void CreateSDLWindow()
{

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("SDL Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    bool quit = false;
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Event handling
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    quit = true;
                }
            }
        }

        // Clear screen (optional)
        SDL_SetRenderDrawColor(SDL_GetRenderer(window), 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(SDL_GetRenderer(window));

        // Render stuff here (if any)

        // Update screen
        SDL_RenderPresent(SDL_GetRenderer(window));
    }

    // Destroy window and quit SDL
    SDL_DestroyWindow(window);
    SDL_Quit();

}

//////////////////////////////////////////////
// Loading and Running Plugins From DLL'S
//////////////////////////////////////////////

static void RunPlugins()
{
        PeachCore::PluginManager::ManagePlugins().InitializePlugins();
        PeachCore::PluginManager::ManagePlugins().UpdatePlugins(0.1f);
        PeachCore::PluginManager::ManagePlugins().ShutdownPlugins();
}

//////////////////////////////////////////////
// Setting Up and Setting Output Directory
//////////////////////////////////////////////

static void SetupLogManager()
{
    PeachCore::LogManager::Logger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE");

    PeachCore::LogManager::Logger().Debug("LogManager successfully initialized", "LogManager");

    std::cout << "Hello World!\n";

    PeachCore::LogManager::Logger().Warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

    PeachCore::LogManager::Logger().Trace("Success! This Built Correctly", "Peach-E");
}

//PYBIND11_MODULE(peach_engine, fp_Module)
//{
//    PeachCore::PythonScriptManager::Python().InitializePythonBindingsForPeachCore(fp_Module);
//}


//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int main(int argc, char* argv[])
{
    try {
        asIScriptEngine* engine = PeachCore::ScriptEngineManager::ScriptEngine().CreateScriptEngine();

        const char* const script = R"(
            void main() {
                print("Hello from AngelScript!");
                string s = "Test string";
                print(s);
                int x = 26;
                print(x);
                float m = 132.342326246;
                print(m);
            }
        )";

        PeachCore::ScriptEngineManager::ScriptEngine().ExecuteScript(engine, script);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::thread gameLogic(GameLogicThread);
    std::thread render(RenderThread);
    std::thread audio(AudioThread);
    std::thread resourceLoading(ResourceLoadingThread);
    std::thread network(NetworkThread);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    running = false;

    gameLogic.join();
    render.join();
    audio.join();
    resourceLoading.join();
    network.join();

    CreateSDLWindow();
    SetupRenderer();
    SetupLogManager();

     
    


    // Clean up
    //PeachCore::ThreadPoolManager::ThreadPool().Shutdown();

    //Princess::PythonScriptParser::Parser().ExtractFunctionInformationFromPythonModule("Test-Function-Read");


   
    std::vector<std::string> ListOfWindowsPluginsToLoad = { "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin.dll",
                                                                                               "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin2.dll" };
    std::vector<std::string> ListOfUnixPluginsToLoad = { };

    #if defined(_WIN32) || defined(_WIN64)
        LoadPluginsFromConfigs(ListOfWindowsPluginsToLoad); // Windows
    #else
        LoadPluginsFromConfigs(ListOfUnixPluginsToLoad); // Linux/Unix
    #endif

    RunPlugins();
  
    PeachCore::LogManager::Logger().Debug("Exit Success!", "Peach-E");

    return EXIT_SUCCESS;
}
