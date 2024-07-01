//#pragma once
//
//#include <SDL2/SDL.h>
//#include <map>
//#include <string>
//#include <functional>
//#include <iostream>
//
//namespace PeachCore {
//
//    class InputManager {
//    public:
//        static InputManager& instance() {
//            static InputManager instance;
//            return instance;
//        }
//
//        InputManager() {
//            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//                // Handle initialization error
//                std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
//            }
//        }
//
//        ~InputManager() {
//            SDL_Quit();
//        }
//
//        void mapInput(const std::string& action, SDL_Keycode key) {
//            inputMap[action] = key;
//        }
//
//        bool getPressed(const std::string& action) {
//            if (inputMap.find(action) == inputMap.end()) return false;
//            const Uint8* state = SDL_GetKeyboardState(NULL);
//            return state[SDL_GetScancodeFromKey(inputMap[action])];
//        }
//
//        void processInput() {
//            SDL_Event event;
//            while (SDL_PollEvent(&event)) {
//                // Handle events here if needed
//            }
//        }
//
//    private:
//        std::map<std::string, SDL_Keycode> inputMap;
//    };
//
//}