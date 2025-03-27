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
#include <SDL3/SDL_main.h>

#include "../../include/Peach-Engine/GameManager.h"

#include <iostream>
#include <string>

#include <thread>

using namespace std;

int 
    main(int fp_ArgCount, const char* fp_ArgVector[]) //This method kinda clean ngl lmfao
{
    cout << "Hello World!\n";
    
    auto engine_manager = &PeachEngine::GameManager::PeachEngine();

    vector<string> dummy_vector = {}; //used for now because idk if plugin paths should be specified in IntializePeachEngine()

    engine_manager->InitializePeachEngine(string(fp_ArgVector[0]), dummy_vector, PeachCore::RendererType::OpenGL);
    engine_manager->StartMainGameLoop();
    engine_manager->ShutdownPeachEngine();

    return EXIT_SUCCESS;
}