/*******************************************************************
 *                                             Peach-E v0.1
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#define SDL_MAIN_HANDLED

#define NK_SDL3_GL3_IMPLEMENTATION
#define NK_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION

#include "../../include/Peach-Engine/GameManager.h"

int 
    main(int fp_ArgCount, const char* fp_ArgVector[]) //This method kinda clean ngl lmfao
{
    cout << "Hello World!\n";
    
    auto engine_manager = &PeachEngine::GameManager::PeachEngine();

    engine_manager->InitializePeachEngine(string(fp_ArgVector[0]), PeachCore::RendererType::OpenGL);
    engine_manager->StartMainGameLoop();
    engine_manager->ShutdownPeachEngine();

    return EXIT_SUCCESS;
}