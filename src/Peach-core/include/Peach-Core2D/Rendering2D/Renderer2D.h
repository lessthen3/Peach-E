#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <iostream>


namespace PeachCore
{

	struct Renderer2D
	{
    public:
        Renderer2D(SDL_Window* window, bgfx::RendererType::Enum type) {
            init(window, type);
        }

        ~Renderer2D() {
            bgfx::shutdown();
        }

        void renderFrame() {
            bgfx::touch(0);
            bgfx::frame();
        }

    private:
        void init(SDL_Window* window, bgfx::RendererType::Enum type) {
            SDL_SysWMinfo wmi;
            SDL_VERSION(&wmi.version);
            if (!SDL_GetWindowWMInfo(window, &wmi)) {
                std::cerr << "Unable to get window info: " << SDL_GetError() << std::endl;
                throw std::runtime_error("Failed to get window manager info.");
            }

            bgfx::Init bgfxInit;

        #if defined(_WIN32)
                    bgfxInit.platformData.nwh = wmi.info.win.window;  // Windows
        #elif defined(__linux__)
                    bgfxInit.platformData.nwh = (void*)wmi.info.x11.window;  // Linux
        #elif defined(__APPLE__)
                    bgfxInit.platformData.nwh = wmi.info.cocoa.window;  // macOS
        #endif

            bgfxInit.type = type; // Use the specified type or auto-select
            bgfxInit.resolution.width = 800;
            bgfxInit.resolution.height = 600;
            bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
            bgfx::init(bgfxInit);

            bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
            bgfx::setViewRect(0, 0, 0, 800, 600);
        }
	};

}

