#pragma once

//#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <functional>
#include <iostream>

#include <SDL3/SDL.h>

using namespace std;

constexpr uint32_t MAX_TEXT_SIZE = 128;

namespace PeachCore {

    enum class InputType
    {
        KeyDown,
        KeyUp,
        MouseDown,
        MouseUp,
        MouseMove,
        MouseScroll,
        TextInput
    };

    struct KeyboardEvent
    {
        InputType type;
        SDL_Scancode key;         // For key input

        char text[MAX_TEXT_SIZE]; // For typing
        uint64_t timestamp;
        bool consumed = false;    // UI can mark this so it doesn't reach gameplay
    };

    struct MouseEvent
    {
        SDL_Scancode key;         // For key input
        uint32_t mouseButton;     // SDL_BUTTON_LEFT, etc.
        float mouseX, mouseY;     // World-space or screen-space (decide!)
        float deltaX, deltaY;     // For motion
        int wheelX, wheelY;       // For scroll
    };

    struct GamePadEvent
    {

    };

    struct InputState
    {
        unordered_map<SDL_Scancode, bool> IsDown;
        unordered_map<SDL_Scancode, uint64_t> LastPressedTimestamp;
        unordered_map<SDL_Scancode, int> RepeatCount;
        unordered_map<uint8_t, bool> MouseButtonDown;
        float MouseX = 0.0f;
        float MouseY = 0.0f;
        float MouseDeltaX = 0.0f;
        float MouseDeltaY = 0.0f;
        float ScrollX = 0.0f;
        float ScrollY = 0.0f;
    };

    class InputManager {
    public:
        static InputManager& GetInstance() {
            static InputManager instance;
            return instance;
        }

        ~InputManager() = default;

    public:

        void PollEvents();

        //void MapInput(const string& action, sf::Keyboard::Key key);

        bool GetPressed(const string& action)
            const;

        //sf::Vector2f GetCurrentMousePosition(const sf::RenderWindow& window)
        //    const;

        //void ProcessInput(sf::RenderWindow& window);

    private:
        InputManager(); // Constructor made private for singleton

    private:
        //map<string, sf::Keyboard::Key> pm_InputMap;
    };

}
