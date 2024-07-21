#include "../../include/Managers/InputManager.h"

namespace PeachCore {

    InputManager::InputManager()
    {

    }

    InputManager::~InputManager()
    {

    }

    void InputManager::MapInput(const string& fp_Action, int fp_KeyCode) 
    {
        pm_InputMap[fp_Action] = fp_KeyCode;
    }

    bool InputManager::GetPressed(const string& fp_Action)
        const
    {
        if (pm_InputMap.find(fp_Action) == pm_InputMap.end()) return false;
        return IsKeyDown(pm_InputMap.at(fp_Action));
    }

    Vector2 InputManager::GetCurrentMousePosition() 
        const 
    {
        return GetMousePosition();
    }

    void InputManager::ProcessInput() 
    {
        if (WindowShouldClose()) 
        {
            CloseWindow();
        }
    }
}
