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
#ifdef PEACH_PLATFORM_WASM

#include "managers/GameManager.h"
#include <SDL3/SDL_main.h>
#include <emscripten/emscripten.h>

int 
    main(int fp_ArgCount, char** fp_ArgVector)
{
    auto& f_Engine = PeachCore::GameManager::get_single();

    constexpr uint8_t f_RequiredThreads = (PeachCore::Subsystem::Audio | PeachCore::Subsystem::Physics2D); 

    //writable storage on emscripten lives at /home/web_user by default in the IDBFS-mounted
    //virtual filesystem, but for simple cases /persist or just "/" works fine. PhysFS will
    //use this as both read and write root.
    if 
    (
        not f_Engine.InitializePeachEngineCustom
        (
            "/",
            f_RequiredThreads,
            PeachCore::RendererType::WebGL,
            1280, 720,
            false
        )
    )
    {
        return EXIT_FAILURE;
    }

    f_Engine.StartMainGameLoop();
    //StartMainGameLoop returns immediately on WASM. Don't call ShutdownPeachEngine — 
    //emscripten keeps the module alive and rAF callback runs after main() returns.
    //Page unload triggers cleanup via emscripten's atexit handlers if registered.

    return EXIT_SUCCESS;
}

#endif /*PEACH_PLATFORM_WASM*/