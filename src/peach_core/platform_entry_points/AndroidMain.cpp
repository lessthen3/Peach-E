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

// just dont include unless need a main entry point for ios, tvos, wasm or android
// marginally speeds up compilation since this TU doesn't compile anything owo

#ifdef PEACH_PLATFORM_ANDROID

#include <SDL3/SDL_main.h> // DO NOT define SDL_MAIN_HANDLED here — that's handled by CMake, SDL_main.h will rename main() → SDL_main() on Android
#include "managers/GameManager.h"

int 
    main(int fp_ArgCount, char** fp_ArgVector)
{
    /*
        SDL called us, so SDL is already initialized enough for main to run.
        Use SDL_GetPrefPath for a writable root — DO NOT hardcode /data/data/...
        It returns something like /data/data/com.starlightbrew.peach/files/
    */

    char* f_PrefPath = SDL_GetPrefPath("starlightbrew", "peach");
    std::string f_RootPath = f_PrefPath ? f_PrefPath : "/data/data/com.starlightbrew.peach/files";
    SDL_free(f_PrefPath);

    auto& f_Engine = PeachCore::GameManager::get_single();

    constexpr uint8_t f_RequiredThreads = ( PeachCore::Subsystem::Render | PeachCore::Subsystem::Audio | PeachCore::Subsystem::Physics2D); 

    if 
    (
        not f_Engine.InitializePeachEngineCustom
        (
            f_RootPath,
            f_RequiredThreads,
            PeachCore::RendererType::Vulkan, //TODO: hook runtime GLES fallback once IsVulkanUsable() lands
            1280, //placeholder, mobile fullscreens to native resolution
            720,  //placeholder
            false //let peachy install its segfault handler
        )
    )
    {
        return EXIT_FAILURE;
    }

    f_Engine.StartMainGameLoop();
    f_Engine.ShutdownPeachEngine();

    return EXIT_SUCCESS;
}

#endif /*PEACH_PLATFORM_ANDROID*/

