#pragma once

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <functional>
#include <iostream>

using namespace std;

namespace PeachCore {

    class InputManager {
    public:
        static InputManager& GetInstance() {
            static InputManager instance;
            return instance;
        }

        ~InputManager();

    public:

        void MapInput(const string& action, sf::Keyboard::Key key);

        bool GetPressed(const string& action)
            const;

        sf::Vector2f GetCurrentMousePosition(const sf::RenderWindow& window)
            const;

        void ProcessInput(sf::RenderWindow& window);

    private:
        InputManager(); // Constructor made private for singleton

    private:
        map<string, sf::Keyboard::Key> pm_InputMap;
    };

}
