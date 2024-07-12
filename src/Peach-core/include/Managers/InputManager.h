#pragma once

#include <SDL2/SDL.h>
#include <map>
#include <string>
#include <functional>
#include <iostream>

#include "../Peach-Core2D/Utilities2D/Vector2D.h"

namespace PeachCore {

    class InputManager {
    public:
        static InputManager& Input() {
            static InputManager input;
            return input;
        }

        InputManager() {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                // Handle initialization error
                std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            }
        }

        ~InputManager() {
            SDL_Quit();
        }

        void mapInput(const std::string& action, SDL_Keycode key) {
            pm_InputMap[action] = key;
        }

        bool GetPressed(const std::string& action) // will work similiar to godot, where the desired key will be searched in the input_map
            const
        {
            if (pm_InputMap.find(action) == pm_InputMap.end()) return false;
            const Uint8* state = SDL_GetKeyboardState(NULL);
            return state[SDL_GetScancodeFromKey(pm_InputMap.at(action))];
        }

        Vector2f GetCurrentMousePosition() //this is where the user/me will call to get updates on the current Mouse position
            const
        {

            return Vector2f(); //returns a zero vector
        }

        void processInput() {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                // Handle events here if needed
            }
        }

    private:
        std::map<std::string, SDL_Keycode> pm_InputMap;
    };

}