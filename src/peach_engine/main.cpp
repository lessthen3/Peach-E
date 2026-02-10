/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#include <Managers/GameManager.h>

int 
    main(int fp_ArgCount, const char* fp_ArgVector[]) //This method kinda clean ngl lmfao
{
    try
    {
        auto engine_manager = &PeachCore::GameManager::get_single();

        std::string f_RootPath = std::string(fp_ArgVector[0]);
        size_t f_EnginePos = f_RootPath.rfind("Peach");

        f_RootPath = f_RootPath.substr(0, f_EnginePos);

        //WARNING: this should not be the custom init but for now it is for testing before peach editor can generate peach binaries dam thats cool i didnt fully realize what that meant until now thats how its supposed to work
        //the editor is a settings factory but i was thinking ab it in the hacky ad hoc get it there fashion but no that sthe final design goal LMFAO dam we gettin it done owo

        if (not engine_manager->InitializePeachEngineCustom(f_RootPath, PeachCore::ThreadName::ALL_THREADS ^ PeachCore::ThreadName::PhysicsThread, PeachCore::RendererType::Vulkan))
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
    catch (const std::exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PeachCore::PrintError(std::format("Unhandled exception: {}", Exception.what()));

        return EXIT_FAILURE;
    }
}
