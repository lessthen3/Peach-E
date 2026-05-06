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

    if (not f_Engine.InitializePeachEngine(f_RootPath))
    {
        return EXIT_FAILURE;
    }

    f_Engine.StartMainGameLoop();
    f_Engine.ShutdownPeachEngine();

    return EXIT_SUCCESS;
}

#endif /*PEACH_PLATFORM_ANDROID*/