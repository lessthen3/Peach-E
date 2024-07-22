#include "../../include/Managers/InputManager.h"

namespace PeachCore {

    InputManager::InputManager()
    {

    }

    InputManager::~InputManager()
    {

    }

    void InputManager::MapInput(const string& fp_Action, sf::Keyboard::Key fp_KeyCode)
    {
        pm_InputMap[fp_Action] = fp_KeyCode;
    }

    bool 
        InputManager::GetPressed(const string& fp_Action)
        const
    {
        if (pm_InputMap.find(fp_Action) == pm_InputMap.end()) return false;
        return sf::Keyboard::isKeyPressed(pm_InputMap.at(fp_Action));
    }

    sf::Vector2f InputManager::GetCurrentMousePosition(const sf::RenderWindow& window)
        const 
    {
        auto mousePosition = sf::Mouse::getPosition(window);
        return sf::Vector2f(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
    }

    void InputManager::ProcessInput(sf::RenderWindow& window)
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
}
