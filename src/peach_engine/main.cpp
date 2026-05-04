/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#include <managers/GameManager.h>
#include <debug/PeachLauncher.h>

static int
    RunPeachEngine(int, const char** fp_ArgVector)
{
    int* crash = nullptr;

    *crash = 69;

    try
    {
        auto engine_manager = &PeachCore::GameManager::get_single();

        std::string f_RootPath = std::string(fp_ArgVector[0]);
        size_t f_EnginePos = f_RootPath.rfind("Peach");

        f_RootPath = f_RootPath.substr(0, f_EnginePos);

        //WARNING: this should not be the custom init but for now it is for testing before peach editor can generate peach binaries dam thats cool i didnt fully realize what that meant until now thats how its supposed to work
        //the editor is a settings factory but i was thinking ab it in the hacky ad hoc get it there fashion but no that sthe final design goal LMFAO dam we gettin it done owo

#ifdef PEACH_PLATFORM_APPLE
        auto f_RenderingBackend = PeachCore::RendererType::Metal; //TESTING: apple only supports metal but the other plats need a default renderer and meta data to save the backend set by usr
#else
        auto f_RenderingBackend = PeachCore::RendererType::Vulkan;
#endif

        if 
        (
            not engine_manager->InitializePeachEngineCustom
            (
                f_RootPath, 
                PeachCore::ThreadName::ALL_THREADS ^ PeachCore::ThreadName::PhysicsThread, 
                f_RenderingBackend,
                800,
                600
            )
        )
        {

            return PEACH_ERROR_FAILED_TO_INITIALIZE;
        }

        engine_manager->StartMainGameLoop(); //should always work and only throw on exception uwu, or segfault onto the handler owo

        if (not engine_manager->ShutdownPeachEngine())
        {

            return PEACH_ERROR_FAILED_TO_SHUTDOWN_PROPERLY;
        }

        return EXIT_SUCCESS;
    }
    catch (const std::exception& cv_Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PEACH_PRINT_ERROR_FMT("Unhandled exception: {}", cv_Exception.what());

        return EXIT_FAILURE;
    }
}



int 
    main(int fp_ArgCount, const char** fp_ArgVector) //This method kinda clean ngl lmfao
{
    // Detect if we are the child or being debugged
    bool f_RunAsEngine = false;

    for (int lv_Index = 0; lv_Index < fp_ArgCount; ++lv_Index) 
    {
        if (std::string(fp_ArgVector[lv_Index]) == "--engine-child") 
        {
            f_RunAsEngine = true;
            break;
        }
    }

    // 2. The Logic Switch
    if (PeachCore::IsDebuggerAttached() || f_RunAsEngine) // THIS IS THE ENGINE MODE
    {
        return RunPeachEngine(fp_ArgCount, fp_ArgVector); 
    }
    else // THIS IS THE WATCHDOG MODE, this process stays light, consumes almost no RAM, and just waits for the Engine to finish or explode.
    {
        return PeachCore::LauncherMain( 1, fp_ArgVector);
    }
}
