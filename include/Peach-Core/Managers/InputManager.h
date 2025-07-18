/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///PeachCore
#include "LogManager.h"

///SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h >

///GLM
#include <glm/glm.hpp>

namespace PeachCore {

    struct InputState
    {
        bool IsActive = false;
        bool IsConsumed = false;
    };

    enum class DeviceType 
    { 
        None,
        Keyboard, 
        Gamepad, 
        Mouse, 
        Stylus, 
        Joystick 
    };

    struct InputBinding 
    {
        DeviceType Device = DeviceType::None;

        union
        {
            SDL_Scancode Key;
            SDL_GamepadButton GamepadButton;
            uint8_t MouseButton;
            SDL_PenID StylusID; 
            uint8_t PenButton;
            int JoystickButton;
        } u;
    };

    struct KeyboardInput
    {
        array<InputState, SDL_SCANCODE_COUNT> IsKeyDown;
        array<InputState, SDL_SCANCODE_COUNT> WasKeyPressed;
        array<InputState, SDL_SCANCODE_COUNT> WasKeyReleased;
    };

    struct MouseInput
    {
        array<InputState, 5> MouseButtonsDown; //not sure if this is right, it works for most configs of mice since the 5 button layout is common but idk not standard
        glm::vec2 Position = glm::vec2(0);
        glm::vec2 Delta = glm::vec2(0);     // For motion
        glm::vec2 Scroll = glm::vec2(0);       // For scroll

        bool IsCursorInWindow = true;
    };

    struct GamepadInput
    {
        glm::vec2 StickPosition = glm::vec2(0);
        //idk how to handle various button layouts
    };

    struct StylusInput //this should work for a generic stylus and an apple pencil type beat
    {
        float Pressure = 0.0f;
        float Rotation = 0.0f;
        float Tilt = 0.0f;

        glm::vec2 Position = glm::vec2(0);
    };

    struct TouchInput
    {

    };

    struct Stylus
    {
        SDL_PenID PenID = 0;
        StylusInput Input;
    };

    struct Gamepad
    {
        SDL_JoystickID  JoystickID = -1;
        SDL_Gamepad* ID = nullptr;
        GamepadInput Input;
    };

    struct Joystick
    {
        SDL_JoystickID JoystickID = -1;
    };

    struct InputSnapshot //used for per frame tracking of inputs recorded
    {
        KeyboardInput KeyboardEvent;
        MouseInput MouseEvent;
        GamepadInput GamepadEvent;
        StylusInput StylusEvent;
        TouchInput TouchEvent;

        void
            Clear()
        {
            MouseEvent = {};
            GamepadEvent = {};
            StylusEvent = {};

            try
            {
                KeyboardEvent.IsKeyDown.fill({ false, false });
                KeyboardEvent.WasKeyPressed.fill({ false, false });
                KeyboardEvent.WasKeyReleased.fill({ false, false });
            }
            catch (exception fp_Exception)
            {
                //idk
            }

        }
    };

    class InputManager 
    {
    //////////////////////////////////////////////
    // Private Destructor and Constructor
    //////////////////////////////////////////////
    private:
        ~InputManager() = default;
        InputManager() = default;

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static InputManager& get_single()
        {
            static InputManager input_manager;
            return input_manager;
        }

        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        InputManager(InputManager&&) = delete;
        InputManager& operator=(InputManager&&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        map<const char*, vector<InputBinding>> pm_InputMap; //action name : corresponding input
        unique_ptr<LogManager> input_logger = nullptr;

        InputSnapshot pm_LastFrameInput;
        InputSnapshot pm_CurrentFrameInput;

        bool pm_IsKeyboardConnected = false;
        bool pm_IsMouseConnected = false;

        bool pm_IsStylusConnected = false;
        bool pm_IsJoystickConnected = false;
        bool pm_IsGamepadConnected = false;
        bool pm_IsTouchControlsActive = false;

        unordered_map<string, SDL_Window*> pm_ActiveWindows; //name : window handle

        unordered_map<SDL_JoystickID, Gamepad> pm_ConnectedGamepads;
        unordered_map<SDL_PenID, Stylus> pm_ConnectedStyluses;

        vector<SDL_WindowID> pm_WindowCloseRequests;

        
    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:


    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:

        [[nodiscard]] bool
            Initialize
            (

            );

        void
            PollEvents();

        void 
            MapInput
            (
                const string& fp_ActionName, 
                SDL_Scancode fp_KeyboardEvent
            );

        //void
        //    MapInput
        //    (
        //        const string& fp_ActionName, 
        //        MouseEvent fp_MouseEvent
        //    );

        //void
        //    MapInput
        //    (
        //        const string& fp_ActionName,
        //        GamepadEvent fp_GamepadEvent
        //    );

        //void
        //    MapInput
        //    (
        //        const string& fp_ActionName, 
        //        StylusEvent fp_StylusEvent
        //    );

        [[nodiscard]] bool 
            WasPressed(const char* fp_ActionName)
            const noexcept;

        [[nodiscard]] bool
            WasReleased(const char* fp_ActionName)
            const noexcept;

        [[nodiscard]] inline glm::vec2
            GetCurrentMousePosition()
            const noexcept;

        inline void
            GetWindowCloseRequests(vector<SDL_WindowID>& fp_WindowCustomer)
            noexcept;

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:

    };

}
