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
#include <SDL3/SDL_main.h>

#define NK_SDL_GL3_IMPLEMENTATION
#define NK_IMPLEMENTATION


#include <pybind11/pybind11.h>

#include "../../include/Peach-Editor/Managers/PeachEditorRenderingManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../include/Peach-Core/General/stb/stb_image.h"

using namespace std;
using namespace PeachEditor;
using namespace PeachEngine;

constexpr int FAILED_TO_CREATE_MAIN_WINDOW = -1000;
constexpr int FAILED_TO_INITIALIZE_OPENGL = -1001;

#if defined(_WIN32) || defined(_WIN64)
    static bool
        EnableColors()
    {
        DWORD f_ConsoleMode;
        HANDLE f_OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        if (GetConsoleMode(f_OutputHandle, &f_ConsoleMode))
        {
            SetConsoleMode(f_OutputHandle, f_ConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            return true;
        }
        else
        {
            PeachCore::LogManager::MainLogger().LogAndPrint("Was not able to set console mode to allow windows to display ANSI escape codes", "main_thread", "error");
            return false;
        }
    }
#endif

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int main(int fp_ArgCount, char* fp_ArgVector[])
{
    cout << fp_ArgVector[0] << "\n"; //COOL AF

    //Enable ANSI colour codes for windows console grumble grumble
    #if defined(_WIN32) || defined(_WIN64)
        EnableColors();
    #endif

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
        "..\\plugins\\SimplePlugin.dll",
        "..\\plugins\\SimplePlugin2.dll"
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

    if (not editor_renderer->CreateMainSDLWindow("Peach Engine", mf_MainWindowWidth, mf_MainWindowHeight))
    {
        main_logger->LogAndPrint("Was not able to create the main window, exiting execution immediately", "main_thread", "fatal");
        return FAILED_TO_CREATE_MAIN_WINDOW;
    }

    main_logger->LogAndPrint("SDL window successfully created for Peach Editor", "main_thread", "debug");

    if (not editor_renderer->InitializeOpenGL())
    {
        main_logger->LogAndPrint("Was not able to initialize a valid OpenGL context, exiting execution immediately", "main_thread", "fatal");
        return FAILED_TO_INITIALIZE_OPENGL;
    }

    main_logger->LogAndPrint("Peach Editor successfully initialized OpenGL", "main_thread", "debug");

    PeachCore::RenderingManager::Renderer().InitializeQueues();

    bool mf_IsEditorOpen = true;

    vector<float> vertices =
    {
        // positions             // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
    };

    vector<unsigned int> indices =
    {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    auto peach_renderer = editor_renderer->GetPeachRenderer();


    while(mf_IsEditorOpen)
    {
        this_thread::sleep_for(chrono::milliseconds(16)); //60 fps oh i just realized the fps flickers by 1 because the floating point conversion isnt exact
        editor_renderer->RenderFrame(&mf_IsEditorOpen);
        //SDL_GL_SwapWindow(peach_renderer->GetMainWindow());

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearColor(0.10f, 0.18f, 0.24f, 1.0f);
    }

    editor_renderer->Shutdown();

    SDL_Quit(); //just makes more sense to have the main method do this
  
    main_logger->LogAndPrint("Exit Success!", "Peach-E", "debug");

    return EXIT_SUCCESS;
}
