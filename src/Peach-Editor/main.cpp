/*******************************************************************
 *                                        Peach Editor v0.0.7
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                 Peach Editor is an open source peach_editor for Peach-E
********************************************************************/
#define SDL_MAIN_HANDLED

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION

#include <pybind11/pybind11.h>

#include "../../include/Peach-Editor/Managers/PeachEditorRenderingManager.h"

using namespace std;
using namespace PeachEditor;
using namespace PeachEngine;

constexpr int FAILED_TO_CREATE_MAIN_WINDOW = -1000;
constexpr int FAILED_TO_INITIALIZE_OPENGL = -1001;

static void RenderThread()
{
    while (true)
    {
        // Play audio
        cout << "Playing ur mom LOL...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void AudioThread()
{
    while (true) 
    {
        // Play audio
        cout << "Playing audio...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void ResourceLoadingThread() 
{
    while (true) 
    {
        // Load resources
        cout << "Loading resources...\n";
        this_thread::sleep_for(chrono::milliseconds(100)); // Simulate work
    }
}

static void NetworkThread() 
{
    while (true) 
    {
        // Handle network communication
        cout << "Handling network...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

//PYBIND11_MODULE(peach_engine, fp_Module)
//{
//    PythonScriptManager::Python().InitializePythonBindingsForPeachCore(fp_Module);
//}

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int main(int fp_ArgCount, char* fp_ArgVector[])
{
    cout << fp_ArgVector[0] << "\n"; //COOL AF

    ////////////////////////////////////////////////
    // Setup Loggers
    ////////////////////////////////////////////////

    auto peach_editor = &PeachEditorManager::PeachEditor();
    auto peach_engine = &PeachEngineManager::PeachEngine();

    peach_editor->SetupInternalLogManagers();
    peach_engine->SetupLogManagers();

    peach_engine->InitializePhysFS(fp_ArgVector[0]);
    
    ////////////////////////////////////////////////
    // Load Plugins
    ////////////////////////////////////////////////

    //DLL's
    vector<string>
        mf_ListOfWindowsPluginsToLoad =
    {
        "../plugins/SimplePlugin.dll",
        "../plugins/SimplePlugin2.dll"
    };

    //SO's
    vector<string>
        mf_ListOfUnixPluginsToLoad =
    {
    };

    #if defined(_WIN32) || defined(_WIN64)
        peach_engine->LoadPluginsFromConfigs(mf_ListOfWindowsPluginsToLoad); // Windows
    #else
        peach_engine->LoadPluginsFromConfigs(mf_ListOfUnixPluginsToLoad); // Linux/Unix
    #endif

    peach_engine->RunPlugins();

    ////////////////////////////////////////////////
    // Setup Communication Queues
    ////////////////////////////////////////////////

    //used for pushing update commands to the Render Thread
    shared_ptr<PeachCore::CommandQueue> mf_PeachEditorRenderingManagersCommandQueue; //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out
    shared_ptr<PeachCore::LoadingQueue> mf_PeachEditorDrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

    shared_ptr<PeachCore::CommandQueue> mf_AudioManagersCommandQueue; //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out
    shared_ptr<PeachCore::LoadingQueue> mf_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager

    // ObjectID : SceneTreeItem : Associated Update Package, used for updating all relevant data at the same time
    //map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllCurrentlyActivePeachNodes;
    //map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllPeachNodesQueuedForRemoval;

    ////////////////////////////////////////////////
    // Setup Peach Editor Renderer
    ////////////////////////////////////////////////

    const unsigned int mf_MainWindowWidth = 800;
    const unsigned int mf_MainWindowHeight = 600;

    auto editor_renderer = &PeachEditorRenderingManager::PeachEngineRenderer();
    auto main_logger = &InternalLogManager::InternalMainLogger();

    //Initialize methods, RenderingManager is special because we need two way communication, so RenderingManager issues one and only one copy of the commandqueue sharedptr for the main thread to use judiciously
    mf_PeachEditorDrawableResourceLoadingQueue = PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue();

    mf_PeachEditorRenderingManagersCommandQueue = editor_renderer->InitializeQueues();

    if (not editor_renderer->CreateSDLWindow("Peach Engine", mf_MainWindowWidth, mf_MainWindowHeight))
    {
        main_logger->Fatal("Was not able to create the main window, exiting execution immediately", "main_thread");
        return FAILED_TO_CREATE_MAIN_WINDOW;
    }

    main_logger->Debug("SDL window successfully created for Peach Editor", "main_thread");

    if (not editor_renderer->InitializeOpenGL())
    {
        main_logger->Fatal("Was not able to initialize a valid OpenGL context, exiting execution immediately", "main_thread");
        return FAILED_TO_INITIALIZE_OPENGL;
    }

    main_logger->Debug("Peach Editor successfully initialized OpenGL", "main_thread");

    bool mf_IsProgramRuntimeOver = false;
    
    while (not mf_IsProgramRuntimeOver)
    {
        editor_renderer->RenderFrame(&mf_IsProgramRuntimeOver);
    }

    editor_renderer->Shutdown();

    SDL_Quit(); //just makes more sense to have the main method do this

//    if (not AudioManager::AudioPlayer().Initialize())
//    {
//        //do something idk
//    }
// 
    //maybe do a ResourceLoadingManager initialize if needed, idk feels weird that we dont have one yet and it just works lmao, i guess its the lazy initialization
//    AudioResourceLoadingQueue = ResourceLoadingManager::ResourceLoader().GetAudioResourceLoadingQueue();

//PeachEngineResourceLoadingManager::PeachEngineResourceLoader().LoadTextureFromSpecifiedFilePath("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/uwu.png");

    //this_thread::sleep_for(chrono::milliseconds(500));
    /*thread T_Audio(AudioThread);
    thread T_ResourceLoading(ResourceLoadingThread);
    thread T_Network(NetworkThread);*/
    
   /* T_Audio.join();
    T_ResourceLoading.join();
    T_Network.join();*/

    // Clean up
    //ThreadPoolManager::ThreadPool().Shutdown
    //Princess::PythonScriptParser::Parser().ExtractFunctionInformationFromPythonModule("Test-Function-Read");

  
    main_logger->Debug("Exit Success!", "Peach-E");

    return EXIT_SUCCESS;
}












//static void
//RunGameInstance()
//{
//    // Assuming this function sets up, runs, and tears down the game environment
//    //PeachCore::RenderingManager::Renderer().CreateWindowAndCamera2D("Peach Game", 800, 600);
//    //PeachCore::RenderingManager::Renderer().RenderFrame(true); //set IsStressTest to true, renders only one frame then exits immediately
//    PeachCore::RenderingManager::Renderer().Shutdown();
//}
//
//static void
//StessTest(int numIterations, int delayMs)
//{
//    for (int i = 0; i < numIterations; ++i)
//    {
//        cout << "Iteration: " << i + 1 << endl;
//
//        // Start the game instance in a thread
//        thread gameThread(RunGameInstance);
//
//        // Allow the thread some time to start and initialize resources
//        this_thread::sleep_for(chrono::milliseconds(delayMs));
//
//        // Assuming the game can be stopped by calling a stop function or similar
//        // If your architecture uses a global or static flag to signal shutdown, set it here
//        PeachCore::RenderingManager::Renderer().ForceQuit();
//
//        // Wait for the thread to finish execution
//        gameThread.join();
//
//        // Allow some time for resources to be fully released
//        this_thread::sleep_for(chrono::milliseconds(delayMs));
//    }
//}




//////////////////////////////////////////////
// Setting Up Renderer For Engine
//////////////////////////////////////////////
//ARTIFACT FOR MUSEUM VIEWING ONLY
//static void SetupRenderer()
//{
//    return;
//    //RenderingManager::Renderer().Initialize("OpenGL");
//
//   //sf::RenderWindow& m_CurrentRenderWindow = RenderingManager::Renderer().GetRenderWindow();
//
//   // // Set global settings
//   //RenderingManager::Renderer().SetFrameRateLimit(120);
//   //RenderingManager::Renderer().SetVSync(true);
//
//   //string f_PathToFontFile = "D:/Game Development/Fonts I guess/Comic Sans MS";
//
//   // sf::Font font;
//   // if (!font.loadFromFile(f_PathToFontFile)) {
//   //     LogManager::Logger().Warn("Load Error: Unable to load font file from specified path: " + f_PathToFontFile, "Peach-E");
//   // }
//}