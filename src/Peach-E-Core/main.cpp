/*
    Copyright(c) 2024-present Ranyodh Singh Mandur.
*/
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <variant>

#include <pybind11/pybind11.h>

#include "../Peach-core/Peach-core.hpp"
#include "../Princess/include/Parsers/PythonScriptParser.h"

using namespace PeachCore;
using namespace std;

atomic<bool> m_Running(true);

const float USER_DEFINED_CONSTANT_UPDATE_FPS = 60.0f;
const float USER_DEFINED_UPDATE_FPS = 60.0f;
float USER_DEFINED_RENDER_FPS = 120.0f; //Needs to be adjustable in-game so no const >w<

weak_ptr<CommandQueue> m_RenderingManagersCommandQueue; //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out

weak_ptr<LoadingQueue> m_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager
weak_ptr<LoadingQueue> m_DrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

// ObjectID : SceneTreeItem : Associated Update Package, used for updating all relevant data at the same time
//map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllCurrentlyActivePeachNodes;

static void RenderFrame() {


    // Render the game scene    
    //PeachCore::RenderingManager::Renderer().renderFrame();

    cout << "Rendering frame...\n";
    this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work

}

static void StepPhysicsWorldState(float fp_FixedDeltaTime)
{

}

static void ConstantUpdate(float fp_FixedDeltaTime) {


    // Render the game scene    
    //PeachCore::RenderingManager::Renderer().renderFrame();

    cout << "Rendering frame...\n";
    this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work

}

static void Update(float fp_FixedDeltaTime) {


    // Render the game scene    
    //PeachCore::RenderingManager::Renderer().renderFrame();

    cout << "Rendering frame...\n";
    this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work

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

    auto f_CurrentTime = chrono::high_resolution_clock::now();

    while (m_Running) {
        auto f_NewTime = chrono::high_resolution_clock::now();
        float f_FrameTime = chrono::duration<float>(f_NewTime - f_CurrentTime).count();
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


static void LoadPluginsFromConfigs(const vector<string>& fp_ListOfPluginsToLoad)
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
        cout << "Playing ur mom LOL...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void AudioThread()
{
    while (m_Running) {
        // Play audio
        cout << "Playing audio...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void ResourceLoadingThread() 
{
    while (m_Running) {
        // Load resources
        cout << "Loading resources...\n";
        this_thread::sleep_for(chrono::milliseconds(100)); // Simulate work
    }
}

static void NetworkThread() 
{
    while (m_Running) {
        // Handle network communication
        cout << "Handling network...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
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

   //string f_PathToFontFile = "D:/Game Development/Fonts I guess/Comic Sans MS";

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

    cout << "Hello World!\n"; //>w<

    PeachCore::LogManager::MainLogger().Warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

    PeachCore::LogManager::MainLogger().Trace("Success! This Built Correctly", "Peach-E");
}

//PYBIND11_MODULE(peach_engine, fp_Module)
//{
//    PeachCore::PythonScriptManager::Python().InitializePythonBindingsForPeachCore(fp_Module);
//}

//static void PushCommands(const CreateData& createData, const UpdateData& updateData, const DeleteData& deleteData) 
//{
//    if (!createData.objectIDs.empty()) {
//        Command createCmd{ CommandType::CreateAsset, createData };
//        commandQueue.push(createCmd);
//    }
//    if (!updateData.objectIDs.empty()) {
//        Command updateCmd{ CommandType::UpdateAsset, updateData };
//        commandQueue.push(updateCmd);
//    }
//    if (!deleteData.objectIDs.empty()) {
//        Command deleteCmd{ CommandType::DeleteAsset, deleteData };
//        commandQueue.push(deleteCmd);
//    }
//}

static void IssueLoadingCommands()
{

}

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int main(int argc, char* argv[])
{
    SetupLogManagers();

    int f_WindowWidth = 800;
    int f_WindowHeight = 600;

    //////////////////////////////////////////////
    // Setup Renderer for 2D
    //////////////////////////////////////////////

    try 
    {
        m_RenderingManagersCommandQueue = RenderingManager::Renderer().Initialize("Peach Engine", f_WindowWidth, f_WindowHeight);
        //RenderingManager::Renderer().GetOpenGLRenderer()->RenderFrame();
    }
    catch (const exception& ex) 
    {
        cerr << "An error occurred: " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    IssueLoadingCommands();

    //////////////////// Camera Setup ////////////////////
    //NEED TO STILL IMPLEMENT
    //PeachCore::Camera2D camera(f_WindowWidth, f_WindowHeight);
    
    ////////////////////// Texture Setup ////////////////////
    //Texture2D m_FirstTexture = PeachCore::Texture2D(">w<", "D:/Game Development/Random Junk I Like to Keep/Texture-Tests/owo.jpg");
    ////m_FirstTexture = PeachCore::Texture2D("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/spooktacular.png");


    m_DrawableResourceLoadingQueue = ResourceLoadingManager::ResourceLoader().GetDrawableResourceLoadingQueue();

    ResourceLoadingManager::ResourceLoader().LoadTexture("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/owo.jpg");


    //this_thread::sleep_for(chrono::milliseconds(200));


    //RenderingManager::Renderer().ProcessLoadedResourcePackages();

    RenderingManager::Renderer().RenderFrame();














    //////////////////// Basic Shader Setup //////////////////


    //////////////////// Draw Call >w< ////////////////////
  
     //while (m_Running) {

    //    // Set up shaders
    //    m_CurrentShaderProgram.Bind();
    //    m_CurrentShaderProgram.SetTexture("texture_sampler", m_FirstTexture.m_ID, 0);
    //    //m_CurrentShaderProgram.SetUniformMat4("model", glm::mat4(1.0));
    //    //m_CurrentShaderProgram.SetUniformMat4("view", glm::mat4(1.0));
    //    //m_CurrentShaderProgram.SetUniformMat4("projection", glm::mat4(1.0));

    //    // Draw
    //    m_CurrentRenderer->DrawTexture(m_FirstTexture.m_ID);

    //    this_thread::sleep_for(chrono::milliseconds(60));
    //}


    m_Running = false;

    //GameLoop();


    /*thread T_Audio(AudioThread);
    thread T_ResourceLoading(ResourceLoadingThread);
    thread T_Network(NetworkThread);*/
    
   /* T_Audio.join();
    T_ResourceLoading.join();
    T_Network.join();*/

    // Clean up
    //PeachCore::ThreadPoolManager::ThreadPool().Shutdown
    //Princess::PythonScriptParser::Parser().ExtractFunctionInformationFromPythonModule("Test-Function-Read");




    ////////////////////////////////// Plugins ///////////////////////////////////////////

    vector<string> ListOfWindowsPluginsToLoad = { "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin.dll",
                                                                                               "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin2.dll" };
    vector<string> ListOfUnixPluginsToLoad = { };

    #if defined(_WIN32) || defined(_WIN64)
        LoadPluginsFromConfigs(ListOfWindowsPluginsToLoad); // Windows
    #else
        LoadPluginsFromConfigs(ListOfUnixPluginsToLoad); // Linux/Unix
    #endif

    RunPlugins();
  
    PeachCore::LogManager::MainLogger().Debug("Exit Success!", "Peach-E");

    return EXIT_SUCCESS;
}