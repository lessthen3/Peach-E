/*
    Copyright(c) 2024-present Ranyodh Singh Mandur.
*/

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

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

    /*std::thread T_Audio(AudioThread);
    std::thread T_ResourceLoading(ResourceLoadingThread);
    std::thread T_Network(NetworkThread);*/

    //////////////////////////////////////////////
    // Setup Renderer for 2D
    //////////////////////////////////////////////

    try 
    {
        //PeachCore::RenderingManager::Renderer().SetRendererType(bgfx::RendererType::Vulkan); // Example to force OpenGL
        PeachCore::RenderingManager::Renderer().CreateSDLWindow("SDL Window", f_WindowWidth, f_WindowHeight);
        PeachCore::RenderingManager::Renderer().CreateRenderer2D();
    }
    catch (const std::exception& ex) {
        std::cerr << "An error occurred: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    //////////////////// Camera Setup ////////////////////

    PeachCore::Camera2D camera(f_WindowWidth, f_WindowHeight);
    //bgfx::setViewTransform(0, camera.GetViewMatrix(), nullptr); // No projection matrix set here, assuming it's handled in Camera2D

    //////////////////// Texture Setup ////////////////////

    //PeachCore::Texture2D m_FirstTexture = PeachCore::Texture2D("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/owo.jpg");
    //m_FirstTexture = PeachCore::Texture2D("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/spooktacular.png");

    //////////////////// Vertex and Index Buffer Setup ////////////////////

    // Assuming you have these already defined somewhere
    struct Vertex {
        float x, y;
        float u, v;
    };

    // Vertex data for a full-screen quad
    Vertex vertices[] = {
        {-1.0f, -1.0f, 0.0f, 1.0f},
        {1.0f, -1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-1.0f, 1.0f, 0.0f, 0.0f}
    };
    const uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };

    //bgfx::VertexLayout f_VertexLayout;

    //f_VertexLayout.begin()
    //    .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float) // 2D position (x, y)
    //    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float) // Texture coordinates (u, v)
    //    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
    //    .end();

    //auto vertexBuffer = bgfx::createVertexBuffer(
    //    bgfx::makeRef(vertices, sizeof(vertices)),
    //    f_VertexLayout
    //);
    //auto indexBuffer = bgfx::createIndexBuffer(
    //    bgfx::makeRef(indices, sizeof(indices))
    //);

    //bgfx::setVertexBuffer(0, vertexBuffer);
    //bgfx::setIndexBuffer(indexBuffer);

    //bgfx::UniformHandle f_TextureUniform = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);


    //// Bind texture
    //bgfx::setTexture(0, f_TextureUniform, m_FirstTexture.GetTextureHandle());

    //////////////////// Basic Shader Setup ////////////////////

    //PeachCore::ShaderProgram f_ShaderProgram;

    // Load and compile the vertex shader
    //if (!f_ShaderProgram.LoadAndCompileShaders("D:/Game Development/Random Junk I Like to Keep/VertexShaderTest.glsl", "D:/Game Development/Random Junk I Like to Keep/FragmentShaderTest.glsl")) {
    //    std::cerr << "Failed to compile the shaders." << std::endl;
    //    return EXIT_FAILURE;
    //}

    //// Finalize and create the shader program
    //if (!f_ShaderProgram.FinalizeProgram()) {
    //    std::cerr << "Failed to create the shader program." << std::endl;
    //    return EXIT_FAILURE;
    //}

    // Draw call
    //bgfx::submit(0, f_ShaderProgram.GetProgram());

    //bgfx::frame();



    std::this_thread::sleep_for(std::chrono::seconds(5));

    m_Running = false;

    //GameLoop();

    
   /* T_Audio.join();
    T_ResourceLoading.join();
    T_Network.join();*/
    







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
  
    PeachCore::LogManager::MainLogger().Debug("Exit Success!", "Peach-E");

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