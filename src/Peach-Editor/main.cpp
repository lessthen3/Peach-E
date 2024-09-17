/*
    Copyright(c) 2024-present Ranyodh Singh Mandur.
*/

#define SDL_MAIN_HANDLED

#include <pybind11/pybind11.h>

#include "../../include/Peach-Editor/Managers/PeachEditorRenderingManager.h"
#include "../../include/Peach-Engine/PeachEngineManager.h"

#include <filesystem>

using namespace std;
using namespace PeachEditor;
using namespace PeachEngine;

constexpr unsigned int FAILED_TO_CREATE_MAIN_WINDOW = -1000;
constexpr unsigned int FAILED_TO_INITIALIZE_OPENGL = -1001;

static atomic<bool> m_Running(true);

// Function to list all files recursively
static unordered_map<string, filesystem::file_time_type> 
    GetCurrentDirectoryState(const filesystem::path& path) 
{
    unordered_map<string, filesystem::file_time_type> files;

    try 
    {
        for (const auto& entry : filesystem::recursive_directory_iterator(path)) 
        {
            if (filesystem::is_regular_file(entry.status()) || filesystem::is_directory(entry.status()))
            {
                files[entry.path().string()] = filesystem::last_write_time(entry);
            }
        }
    }
    catch (const filesystem::filesystem_error& e) 
    {
        cerr << "Error: " << e.what() << '\n';
    }

    return files;
}

// Function to compare two filesystem states, returns true if file_system_1 == file_system_2, returns false otherwise
static bool 
    CompareStates(const unordered_map<string, filesystem::file_time_type>& old_state, const unordered_map<string, filesystem::file_time_type>& new_state) 
{
    for (const auto& file : new_state) 
    {
        auto it = old_state.find(file.first);

        if (it == old_state.end())
        {
            cout << "New file: " << file.first << '\n';
            return false;
        }
        else if (it->second != file.second)
        {
            cout << "Modified file: " << file.first << '\n';
            return false;
        }
    }

    for (const auto& file : old_state)
    {
        if (new_state.find(file.first) == new_state.end())
        {
            cout << "Deleted file: " << file.first << '\n';
            return false;
        }
    }

    return true;
}

static void 
    CheckAndUpdateFileSystem()
{
    auto f_CurrentPath = filesystem::current_path(); //idfk
    auto f_InitialPathState = GetCurrentDirectoryState(f_CurrentPath);

    while (true)
    {
        this_thread::sleep_for(chrono::seconds(1)); // Check once every second for any changes

        auto new_state = GetCurrentDirectoryState(f_CurrentPath);

        if (new_state != f_InitialPathState)
        {
            CompareStates(f_InitialPathState, new_state);
            f_InitialPathState = move(new_state);
        }
    }
}

static void 
    SetupInternalLogManagers()
{
    InternalLogManager::InternalMainLogger().Initialize("../logs", "InternalMainLogger");
    InternalLogManager::InternalAudioLogger().Initialize("../logs", "InternalAudioLogger");
    InternalLogManager::InternalRenderingLogger().Initialize("../logs", "InternalRenderingLogger");
    InternalLogManager::InternalResourceLoadingLogger().Initialize("../logs", "InternalResourceLoadingLogger");

    InternalLogManager::InternalMainLogger().Debug("InternalMainLogger successfully initialized", "Peach-E");
    InternalLogManager::InternalAudioLogger().Debug("InternalAudioLogger successfully initialized", "Peach-E");
    InternalLogManager::InternalRenderingLogger().Debug("InternalRenderingLogger successfully initialized", "Peach-E");
    InternalLogManager::InternalResourceLoadingLogger().Debug("InternalResourceLoadingLogger successfully initialized", "Peach-E");
}


void test()
{

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


//////////////////////////////////////////////
// Setting Up Renderer For Engine
//////////////////////////////////////////////
//ARTIFACT FOR MUSEUM VIEWING ONLY
static void SetupRenderer()
{
    return;
    //RenderingManager::Renderer().Initialize("OpenGL");

   //sf::RenderWindow& m_CurrentRenderWindow = RenderingManager::Renderer().GetRenderWindow();

   // // Set global settings
   //RenderingManager::Renderer().SetFrameRateLimit(120);
   //RenderingManager::Renderer().SetVSync(true);

   //string f_PathToFontFile = "D:/Game Development/Fonts I guess/Comic Sans MS";

   // sf::Font font;
   // if (!font.loadFromFile(f_PathToFontFile)) {
   //     LogManager::Logger().Warn("Load Error: Unable to load font file from specified path: " + f_PathToFontFile, "Peach-E");
   // }
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
    // Setup Renderer for 2D
    ////////////////////////////////////////////////

    const unsigned int mf_MainWindowWidth = 800;
    const unsigned int mf_MainWindowHeight = 600;

    //Initialize methods, RenderingManager is special because we need two way communication, so RenderingManager issues one and only one copy of the commandqueue sharedptr for the main thread to use judiciously
    mf_PeachEditorRenderingManagersCommandQueue = PeachEditorRenderingManager::PeachEngineRenderer().InitializeQueues();
    mf_PeachEditorDrawableResourceLoadingQueue = PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue();

    if (not PeachEditorRenderingManager::PeachEngineRenderer().CreateSDLWindow("Peach Engine", mf_MainWindowWidth, mf_MainWindowHeight))
    {
        InternalLogManager::InternalMainLogger().Fatal("Was not able to create the main window, exiting execution immediately", "main_thread");
        return FAILED_TO_CREATE_MAIN_WINDOW;
    }

    InternalLogManager::InternalMainLogger().Debug("SDL window successfully created for Peach Editor", "main_thread");

    if (not PeachEditorRenderingManager::PeachEngineRenderer().InitializeOpenGL())
    {
        InternalLogManager::InternalMainLogger().Fatal("Was not able to initialize a valid OpenGL context, exiting execution immediately", "main_thread");
        return FAILED_TO_INITIALIZE_OPENGL;
    }

    InternalLogManager::InternalMainLogger().Debug("Peach Editor successfully initialized OpenGL", "main_thread");

    PeachEditorRenderingManager::PeachEngineRenderer().RenderFrame();

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
