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

#include <pybind11/pybind11.h>

#include "../../include/Peach-Editor/Managers/PeachEditorRenderingManager.h"

using namespace std;
using namespace PeachEditor;
using namespace PeachEngine;

namespace fs = filesystem;

constexpr int FAILED_TO_CREATE_MAIN_WINDOW = -1000;
constexpr int FAILED_TO_INITIALIZE_OPENGL = -1001;

static atomic<bool> m_Running(true);

// Function to list all files recursively
static unordered_map<string, fs::file_time_type> 
    GetCurrentDirectoryState
    (
        const fs::path& fp_Directory
    ) 
{
    unordered_map<string, fs::file_time_type> f_Files;

    try 
    {
        for (const auto& _entry : fs::recursive_directory_iterator(fp_Directory))
        {
            if (fs::is_regular_file(_entry.status()) || fs::is_directory(_entry.status()))
            {
                f_Files[_entry.path().string()] = fs::last_write_time(_entry);
            }
        }
    }
    catch (const fs::filesystem_error& e) 
    {
        cerr << "Error: " << e.what() << '\n';
    }

    return f_Files;
}

// Function to compare two fs states, returns true if file_system_1 == file_system_2, returns false otherwise
static bool 
    CompareStates
    (
        const unordered_map<string, 
        fs::file_time_type>& fp_OldState, 
        const unordered_map <string, fs::file_time_type>& fp_NewState
    ) 
{
    for (const auto& _file : fp_NewState)
    {
        auto it = fp_OldState.find(_file.first);

        if (it == fp_OldState.end())
        {
            cout << "New file: " << _file.first << '\n';
            return false;
        }
        else if (it->second != _file.second)
        {
            cout << "Modified file: " << _file.first << '\n';
            return false;
        }
    }

    for (const auto& _file : fp_OldState)
    {
        if (fp_NewState.find(_file.first) == fp_NewState.end())
        {
            cout << "Deleted file: " << _file.first << '\n';
            return false;
        }
    }

    return true;
}

static void 
    CheckAndUpdateFileSystem()
{
    auto f_CurrentPath = fs::current_path(); //idfk
    auto f_InitialPathState = GetCurrentDirectoryState(f_CurrentPath);

    while (true)
    {
        //this is a sleep for testing purposes, itll have an accumulator in the final implementation
        this_thread::sleep_for(chrono::seconds(1)); // Check once every second for any changes

        auto f_NewState = GetCurrentDirectoryState(f_CurrentPath);

        if (f_NewState != f_InitialPathState)
        {
            CompareStates(f_InitialPathState, f_NewState);
            f_InitialPathState = move(f_NewState);
        }
    }
}

static void 
    SetupInternalLogManagers()
{
    shared_ptr<PeachConsole> f_PeachConsole = make_shared<PeachConsole>();

    //probably should have better error handling for the loggers, especially
    InternalLogManager::InternalMainLogger().Initialize("../logs", "main_thread", f_PeachConsole);
    InternalLogManager::InternalAudioLogger().Initialize("../logs", "audio_thread", f_PeachConsole);
    InternalLogManager::InternalRenderingLogger().Initialize("../logs", "render_thread", f_PeachConsole);
    InternalLogManager::InternalResourceLoadingLogger().Initialize("../logs", "resource_thread", f_PeachConsole);

    InternalLogManager::InternalMainLogger().Debug("InternalMainLogger successfully initialized", "Peach-E");
    InternalLogManager::InternalAudioLogger().Debug("InternalAudioLogger successfully initialized", "Peach-E");
    InternalLogManager::InternalRenderingLogger().Debug("InternalRenderingLogger successfully initialized", "Peach-E");
    InternalLogManager::InternalResourceLoadingLogger().Debug("InternalResourceLoadingLogger successfully initialized", "Peach-E");
}

static void RenderThread()
{
    while (m_Running)
    {
        // Play audio
        cout << "Playing ur mom LOL...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void AudioThread()
{
    while (m_Running) 
    {
        // Play audio
        cout << "Playing audio...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void ResourceLoadingThread() 
{
    while (m_Running) 
    {
        // Load resources
        cout << "Loading resources...\n";
        this_thread::sleep_for(chrono::milliseconds(100)); // Simulate work
    }
}

static void NetworkThread() 
{
    while (m_Running) 
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
    ////////////////////////////////////////////////
    // Setup Loggers
    ////////////////////////////////////////////////

    PeachEngineManager::PeachEngine().SetupLogManagers();
    
    SetupInternalLogManagers();

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
        PeachEngineManager::PeachEngine().LoadPluginsFromConfigs(mf_ListOfWindowsPluginsToLoad); // Windows
    #else
        PeachEngineManager::PeachEngine().LoadPluginsFromConfigs(mf_ListOfUnixPluginsToLoad); // Linux/Unix
    #endif

    PeachEngineManager::PeachEngine().RunPlugins();

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

    auto peach_editor = &PeachEditorRenderingManager::PeachEngineRenderer();

    //Initialize methods, RenderingManager is special because we need two way communication, so RenderingManager issues one and only one copy of the commandqueue sharedptr for the main thread to use judiciously
    mf_PeachEditorRenderingManagersCommandQueue = peach_editor->InitializeQueues();
    mf_PeachEditorDrawableResourceLoadingQueue = PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue();

    if (not peach_editor->CreateSDLWindow("Peach Engine", mf_MainWindowWidth, mf_MainWindowHeight))
    {
        InternalLogManager::InternalMainLogger().Fatal("Was not able to create the main window, exiting execution immediately", "main_thread");
        return FAILED_TO_CREATE_MAIN_WINDOW;
    }

    InternalLogManager::InternalMainLogger().Debug("SDL window successfully created for Peach Editor", "main_thread");

    if (not peach_editor->InitializeOpenGL())
    {
        InternalLogManager::InternalMainLogger().Fatal("Was not able to initialize a valid OpenGL context, exiting execution immediately", "main_thread");
        return FAILED_TO_INITIALIZE_OPENGL;
    }

    InternalLogManager::InternalMainLogger().Debug("Peach Editor successfully initialized OpenGL", "main_thread");

    peach_editor->RenderFrame();

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

  
    InternalLogManager::InternalMainLogger().Debug("Exit Success!", "Peach-E");

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