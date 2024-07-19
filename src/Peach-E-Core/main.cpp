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

#include "../Peach-core/include/Managers/PeachEngineManager.h"
#include "../Princess/include/Parsers/PythonScriptParser.h"

using namespace PeachCore;
using namespace std;

atomic<bool> m_Running(true);

//used for pushing update commands to the Render Thread
shared_ptr<CommandQueue> m_RenderingManagersCommandQueue; //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out

shared_ptr<LoadingQueue> m_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager
shared_ptr<LoadingQueue> m_DrawableResourceLoadingQueue; //used to push load commands that are destined for RenderingManager

// ObjectID : SceneTreeItem : Associated Update Package, used for updating all relevant data at the same time
//map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllCurrentlyActivePeachNodes;
//map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllPeachNodesQueuedForRemoval;

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

static void IssueLoadingCommands(vector<LoadCommand> fp_ListOfLoadCommands)
{

}

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int main(int argc, char* argv[])
{
    PeachEngineManager::PeachEngine().SetupLogManagers();

    int f_WindowWidth = 800;
    int f_WindowHeight = 600;

    //////////////////////////////////////////////
    // Setup Renderer for 2D
    //////////////////////////////////////////////

    try 
    {
        //Initialize methods, RenderingManager is special because we need two way communication, so RenderingManager issues one and only one copy of the commandqueue sharedptr 
        //for the main thread to use judiciously
        m_RenderingManagersCommandQueue = RenderingManager::Renderer().Initialize("Peach Engine", f_WindowWidth, f_WindowHeight);

        if (!AudioManager::AudioPlayer().Initialize())
        {
            //do something idk
        }
        //maybe do a ResourceLoadingManager initialize if needed, idk feels weird that we dont have one yet and it just works lmao, i guess its the lazy initialization
        m_DrawableResourceLoadingQueue = ResourceLoadingManager::ResourceLoader().GetDrawableResourceLoadingQueue();
        m_AudioResourceLoadingQueue = ResourceLoadingManager::ResourceLoader().GetAudioResourceLoadingQueue();
    }
    catch (const exception& ex) 
    {
        cerr << "An error occurred: " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    //IssueLoadingCommands();

    //////////////////// Camera Setup ////////////////////
    //NEED TO STILL IMPLEMENT
    //Camera2D camera(f_WindowWidth, f_WindowHeight);
    
    ////////////////////// Texture Setup ////////////////////
    //Texture2D m_FirstTexture = Texture2D(">w<", "D:/Game Development/Random Junk I Like to Keep/Texture-Tests/owo.jpg");
    ////m_FirstTexture = Texture2D("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/spooktacular.png");


    

    ResourceLoadingManager::ResourceLoader().LoadTexture("D:/Game Development/Random Junk I Like to Keep/Texture-Tests/owo.jpg");

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
    //ThreadPoolManager::ThreadPool().Shutdown
    //Princess::PythonScriptParser::Parser().ExtractFunctionInformationFromPythonModule("Test-Function-Read");




    ////////////////////////////////// Plugins ///////////////////////////////////////////

    vector<string> ListOfWindowsPluginsToLoad = { "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin.dll",
                                                                                               "D:/Game Development/Peach-E/src/Peach-E-Core/plugins/SimplePlugin2.dll" };
    vector<string> ListOfUnixPluginsToLoad = { };

    #if defined(_WIN32) || defined(_WIN64)
        PeachEngineManager::PeachEngine().LoadPluginsFromConfigs(ListOfWindowsPluginsToLoad); // Windows
    #else
        PeachEngineManager::PeachEngine().LoadPluginsFromConfigs(ListOfUnixPluginsToLoad); // Linux/Unix
    #endif

    PeachEngineManager::PeachEngine().RunPlugins();
  
    LogManager::MainLogger().Debug("Exit Success!", "Peach-E");

    return EXIT_SUCCESS;
}