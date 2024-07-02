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

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>



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

void test()
{

}

// Component definitions
struct Position { float x, y; };
struct Velocity { float vx, vy; };
struct Health { int hp; };
struct MoveEvent {
    entt::entity entity;
    float x, y;
};

// System definitions
void PlayerSystem(entt::registry& registry, float deltaTime) {
    auto view = registry.view<Position, Velocity>();
    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& vel = view.get<Velocity>(entity);
        pos.x += vel.vx * deltaTime;
        pos.y += vel.vy * deltaTime;
        std::cout << "Player position: (" << pos.x << ", " << pos.y << ")\n";
    }
}

void EnemySystem(entt::registry& registry, float deltaTime) {
    auto view = registry.view<Position, Health>();
    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& health = view.get<Health>(entity);
        std::cout << "Enemy position: (" << pos.x << ", " << pos.y << ") with HP: " << health.hp << "\n";
    }
}

void MovementSystem(entt::registry& registry, float deltaTime) {
    auto view = registry.view<Position, Velocity>();
    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& vel = view.get<Velocity>(entity);
        pos.x += vel.vx * deltaTime;
        pos.y += vel.vy * deltaTime;
    }
}

void HealthSystem(entt::registry& registry, float deltaTime) {
    auto view = registry.view<Health>();
    for (auto entity : view) {
        auto& health = view.get<Health>(entity);
        if (health.hp <= 0) {
            std::cout << "Entity " << entt::to_integral(entity) << " has died.\n";
            registry.destroy(entity);
        }
    }
}

void PrintMessage(const std::string& msg, int duration = 0) {
    std::cout << msg << std::endl;
    if (duration > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(duration));
    }
}

void TestEventTasks() {
    std::cout << "Starting Test: TestEventTasks" << std::endl;

    // Enqueue event tasks
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueEventBatch({
        []() { PrintMessage("Event Task 1", 2); },
        []() { PrintMessage("Event Task 2"); }
        });

    // Enqueue user tasks with different priorities
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueOneTime([]() { PrintMessage("User Task 1 - Priority 1"); }, 1);
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueOneTime([]() { PrintMessage("User Task 2 - Priority 2"); }, 2);

    // Start processing tasks
    PeachCore::ThreadPoolManager::ThreadPool().ProcessTasks();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Finished Test: TestEventTasks" << std::endl;
}

void TestHardEventSync() {
    std::cout << "Starting Test: TestHardEventSync" << std::endl;

    PeachCore::ThreadPoolManager::ThreadPool().SetEnforceHardEventSync(true);

    // Enqueue event tasks
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueEventBatch({
        []() { PrintMessage("Event Task 1", 2); },
        []() { PrintMessage("Event Task 2"); }
        });

    // Enqueue user tasks with different priorities
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueOneTime([]() { PrintMessage("User Task 1 - Priority 1"); }, 1);
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueOneTime([]() { PrintMessage("User Task 2 - Priority 2"); }, 2);

    // Start processing tasks
    PeachCore::ThreadPoolManager::ThreadPool().ProcessTasks();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Finished Test: TestHardEventSync" << std::endl;

    PeachCore::ThreadPoolManager::ThreadPool().SetEnforceHardEventSync(false);
}

void TestContinuousTasks() {
    std::cout << "Starting Test: TestContinuousTasks" << std::endl;

    // Enqueue continuous tasks
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueContinuous([]() { PrintMessage("Continuous Task 1 - Priority 0"); }, 0);
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueContinuous([]() { PrintMessage("Continuous Task 2 - Priority 1"); }, 1);

    // Enqueue user tasks with different priorities
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueOneTime([]() { PrintMessage("User Task 1 - Priority 1"); }, 1);
    PeachCore::ThreadPoolManager::ThreadPool().EnqueueOneTime([]() { PrintMessage("User Task 2 - Priority 2"); }, 2);

    // Start processing tasks
    PeachCore::ThreadPoolManager::ThreadPool().ProcessTasks();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Finished Test: TestContinuousTasks" << std::endl;
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

    TestEventTasks();
    TestHardEventSync();
    TestContinuousTasks();

    // Clean up
    PeachCore::ThreadPoolManager::ThreadPool().Shutdown();




    //Princess::PythonScriptParser::Parser().ExtractFunctionInformationFromPythonModule("Test-Function-Read");

    //std::vector<std::string> ListOfWindowsPluginsToLoad = { "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin.dll",
    //                                                                                         "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin2.dll" };
    //std::vector<std::string> ListOfUnixPluginsToLoad = { };

    //PeachCore::LogManager::Logger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE");
    //PeachCore::LogManager::Logger().Initialize("D:/Game Development/Random Junk I Like to Keep/Bigga");

    //PeachCore::LogManager::Logger().Debug("LogManager successfully initialized", "LogManager");
    //std::cout << "Hello World!\n";
    //PeachCore::LogManager::Logger().Warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

    //PeachCore::LogManager::Logger().Trace("Success! This Built Correctly", "Peach-E");

    ////////////////////////////////////////////////////
    //////// Plugin Loader Step
    ////////////////////////////////////////////////////

    //#if defined(_WIN32) || defined(_WIN64)
    //    LoadPluginsFromConfigs(ListOfWindowsPluginsToLoad); // Windows
    //#else
    //    LoadPluginsFromConfigs(ListOfUnixPluginsToLoad); // Linux/Unix
    //#endif

    //PeachCore::PluginManager::ManagePlugins().InitializePlugins();
    //PeachCore::PluginManager::ManagePlugins().UpdatePlugins(0.1f);
    //PeachCore::PluginManager::ManagePlugins().ShutdownPlugins();

    ////////////////////////////////////////////////
    //// Setting Up Renderer For Engine
    ////////////////////////////////////////////////
   
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


   // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
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
        return 1;
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

    PeachCore::LogManager::Logger().Debug("Exit Success!", "Peach-E");

    return EXIT_SUCCESS;
}
