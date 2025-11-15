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
#include "Managers/GameManager.h"

int 
    main(int fp_ArgCount, const char* fp_ArgVector[]) //This method kinda clean ngl lmfao
{
    std::cout << "Hello World!\n"; //Used this to test the projects first start up uwu

    try
    {
        auto engine_manager = &PeachCore::GameManager::get_single();

//        engine_manager->InitializePeachEngine(string(fp_ArgVector[0]), PeachCore::RendererType::OpenGL);
//        engine_manager->StartMainGameLoop();
//        engine_manager->ShutdownPeachEngine();

        return EXIT_SUCCESS;
    }

    catch (const std::exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PeachCore::PrintError(std::format("Unhandled exception: {}", Exception.what()));

        return EXIT_FAILURE;
    }
}
