#pragma once
#include "../Unsorted/raylib-conflictfree.h"


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

        void MapInput(const string& action, int key);

        bool GetPressed(const string& action) const;

        Vector2 GetCurrentMousePosition() const;

        void ProcessInput();

    private:
        InputManager(); // Constructor made private for singleton

    private:
        map<string, int> pm_InputMap;
    };

}
