/*
    Copyright(c) 2024-present Ranyodh Singh Mandur.
*/

//#include <SFML/Graphics.hpp>
//#include <imgui.h>
//#include "../Peach-core/SFML-Imgui/imgui-SFML.h"

#include <pybind11/pybind11.h>

//#include "include/Managers/PeachEngine.h"
#include "include/Managers/PeachEditorRenderingManager.h"
#include "../Peach-core/include/Managers/PeachEngineManager.h"
#include "../Princess/include/Parsers/PythonScriptParser.h"


using namespace PeachEditor;

atomic<bool> m_Running(true);

//used for pushing update commands to the Render Thread
shared_ptr<CommandQueue> m_RenderingManagersCommandQueue; //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out

shared_ptr<LoadingQueue> m_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager
shared_ptr<LoadingQueue> m_DrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

// ObjectID : SceneTreeItem : Associated Update Package, used for updating all relevant data at the same time
//map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllCurrentlyActivePeachNodes;
//map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllPeachNodesQueuedForRemoval;

#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

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

    InternalLogManager::InternalMainLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "InternalMainLogger");
    InternalLogManager::InternalAudioLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "InternalAudioLogger");
    InternalLogManager::InternalRenderingLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "InternalRenderingLogger");
    InternalLogManager::InternalResourceLoadingLogger().Initialize("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE", "InternalResourceLoadingLogger");

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

unique_ptr<sf::RenderWindow> pm_CurrentWindow = nullptr;
unique_ptr<sf::View> pm_View = nullptr;

static void RunGameInstance()
{
    //////////////////////////////////////////////
    // Setup Window and Camera
    //////////////////////////////////////////////

    if (pm_CurrentWindow || pm_View)
    {
        LogManager::RenderingLogger().Warn("RenderingManager already has a valid window or camera2D currently active", "RenderingManager");
        throw runtime_error("OH NO ");
    }

    // Create an SFML window and "camera"
    pm_CurrentWindow = make_unique<sf::RenderWindow>(sf::VideoMode(800, 600), "Peach Game", sf::Style::Default);
    pm_View = make_unique<sf::View>();

    // Initialize the view to the size of the window
    pm_View->setSize(pm_CurrentWindow->getSize().x, pm_CurrentWindow->getSize().y);
    pm_View->setCenter(pm_CurrentWindow->getSize().x / 2, pm_CurrentWindow->getSize().y / 2);

    pm_CurrentWindow->setView(*pm_View);

    if (!pm_CurrentWindow->isOpen())
    {
        throw runtime_error("Failed to create window.");
    }

    //////////////////////////////////////////////
    // Rendering Loop
    //////////////////////////////////////////////

    bool fp_IsStressTest = true;
    // Main loop that continues until the window is closed
    while (pm_CurrentWindow->isOpen())
    {
        // Process events
        sf::Event event;

        while (pm_CurrentWindow->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                pm_CurrentWindow->close();
            }
        }

        // Clear the screen with a dark blue color
        pm_CurrentWindow->clear(sf::Color(0, 0, 139));

        // Update the window
        pm_CurrentWindow->display();

        if (fp_IsStressTest) //used to stop rendering loop after one cycle for testing
        {
            break;
        }
    }

    //////////////////////////////////////////////
    // Shutdown and Cleanup
    //////////////////////////////////////////////
    
    if (pm_View)
    {
        pm_View.reset(nullptr);
    }
    if (pm_CurrentWindow)
    {
        pm_CurrentWindow->clear();
        pm_CurrentWindow->close();

        pm_CurrentWindow.reset(nullptr);
    }
}

/////////////////////////////////////////////////////////////
// Run the actual test starting up and shutting down the windown
/////////////////////////////////////////////////////////////

static void StessTest(int numIterations, int delayMs)
{
    for (int i = 0; i < numIterations; ++i)
    {
        cout << "Iteration: " << i + 1 << endl;

        // Start the game instance in a thread
        thread gameThread(RunGameInstance);

        // Allow the thread some time to start and initialize resources
        this_thread::sleep_for(chrono::milliseconds(delayMs));

        // Wait for the thread to finish execution
        gameThread.join();

        // Allow some time for resources to be fully released
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
}

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int main(int argc, char* argv[])
{
    /*PeachEngineManager::PeachEngine().SetupLogManagers();
    
    SetupInternalLogManagers();

    int f_WindowWidth = 800;
    int f_WindowHeight = 600;*/

    ////////////////////////////////////////////////
    //// Setup Renderer for 2D
    ////////////////////////////////////////////////

    try 
    {
        //Initialize methods, RenderingManager is special because we need two way communication, so RenderingManager issues one and only one copy of the commandqueue sharedptr for the main thread to use judiciously
            //m_RenderingManagersCommandQueue = PeachEngineRenderingManager::PeachEngineRenderer().Initialize("Peach Engine", f_WindowWidth, f_WindowHeight);

    //    if (!AudioManager::AudioPlayer().Initialize())
    //    {
    //        //do something idk
    //    }
        StessTest(1000, 50);
        //maybe do a ResourceLoadingManager initialize if needed, idk feels weird that we dont have one yet and it just works lmao, i guess its the lazy initialization
            //m_DrawableResourceLoadingQueue = PeachEngineResourceLoadingManager::PeachEngineResourceLoader().GetDrawableResourceLoadingQueue();
    //    m_AudioResourceLoadingQueue = ResourceLoadingManager::ResourceLoader().GetAudioResourceLoadingQueue();


        ////////////////////// Texture Setup ////////////////////
    //PeachEngineResourceLoadingManager::PeachEngineResourceLoader().LoadTextureFromSpecifiedFilePath("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/uwu.png");
        /*                PeachEngineRenderingManager::PeachEngineRenderer().RenderFrame();
                        PeachEngineRenderingManager::PeachEngineRenderer().Shutdown();*/
    }
    catch (const exception& ex) 
    {
        cerr << "An error occurred: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
    /*thread T_Audio(AudioThread);
    thread T_ResourceLoading(ResourceLoadingThread);
    thread T_Network(NetworkThread);*/
    
   /* T_Audio.join();
    T_ResourceLoading.join();
    T_Network.join();*/

    // Clean up
    //ThreadPoolManager::ThreadPool().Shutdown
    //Princess::PythonScriptParser::Parser().ExtractFunctionInformationFromPythonModule("Test-Function-Read");

    ////////////////////////////////// Plugins ///////////////////////////////////////////
    //DLL's
    //vector<string> ListOfWindowsPluginsToLoad = { "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin.dll",
    //                                                                                           "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin2.dll" };
    ////SO's
    //vector<string> ListOfUnixPluginsToLoad = { };

    //#if defined(_WIN32) || defined(_WIN64)
    //    PeachEngineManager::PeachEngine().LoadPluginsFromConfigs(ListOfWindowsPluginsToLoad); // Windows
    //#else
    //    PeachEngineManager::PeachEngine().LoadPluginsFromConfigs(ListOfUnixPluginsToLoad); // Linux/Unix
    //#endif

    //PeachEngineManager::PeachEngine().RunPlugins();
  
    //LogManager::MainLogger().Debug("Exit Success!", "Peach-E");

    unique_ptr<sf::RenderWindow> f_MemoryCheckWindow = make_unique<sf::RenderWindow>(sf::VideoMode(800, 600), "Peach Game", sf::Style::Default);

    while (f_MemoryCheckWindow->isOpen())
    {
        // Process events
        sf::Event event;

        while (f_MemoryCheckWindow->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                f_MemoryCheckWindow->close();
            }
        }

        // Clear the screen with a dark blue color
        f_MemoryCheckWindow->clear(sf::Color(0, 0, 139));

        // Update the window
        f_MemoryCheckWindow->display();

    }

    f_MemoryCheckWindow.reset(nullptr);

    return EXIT_SUCCESS;
}
