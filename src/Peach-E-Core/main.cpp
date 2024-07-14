/*
    Copyright(c) 2024-present Ranyodh Singh Mandur.
*/
#define STB_IMAGE_IMPLEMENTATION //BLACK MAGIC KEEP HERE IMPORTANT DOOOO NOT MOVE THIS FUCKING HEADER


#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

//#include <GL/glew.h>
//#include <GL/GL.h>

#include <iostream>
#include <string>
#include "../Peach-core/Peach-core.hpp"
#include "../Princess/include/Parsers/PythonScriptParser.h"
#include <pybind11/pybind11.h>

#include <chrono>
#include <thread>
#include <atomic>


std::atomic<bool> m_Running(true);

const float USER_DEFINED_CONSTANT_UPDATE_FPS = 60.0f;
const float USER_DEFINED_UPDATE_FPS = 60.0f;
float USER_DEFINED_RENDER_FPS = 120.0f; //Needs to be adjustable in-game so no const >w<

using namespace PeachCore;

static void RenderFrame() {


    // Render the game scene    
    //PeachCore::RenderingManager::Renderer().renderFrame();

    std::cout << "Rendering frame...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work

}

static void StepPhysicsWorldState(float fp_FixedDeltaTime)
{

}

static void ConstantUpdate(float fp_FixedDeltaTime) {


    // Render the game scene    
    //PeachCore::RenderingManager::Renderer().renderFrame();

    std::cout << "Rendering frame...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work

}

static void Update(float fp_FixedDeltaTime) {


    // Render the game scene    
    //PeachCore::RenderingManager::Renderer().renderFrame();

    std::cout << "Rendering frame...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work

}

static void PollUserInputEvents()
{

}

static void GameLoop()
{
    const float f_PhysicsDeltaTime = 1.0f / USER_DEFINED_CONSTANT_UPDATE_FPS;  // Fixed physics update rate 
    const float f_UserDefinedDeltaTime = 1.0f / USER_DEFINED_UPDATE_FPS;  // User-defined Update() rate
    float f_RenderDeltaTime = 1.0f / USER_DEFINED_RENDER_FPS;  // Should be variable to allow dynamic adjustment in-game

    float f_PhysicsAccumulator = 0.0f;
    float f_GeneralUpdateAccumulator = 0.0f;
    float f_RenderAccumulator = 0.0f;

    auto f_CurrentTime = std::chrono::high_resolution_clock::now();

    while (m_Running) {
        auto f_NewTime = std::chrono::high_resolution_clock::now();
        float f_FrameTime = std::chrono::duration<float>(f_NewTime - f_CurrentTime).count();
        f_CurrentTime = f_NewTime;

        // Prevent spiral of death by clamping frame time, frames will be skipped, but if you're already this behind then thats the least of your problems lmao
        if (f_FrameTime > 0.25)
            {f_FrameTime = 0.25;}

        f_PhysicsAccumulator += f_FrameTime;
        f_GeneralUpdateAccumulator += f_FrameTime;
        f_RenderAccumulator += f_FrameTime;

        PollUserInputEvents();  // Handle user input

        // Physics and fixed interval updates
        while (f_PhysicsAccumulator >= f_PhysicsDeltaTime)
        {
            ConstantUpdate(f_PhysicsDeltaTime);
            StepPhysicsWorldState(f_PhysicsDeltaTime);
            f_PhysicsAccumulator -= f_PhysicsDeltaTime;
        }

        // User-defined game logic updates
        while (f_GeneralUpdateAccumulator >= f_UserDefinedDeltaTime)
        {
            Update(f_UserDefinedDeltaTime);
            f_GeneralUpdateAccumulator -= f_UserDefinedDeltaTime;
        }

        if (f_RenderAccumulator >= f_RenderDeltaTime) {
            RenderFrame();
            f_RenderAccumulator -= f_RenderDeltaTime;
        }
    }
}


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

static void RenderThread()
{
    while (m_Running) {
        // Play audio
        std::cout << "Playing ur mom LOL...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work
    }
}

static void AudioThread()
{
    while (m_Running) {
        // Play audio
        std::cout << "Playing audio...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate work
    }
}

static void ResourceLoadingThread() 
{
    while (m_Running) {
        // Load resources
        std::cout << "Loading resources...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    }
}

static void NetworkThread() 
{
    while (m_Running) {
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

static void SetupLogManagers()
{

    PeachCore::LogManager::MainLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "MainLogger");
    PeachCore::LogManager::AudioLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "AudioLogger");
    PeachCore::LogManager::RenderingLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "RenderingLogger");
    PeachCore::LogManager::ResourceLoadingLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "ResourceLoadingLogger");
    PeachCore::LogManager::NetworkLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "NetworkLogger");
   
    PeachCore::LogManager::MainLogger().Debug("MainLogger successfully initialized", "Peach-E");
    PeachCore::LogManager::AudioLogger().Debug("AudioLogger successfully initialized", "Peach-E");
    PeachCore::LogManager::RenderingLogger().Debug("RenderingLogger successfully initialized", "Peach-E");
    PeachCore::LogManager::ResourceLoadingLogger().Debug("ResourceLoadingLogger successfully initialized", "Peach-E");
    PeachCore::LogManager::NetworkLogger().Debug("NetworkLogger successfully initialized", "Peach-E");

    std::cout << "Hello World!\n"; //>w<

    PeachCore::LogManager::MainLogger().Warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

    PeachCore::LogManager::MainLogger().Trace("Success! This Built Correctly", "Peach-E");
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
    SDL_SetMainReady();  // Tell SDL that the main function is handled and ready
    SetupLogManagers();

    int f_WindowWidth = 800;
    int f_WindowHeight = 600;

    //////////////////////////////////////////////
    // Setup Renderer for 2D
    //////////////////////////////////////////////

    try 
    {
        //PeachCore::RenderingManager::Renderer().SetRendererType(bgfx::RendererType::Vulkan); // Example to force OpenGL
        RenderingManager::Renderer().CreateSDLWindow("SDL Window", f_WindowWidth, f_WindowHeight);
        RenderingManager::Renderer().CreateRenderer2D();
        glViewport(0, 0, f_WindowWidth, f_WindowHeight);
        RenderingManager::Renderer().GetOpenGLRenderer()->RenderFrame();
    }
    catch (const std::exception& ex) {
        std::cerr << "An error occurred: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    //////////////////// Camera Setup ////////////////////
    //NEED TO STILL IMPLEMENT
    //PeachCore::Camera2D camera(f_WindowWidth, f_WindowHeight);
    
    //////////////////// Texture Setup ////////////////////
    Texture2D m_FirstTexture = PeachCore::Texture2D(">w<", "D:/Game Development/Random Junk I Like to Keep/Texture-Tests/owo.jpg");
    //m_FirstTexture = PeachCore::Texture2D("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/spooktacular.png");

    ////////////////// Basic Shader Setup //////////////////
    
    OpenGLRenderer* m_CurrentRenderer = RenderingManager::Renderer().GetOpenGLRenderer();

    ShaderProgram* m_CurrentShaderProgram = m_CurrentRenderer->LoadShadersFromSource("Test_Program",
                                                                                                            "D:/Game Development/Peach-E/src/Peach-core/tests/vertex.glsl", 
                                                                                                            "D:/Game Development/Peach-E/src/Peach-core/tests/fragment.glsl");
    (m_CurrentShaderProgram->Bind());

    

    //m_CurrentShaderProgram->CreateSamplerUniform("texture_sampler");
    m_CurrentShaderProgram->SetTexture("texture_sampler", m_FirstTexture.m_ID, 30);

    //m_CurrentShaderProgram->CreateUniform("model");
    //m_CurrentShaderProgram->CreateUniform("view");
    //m_CurrentShaderProgram->CreateUniform("projection");
    //   
    m_CurrentShaderProgram->SetUniformMat4("model", glm::mat4(1.0));
    m_CurrentShaderProgram->SetUniformMat4("view", glm::mat4(1.0));
    m_CurrentShaderProgram->SetUniformMat4("projection", glm::mat4(1.0));

    //m_CurrentShaderProgram->PrintShaderProgramDebugVerbose();
    //////////////////// Vertex and Index Buffer Setup ////////////////////
    glm::vec2 position = glm::vec2(0.1, 0.1);
    glm::vec2 size = glm::vec2(0.2, 0.2);


    m_CurrentRenderer->DrawTexture(m_FirstTexture.m_ID);
    Vertex vertices[] =
    {
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},  // Bottom-left
        {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},   // Bottom-right
        {0.5f,  0.5f, 0.0f, 1.0f, 1.0f},   // Top-right
        {-0.5f,  0.5f, 0.0f, 0.0f, 1.0f}   // Top-left
    };

    std::vector<unsigned int> indices =
    {
        0, 1, 2,   // First Triangle
        2, 3, 0    // Second Triangle
    };


    //////////////////// Draw Call >w< ////////////////////
    int FrameCount = 0;
    while (true)
    {
        if(FrameCount > 100)
            {break;}
        FrameCount++;
        m_CurrentRenderer->DrawTexture(m_FirstTexture.m_ID); std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    

    m_Running = false;

    //GameLoop();


    /*std::thread T_Audio(AudioThread);
    std::thread T_ResourceLoading(ResourceLoadingThread);
    std::thread T_Network(NetworkThread);*/
    
   /* T_Audio.join();
    T_ResourceLoading.join();
    T_Network.join();*/

    // Clean up
    //PeachCore::ThreadPoolManager::ThreadPool().Shutdown
    //Princess::PythonScriptParser::Parser().ExtractFunctionInformationFromPythonModule("Test-Function-Read");




    //////////////////////////////////////////////////////////////////////////////////////////

    //std::vector<std::string> ListOfWindowsPluginsToLoad = { "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin.dll",
    //                                                                                           "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin2.dll" };
    //std::vector<std::string> ListOfUnixPluginsToLoad = { };

    //#if defined(_WIN32) || defined(_WIN64)
    //    LoadPluginsFromConfigs(ListOfWindowsPluginsToLoad); // Windows
    //#else
    //    LoadPluginsFromConfigs(ListOfUnixPluginsToLoad); // Linux/Unix
    //#endif

    //RunPlugins();
  
    //PeachCore::LogManager::MainLogger().Debug("Exit Success!", "Peach-E");

    return EXIT_SUCCESS;
}







//////////////////////////////////////////////
// Setting Up SDL Context
//////////////////////////////////////////////

//static void CreateSDLWindow()
//{
//
//    // Initialize SDL
//    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
//        return;
//    }
//
//    // Create a window
//    SDL_Window* window = SDL_CreateWindow("SDL Window",
//        SDL_WINDOWPOS_UNDEFINED,
//        SDL_WINDOWPOS_UNDEFINED,
//        640, 480,
//        SDL_WINDOW_SHOWN);
//    if (window == nullptr) {
//        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
//        SDL_Quit();
//        return;
//    }
//
//    bool quit = false;
//    SDL_Event e;
//
//    // Main loop
//    while (!quit) {
//        // Event handling
//        while (SDL_PollEvent(&e) != 0) {
//            if (e.type == SDL_QUIT) {
//                quit = true;
//            }
//            else if (e.type == SDL_KEYDOWN) {
//                if (e.key.keysym.sym == SDLK_SPACE) {
//                    quit = true;
//                }
//            }
//        }
//
//        // Clear screen (optional)
//        SDL_SetRenderDrawColor(SDL_GetRenderer(window), 0x00, 0x00, 0x00, 0xFF);
//        SDL_RenderClear(SDL_GetRenderer(window));
//
//        // Render stuff here (if any)
//
//        // Update screen
//        SDL_RenderPresent(SDL_GetRenderer(window));
//    }
//
//    // Destroy window and quit SDL
//    SDL_DestroyWindow(window);
//    SDL_Quit();
//
//}