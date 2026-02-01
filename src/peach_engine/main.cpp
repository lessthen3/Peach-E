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

        engine_manager->InitializePeachEngine(f_RootPath, PeachCore::ThreadName::ALL_THREADS ^ PeachCore::ThreadName::PhysicsThread, PeachCore::RendererType::Vulkan);
        engine_manager->StartMainGameLoop();
        engine_manager->ShutdownPeachEngine();

        return EXIT_SUCCESS;
    }
    catch (const std::exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PeachCore::PrintError(std::format("Unhandled exception: {}", Exception.what()));

        return EXIT_FAILURE;
    }
}
