#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <functional>
#include <iostream>

namespace PeachCore {

    class InputManager {
    public:
        static InputManager& GetInstance() {
            static InputManager instance;
            return instance;
        }

        ~InputManager();

    public:

        void MapInput(const std::string& action, sf::Keyboard::Key key);

        bool GetPressed(const std::string& action)
            const;

        sf::Vector2f GetCurrentMousePosition(const sf::RenderWindow& window) 
            const;

        void ProcessInput(sf::RenderWindow& window);

    private:
        InputManager(); // Constructor made private for singleton
        

    private:
        std::map<std::string, sf::Keyboard::Key> pm_InputMap;
    };

}