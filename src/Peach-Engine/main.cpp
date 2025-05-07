/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#define SDL_MAIN_HANDLED

#define NK_SDL3_GL3_IMPLEMENTATION
#define NK_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION

#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#define MINIAUDIO_IMPLEMENTATION

#include "../../include/Peach-Core/Managers/GameManager.h"

#include <csignal>

static void
    SegFaultHandler(int fp_Signal)
{
    PeachCore::PrintError(format("[!] Crash signal received: {}", fp_Signal));
    // possibly notify watchdog or dump stack trace
    exit(EXIT_FAILURE);
}

int 
    main(int fp_ArgCount, const char* fp_ArgVector[]) //This method kinda clean ngl lmfao
{
    cout << "Hello World!\n";

    signal(SIGSEGV, SegFaultHandler); //XXX: used for trying to close and flush logs on seg fault

    try
    {
        auto engine_manager = &PeachCore::GameManager::PeachEngine();

        engine_manager->InitializePeachEngine(string(fp_ArgVector[0]), PeachCore::RendererType::OpenGL);
        engine_manager->StartMainGameLoop();
        engine_manager->ShutdownPeachEngine();

        return EXIT_SUCCESS;
    }

    catch (const std::exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PeachCore::PrintError(format("Unhandled exception: {}", Exception.what()));

        return EXIT_FAILURE;
    }
}