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

#define NK_SDL3_GL3_IMPLEMENTATION
#define NK_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION

#include "../../include/Peach-Editor/Managers/PeachEditorManager.h"

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int 
    main(int fp_ArgCount, const char* fp_ArgVector[])
{
    cout << fp_ArgVector[0] << "\n"; //COOL AF

    //WARNING: WE ONLY USE THIS FOR DEVELOPMENT, FOR DEPLOYMENT WE NEED THIS DIRECTORY TO BE THE BASE DIR OF THE EXECUTABLE
    // Get the full path of the executable
    filesystem::path mf_ExePath = filesystem::absolute(fp_ArgVector[0]);
    filesystem::path mf_TopLevelDir = mf_ExePath.parent_path();  // Start from the executable directory

    // Traverse upwards until we find the "Peach-E" directory
    while (not mf_TopLevelDir.empty() && mf_TopLevelDir.filename() != "Peach-E")
    {
        mf_TopLevelDir = mf_TopLevelDir.parent_path();
    }

    if (mf_TopLevelDir.empty())
    {
        PC::Print("Failed to find the top-level directory 'Peach-E'!", PeachCore::Colours::Magenta);
        return false;
    }

    string mf_PeachERootPath = mf_TopLevelDir.string();

    ////////////////////////////////////////////////
    // Setup Environment
    ////////////////////////////////////////////////

    auto peach_editor = &PeachEditor::PeachEditorManager::PeachEditor();
    auto peach_engine = &PeachEngine::GameManager::PeachEngine();

    auto editor_renderer = &PeachEditor::PeachEditorRenderingManager::PeachEditorRenderer();
    auto engine_renderer = &PC::RenderingManager::Renderer();

    auto throw_away = engine_renderer->InitializeQueues();

    peach_engine->InitializePeachEngine
    (
        mf_PeachERootPath,
        PeachCore::RendererType::OpenGL
    );

    peach_editor->InitializePeachEditor
    (
        mf_PeachERootPath
    );

    ////////////////////////////////////////////////
    // Setup Communication Queues
    ////////////////////////////////////////////////

    //used for pushing update commands to the Render Thread
    //Initialize methods, so RenderingManager issues one and only one copy of the commandqueue sharedptr for the main thread to use judiciously
    shared_ptr<PeachCore::CommandQueue> mf_PeachEditorRenderingManagersCommandQueue = editor_renderer->InitializeQueues(); //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out
    shared_ptr<PeachCore::LoadingQueue> mf_PeachEditorDrawableResourceLoadingQueue = PeachEditor::PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue(); //used to push load commands that are destined for RenderingManager

    shared_ptr<PeachCore::CommandQueue> mf_AudioManagersCommandQueue; //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out
    shared_ptr<PeachCore::LoadingQueue> mf_AudioResourceLoadingQueue; //used to push load commands that are destined for AudioManager

    // ObjectID : SceneTreeItem : Associated Update Package, used for updating all relevant data at the same time
    //map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllCurrentlyActivePeachNodes;
    //map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllPeachNodesQueuedForRemoval;

    peach_editor->StartPeachEditorMainLoop();

    return EXIT_SUCCESS;
}
