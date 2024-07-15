#include "../../include/Managers/InputManager.h"

namespace PeachCore {
	InputManager::InputManager()
	{

	}

	InputManager:: ~InputManager()
	{

	}

    void InputManager::MapInput(const std::string& action, sf::Keyboard::Key key) {
        pm_InputMap[action] = key;
    }

    bool InputManager::GetPressed(const std::string& action) const {
        if (pm_InputMap.find(action) == pm_InputMap.end()) return false;
        return sf::Keyboard::isKeyPressed(pm_InputMap.at(action));
    }

    sf::Vector2f InputManager::GetCurrentMousePosition(const sf::RenderWindow& window) const {
        auto mousePosition = sf::Mouse::getPosition(window);
        return sf::Vector2f(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
    }

    void InputManager::ProcessInput(sf::RenderWindow& window) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
}