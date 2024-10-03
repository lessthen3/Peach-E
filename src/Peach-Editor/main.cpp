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
#define NK_SDL_GL3_IMPLEMENTATION


#define NK_IMPLEMENTATION

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

    ////////////////////////////////////////////////
    // Setup Nuklear GUI
    ////////////////////////////////////////////////
    struct nk_context* f_NuklearCtx = nk_sdl_init(editor_renderer->GetMainWindow());

    struct nk_font_atlas* mf_FontAtlas;
    nk_sdl_font_stash_begin(&mf_FontAtlas);

    struct nk_font* mf_ComicSans = nk_font_atlas_add_from_file(mf_FontAtlas, "../fonts/ComicSansMS.ttf", 18, 0);

    nk_sdl_font_stash_end();

    if(mf_ComicSans)
    {
        nk_style_set_font(f_NuklearCtx, &(mf_ComicSans->handle));
    }

    //nk_init_fixed(&f_NuklearCtx, calloc(1, MAX_MEMORY), MAX_MEMORY, mf_ComicSans);
    //const size_t MAX_MEMORY = pow(2, 16);

    struct nk_colorf bg = { 0.10f, 0.18f, 0.24f, 1.0f };

    int f_CurrentAvailableMainWindowHeight = mf_MainWindowHeight;
    int f_CurrentAvailableMainWindowWidth = mf_MainWindowWidth;

    constexpr float MAIN_MENU_BAR_SCALE = 0.05f;
    constexpr unsigned int NUMBER_OF_HORIZONTAL_MAIN_MENU_BAR_ELEMENTS = 7;

    int f_MainMenuBarHeight = mf_MainWindowHeight * MAIN_MENU_BAR_SCALE;
    int f_MainMenuBarWidth = f_CurrentAvailableMainWindowWidth;

    while (not mf_IsProgramRuntimeOver)
    {
        this_thread::sleep_for(chrono::milliseconds(16));
        //editor_renderer->RenderFrame(&mf_IsProgramRuntimeOver);
        
        ////////////////////////////////////////////////
        // Input Handling
        ////////////////////////////////////////////////
        SDL_Event mf_Event;
        nk_input_begin(f_NuklearCtx);
        while (SDL_PollEvent(&mf_Event))
        {
            if (mf_Event.type == SDL_QUIT)
            {
                mf_IsProgramRuntimeOver = true;
            }
            nk_sdl_handle_event(&mf_Event);
        }
        nk_sdl_handle_grab();
        nk_input_end(f_NuklearCtx);

        ////////////////////////////////////////////////
        // Draw GUI
        ////////////////////////////////////////////////
        if (nk_begin(f_NuklearCtx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            enum { EASY, HARD };
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(f_NuklearCtx, 30, 80, 1);
            if (nk_button_label(f_NuklearCtx, "button"))
                printf("button pressed!\n");
            nk_layout_row_dynamic(f_NuklearCtx, 30, 2);
            if (nk_option_label(f_NuklearCtx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(f_NuklearCtx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(f_NuklearCtx, 22, 1);
            nk_property_int(f_NuklearCtx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(f_NuklearCtx, 20, 1);
            nk_label(f_NuklearCtx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(f_NuklearCtx, 25, 1);
            if (nk_combo_begin_color(f_NuklearCtx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(f_NuklearCtx), 400))) {
                nk_layout_row_dynamic(f_NuklearCtx, 120, 1);
                bg = nk_color_picker(f_NuklearCtx, bg, NK_RGBA);
                nk_layout_row_dynamic(f_NuklearCtx, 25, 1);
                bg.r = nk_propertyf(f_NuklearCtx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(f_NuklearCtx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(f_NuklearCtx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(f_NuklearCtx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(f_NuklearCtx);
            }
        }
        nk_end(f_NuklearCtx);

        ////////////////////////////////////////////////
        // Menubar
        ////////////////////////////////////////////////

        SDL_GL_GetDrawableSize(editor_renderer->GetMainWindow(), &f_CurrentAvailableMainWindowWidth, &f_CurrentAvailableMainWindowHeight); //used for correcting to window size

        f_MainMenuBarHeight = f_CurrentAvailableMainWindowHeight * MAIN_MENU_BAR_SCALE;
        f_CurrentAvailableMainWindowHeight -= f_MainMenuBarHeight; //subtract menu bar since we want the available 

        if (nk_begin(f_NuklearCtx, "Menu_Bar", nk_rect(0, 0, f_CurrentAvailableMainWindowWidth, f_MainMenuBarHeight), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR))
        {
            nk_menubar_begin(f_NuklearCtx);

            // Menu label, must match with nk_menu_begin_label call
            nk_layout_row_begin(f_NuklearCtx, NK_STATIC, 25, NUMBER_OF_HORIZONTAL_MAIN_MENU_BAR_ELEMENTS);
            nk_layout_row_push(f_NuklearCtx, 45);

            if (nk_menu_begin_label(f_NuklearCtx, "File", NK_TEXT_LEFT, nk_vec2(300, 200)))
            {
                nk_layout_row_dynamic(f_NuklearCtx, 30, 1);

                if (nk_menu_item_label(f_NuklearCtx, "Open Project", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Open menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Save Everything", NK_TEXT_LEFT))
                {
                    // Handle save action
                    cout << ("Save menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Save as", NK_TEXT_LEFT))
                {
                    // Handle save action
                    cout << ("Save as menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "New Project", NK_TEXT_LEFT))
                {
                    cout << ("New Project menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Exit", NK_TEXT_LEFT))
                {
                    mf_IsProgramRuntimeOver = true;
                }

                nk_menu_end(f_NuklearCtx);
            }
            if (nk_menu_begin_label(f_NuklearCtx, "Edit", NK_TEXT_LEFT, nk_vec2(300, 200)))
            {
                nk_layout_row_dynamic(f_NuklearCtx, 30, 1);

                if (nk_menu_item_label(f_NuklearCtx, "Undo", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Undo menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Redo", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Redo menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Show Action History", NK_TEXT_LEFT))
                {
                    // Handle save action
                    cout << ("Show Action History menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Show Clipboard History", NK_TEXT_LEFT))
                {
                    cout << ("Show Clipboard History menu item clicked\n");
                }

                nk_menu_end(f_NuklearCtx);
            }
            if (nk_menu_begin_label(f_NuklearCtx, "Run", NK_TEXT_LEFT, nk_vec2(300, 200)))
            {
                nk_layout_row_dynamic(f_NuklearCtx, 30, 1);

                if (nk_menu_item_label(f_NuklearCtx, "Run Peach-E Project", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Run Peach-E Project menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Force Quit Peach-E Project", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Force Quit Peach-E Project menu item clicked\n");
                }

                nk_menu_end(f_NuklearCtx);
            }

            nk_layout_row_push(f_NuklearCtx, 60); //adjust the spacing ratio for the elements, since these are longer it looks a bit weirder

            if (nk_menu_begin_label(f_NuklearCtx, "Project", NK_TEXT_LEFT, nk_vec2(300, 200)))
            {
                nk_layout_row_dynamic(f_NuklearCtx, 30, 1);

                if (nk_menu_item_label(f_NuklearCtx, "Settings", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Settings menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Input Map", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Input Map menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Refresh Project", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Refresh Project menu item clicked\n");
                }

                nk_menu_end(f_NuklearCtx);
            }
            if (nk_menu_begin_label(f_NuklearCtx, "Editor", NK_TEXT_LEFT, nk_vec2(300, 200)))
            {
                nk_layout_row_dynamic(f_NuklearCtx, 30, 1);

                if (nk_menu_item_label(f_NuklearCtx, "Settings", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Settings menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Input Map", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Input Map menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Refresh Project", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Refresh Project menu item clicked\n");
                }

                nk_menu_end(f_NuklearCtx);
            }
            if (nk_menu_begin_label(f_NuklearCtx, "Tools", NK_TEXT_LEFT, nk_vec2(300, 200)))
            {
                nk_layout_row_dynamic(f_NuklearCtx, 30, 1);

                if (nk_menu_item_label(f_NuklearCtx, "Open Terminal", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Open Terminal menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "Run Stress Test", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Run Stress Test menu item clicked\n");
                }

                nk_menu_end(f_NuklearCtx);
            }
            if (nk_menu_begin_label(f_NuklearCtx, "Plugins", NK_TEXT_LEFT, nk_vec2(300, 200)))
            {
                nk_layout_row_dynamic(f_NuklearCtx, 30, 1);

                if (nk_menu_item_label(f_NuklearCtx, "Load Plugin", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Load Plugin menu item clicked\n");
                }
                if (nk_menu_item_label(f_NuklearCtx, "List of Active Plugins", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("List of Active Plugins menu item clicked\n");
                }

                nk_menu_end(f_NuklearCtx);
            }

            nk_layout_row_end(f_NuklearCtx);
            nk_menubar_end(f_NuklearCtx);
        }
        nk_end(f_NuklearCtx);

        ////////////////////////////////////////////////
        // Clear and Render
        ////////////////////////////////////////////////

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.10f, 0.18f, 0.24f, 1.0f);

        nk_sdl_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        SDL_GL_SwapWindow(editor_renderer->GetMainWindow());
    }

    editor_renderer->Shutdown();

    SDL_Quit(); //just makes more sense to have the main method do this
  
    main_logger->Debug("Exit Success!", "Peach-E");

    return EXIT_SUCCESS;
}








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