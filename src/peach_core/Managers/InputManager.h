/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///PeachCore
#include "Utils/Logger.h"

///SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>

///GLM
#include <glm/glm.hpp>

namespace PeachCore {

    constexpr float JOYSTICK_MAX_STICK_VALUE = SDL_JOYSTICK_AXIS_MAX;

    //struct DoubleVector2
    //{
    //    glm::vec2 ouo[2];

    //};

    // somewhere accessible to both — could live on GameManager or RenderingManager
    struct WindowState
    {
        std::atomic<uint32_t>  Width{0};
        std::atomic<uint32_t>  Height{0};

        std::atomic<bool> IsMinimized{false};
        std::atomic<bool> IsMaximized{ false };

        std::atomic<bool> IsKeyboardFocus{ false };
        std::atomic<bool> IsMouseFocus{ false };

        std::atomic<bool> IsHidden{false};
        std::atomic<bool> IsOccluded{ false };

        void
            SetMaximized()
        {
            IsMinimized.store(false, std::memory_order_relaxed);
            IsMaximized.store(true, std::memory_order_relaxed);
        }

        void
            SetMinimized()
        {
            IsMinimized.store(true, std::memory_order_relaxed);
            IsMaximized.store(false, std::memory_order_relaxed);
        }

        //void
        //    SetHidden()
        //{
        //    IsHidden.store(true, std::memory_order_relaxed);
        //    .store(false, std::memory_order_relaxed);
        //}

        //void 
        //    SetShown()
        //{
        //    IsMinimized.store(true, std::memory_order_relaxed);
        //    IsMaximized.store(false, std::memory_order_relaxed);
        //}
    };

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
    };

    struct JoystickInput
    {
        glm::vec2 StickPosition = glm::vec2(0);
    };

    struct GamepadInput
    {
        glm::vec2 StickPosition = glm::vec2(0);

        //ThreadsafeVec2() : x(0.0f), y(0.0f) {}

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
        SDL_Joystick* ID = nullptr;
        JoystickInput Input;
    };

    struct InputSnapshot //used for per frame tracking of inputs recorded
    {
        KeyboardInput KeyboardEvent;
        MouseInput MouseEvent;
        GamepadInput GamepadEvent;
        StylusInput StylusEvent;
        TouchInput TouchEvent;

        void
            Clear(Logger* logger)
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
                logger->Error("failed to fill array with fresh values for keyboard events", "InputManager::InputSnapshot::Clear");
            }

        }
    };

    class InputManager 
    {
    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        unique_ptr<Logger> input_logger = nullptr;

        unordered_map<string, vector<InputBinding>> pm_InputMap; //action name : corresponding input

        InputSnapshot pm_LastFrameInput;
        InputSnapshot pm_CurrentFrameInput;

        bool pm_IsKeyboardConnected = false;
        bool pm_IsMouseConnected = false;

        bool pm_IsStylusConnected = false;
        bool pm_IsJoystickConnected = false;
        bool pm_IsGamepadConnected = false;
        bool pm_IsTouchControlsActive = false;

        unordered_map<SDL_JoystickID, Gamepad> pm_ConnectedGamepads;
        unordered_map<SDL_JoystickID, Joystick> pm_ConnectedJoysticks;

        unordered_map<SDL_PenID, Stylus> pm_ConnectedStyluses;

        vector<SDL_WindowID> pm_WindowCloseRequests;

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        //enum class ConnectedDevice : uint8_t
        //{
        //    None = 0,
        //    Keyboard = 1 << 0,
        //    Mouse = 1 << 1,
        //    Stylus = 1 << 2,
        //    Joystick = 1 << 3,
        //    Gamepad = 1 << 4,
        //    Touch = 1 << 5
        //};

        WindowState m_CurrentMainWindowState;

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
    // Public Methods
    //////////////////////////////////////////////
    public:
        [[nodiscard]] bool
            Initialize
            (
                const uint32_t fp_InitialWindowWidth,
                const uint32_t fp_InitialWindowHeight,
                const string& fp_LogOutputDirectory,
                const uint32_t fp_LogFlags
            );

        void
            PollEvents();

        void 
            MapInput
            (
                const string& fp_ActionName, 
                InputBinding& fp_InputBinding
            );

        [[nodiscard]] bool 
            WasPressed(const string& fp_ActionName)
            const noexcept;

        [[nodiscard]] bool
            WasReleased(const string& fp_ActionName)
            const noexcept;

        [[nodiscard]] inline glm::vec2
            GetCurrentMousePosition()
            const noexcept
        {
            return pm_CurrentFrameInput.MouseEvent.Position;
        }

        inline void
            GetWindowCloseRequests(vector<SDL_WindowID>& fp_WindowCustomer)
            noexcept
        {
            if (pm_WindowCloseRequests.size() > 0)
            {
                swap(pm_WindowCloseRequests, fp_WindowCustomer);
                pm_WindowCloseRequests.clear();
            }
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:

    };

}
