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
#include "../../include/Peach-Core/Managers/InputManager.h"

namespace PeachCore {

    [[nodiscard]] bool
        InputManager::Initialize
        (
            const string& fp_LogOutputDirectory,
            const LogManager::LogLevel fp_LogFilter,
            shared_ptr<Console> fp_Console
        )
    {
        //////////////////// Nullptr check for PeachConsole ref ////////////////////

        if (not fp_Console)
        {
            PrintError("Tried to initialize RenderingManager with a nullptr reference to the Console");
            return false;
        }

        //////////////////// Initialize Logger ////////////////////

        input_logger = make_unique<LogManager>();
        input_logger->Initialize(ThreadName::MainThread, fp_LogOutputDirectory, "InputManager", fp_Console, fp_LogFilter);
        input_logger->PEACH_LOG("InputLogger successfully initialized", "RenderingManager", LogManager::LogLevel::Debug);

        return true;
    }

    void 
        InputManager::MapInput
        (
            const string& fp_Action, 
            InputBinding& fp_InputBinding
        )
    {
        pm_InputMap[fp_Action.c_str()].push_back(fp_InputBinding);
    }

    [[nodiscard]] bool
        InputManager::WasPressed(const string& fp_ActionName)
        const noexcept
    {
        auto f_FindActionResult = pm_InputMap.find(fp_ActionName);

        if (f_FindActionResult == pm_InputMap.end())
        {
            input_logger->PEACH_LOG
            (
                format("Action named : '{}' was not found in the Input Map, did you misspell or forget to add '{}' to the Input Map?", fp_ActionName, fp_ActionName),
                "InputManager::WasPressed",
                LogManager::LogLevel::Error
            );

            return false;
        }

        for (const auto& lv_Binding : f_FindActionResult->second)
        {
            switch(lv_Binding.Device)
            {
            case DeviceType::Keyboard:
                if (pm_CurrentFrameInput.KeyboardEvent.WasKeyPressed[lv_Binding.u.Key].IsActive)
                {
                    return true;
                }
                break;
            case DeviceType::Mouse:
                if (pm_CurrentFrameInput.MouseEvent.MouseButtonsDown[lv_Binding.u.MouseButton].IsActive)
                {
                    return true;
                }
                break;
            case DeviceType::Gamepad:
                //placeholder
                break;
            case DeviceType::Joystick:
                //placeholder
                break;
            case DeviceType::Stylus:
                //placeholder
                break;
            }
        }

        return false;
    }

    [[nodiscard]] bool
        InputManager::WasReleased(const string& fp_ActionName)
        const noexcept
    {
        auto f_FindActionResult = pm_InputMap.find(fp_ActionName);

        if (f_FindActionResult == pm_InputMap.end())
        {
            input_logger->PEACH_LOG
            (
                format("Action named : '{}' was not found in the Input Map, did you misspell or forget to add '{}' to the Input Map?", fp_ActionName, fp_ActionName),
                "InputManager::WasPressed",
                LogManager::LogLevel::Error
            );

            return false;
        }

        for (const auto& lv_Binding : f_FindActionResult->second)
        {
            switch (lv_Binding.Device)
            {
            case DeviceType::Keyboard:
                if (pm_CurrentFrameInput.KeyboardEvent.WasKeyReleased[lv_Binding.u.Key].IsActive)
                {
                    return true;
                }
                break;
            case DeviceType::Mouse:
                if (pm_CurrentFrameInput.KeyboardEvent.WasKeyReleased[lv_Binding.u.Key].IsActive)
                {
                    return true;
                }
                break;
            case DeviceType::Gamepad:
                if (pm_CurrentFrameInput.KeyboardEvent.WasKeyReleased[lv_Binding.u.Key].IsActive)
                {
                    return true;
                }
                break;
            case DeviceType::Joystick:
                if (pm_CurrentFrameInput.KeyboardEvent.WasKeyReleased[lv_Binding.u.Key].IsActive)
                {
                    return true;
                }
                break;
            case DeviceType::Stylus:
                if (pm_CurrentFrameInput.KeyboardEvent.WasKeyReleased[lv_Binding.u.Key].IsActive)
                {
                    return true;
                }
                break;
            }
        }

        return false;
    }

    void 
        InputManager::PollEvents()
    {
        swap(pm_LastFrameInput, pm_CurrentFrameInput);
        pm_CurrentFrameInput.Clear(input_logger.get()); // set everything false or zero

        SDL_Event e;

        while (SDL_PollEvent(&e)) 
        {
            switch (e.type) 
            {
            //////////////////// Keyboard Input Stuff ////////////////////

            case SDL_EVENT_KEY_DOWN:
                pm_CurrentFrameInput.KeyboardEvent.IsKeyDown[e.key.scancode] = { true, false };
                break;
            case SDL_EVENT_KEY_UP:
                pm_CurrentFrameInput.KeyboardEvent.IsKeyDown[e.key.scancode] = { false, false };
                break;

            //////////////////// Mouse Input Handling ////////////////////

            case SDL_EVENT_MOUSE_MOTION:
                pm_CurrentFrameInput.MouseEvent.Delta = { e.motion.xrel, e.motion.yrel };
                pm_CurrentFrameInput.MouseEvent.Position = { e.motion.x, e.motion.y };
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                pm_CurrentFrameInput.MouseEvent.Scroll = { e.wheel.x, e.wheel.y };
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                pm_CurrentFrameInput.MouseEvent.MouseButtonsDown[e.button.button] = { true, false };
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                pm_CurrentFrameInput.MouseEvent.MouseButtonsDown[e.button.button] = { true, false };
                break;
            ///NOT SURE IF THESE ARE NEEDED SINCE THERE IS A WINDOW FLAG FOR MOUSE FOCUS ALREADY
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                pm_CurrentFrameInput.MouseEvent.IsCursorInWindow = false;
                break;
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                pm_CurrentFrameInput.MouseEvent.IsCursorInWindow = true;
                break;

            //////////////////// Stylus Input ////////////////////

            case SDL_EVENT_PEN_PROXIMITY_IN: //add pen when it comes into proximity, sdl has no other way to detect connected styluses so thisll work for now
            {
                SDL_PenID f_PenID = e.pproximity.which;

                // Register a new stylus if it doesn't exist
                if (not pm_ConnectedStyluses.contains(f_PenID))
                {
                    pm_ConnectedStyluses[f_PenID] = Stylus
                    {
                        .PenID = f_PenID,
                        .Input = {}
                    };

                    input_logger->PEACH_LOG
                    (
                        format("Stylus with ID: '{}' entered proximity", f_PenID),
                        "InputManager::PollEvents",
                        LogManager::LogLevel::Info
                    );
                }

                break;
            }

            case SDL_EVENT_PEN_DOWN:
            case SDL_EVENT_PEN_MOTION:
            {
                SDL_PenID f_PenID = e.pmotion.which;

                if (pm_ConnectedStyluses.contains(f_PenID))
                {
                    pm_ConnectedStyluses[f_PenID].Input.Position = { e.pmotion.x, e.pmotion.y };
                }

                break;
            }

            case SDL_EVENT_PEN_AXIS:
            {
                SDL_PenID f_PenID = e.paxis.which;

                if (pm_ConnectedStyluses.contains(f_PenID))
                {
                    switch(e.paxis.axis)
                    {
                    case SDL_PEN_AXIS_PRESSURE:
                        pm_ConnectedStyluses[f_PenID].Input.Pressure = e.paxis.value;
                        break;

                    case SDL_PEN_AXIS_DISTANCE:
                        break;

                    case SDL_PEN_AXIS_ROTATION:
                        break;

                    case SDL_PEN_AXIS_XTILT:
                        break;
                    case SDL_PEN_AXIS_YTILT:
                        break;

                    case SDL_PEN_AXIS_TANGENTIAL_PRESSURE:
                        break;

                    case SDL_PEN_AXIS_SLIDER:
                        break;
                    }
                }

                break;
            }

            case SDL_EVENT_PEN_UP:
            {
                SDL_PenID f_PenID = e.pmotion.which;

                if (pm_ConnectedStyluses.contains(f_PenID))
                {
                    pm_ConnectedStyluses[f_PenID].Input.Pressure = 0.0f;
                }

                break;
            }

            case SDL_EVENT_PEN_PROXIMITY_OUT: //remove pen from connected styluses when out of proximity
            {
                SDL_PenID f_PenID = e.pproximity.which;

                if (pm_ConnectedStyluses.contains(f_PenID))
                {
                    pm_ConnectedStyluses.erase(f_PenID);

                    input_logger->PEACH_LOG
                    (
                        format("Stylus with ID: '{}' left proximity", f_PenID),
                        "InputManager::PollEvents",
                        LogManager::LogLevel::Info
                    );
                }

                break;
            }

            //////////////////// Gamepad UwU ////////////////////

            case SDL_EVENT_GAMEPAD_ADDED:
            {
                SDL_JoystickID f_JoystickID = e.gdevice.which;
                SDL_Gamepad* f_GamepadHandle = SDL_OpenGamepad(f_JoystickID);

                if (f_GamepadHandle)
                {
                    pm_ConnectedGamepads[f_JoystickID] = Gamepad
                    {
                        .JoystickID = f_JoystickID,
                        .ID = f_GamepadHandle,
                        .Input = {}
                    };

                    pm_IsGamepadConnected = true;

                    input_logger->PEACH_LOG
                    (
                        format("Gamepad connected with ID: {}", f_JoystickID),
                        "InputManager::PollEvents",
                        LogManager::LogLevel::Info
                    );
                }

                break;
            }
            case SDL_EVENT_GAMEPAD_REMOVED:
            {
                SDL_JoystickID f_JoystickID = e.gdevice.which;

                if (pm_ConnectedGamepads.contains(f_JoystickID))
                {
                    SDL_CloseGamepad(pm_ConnectedGamepads[f_JoystickID].ID);

                    pm_ConnectedGamepads.erase(f_JoystickID);

                    input_logger->PEACH_LOG
                    (
                        format("Gamepad disconnected with ID: {}", f_JoystickID),
                        "InputManager::PollEvents",
                        LogManager::LogLevel::Info
                    );

                    pm_IsGamepadConnected = not pm_ConnectedGamepads.empty();
                }

                break;
            }
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            {
                SDL_JoystickID f_JoystickID = e.gaxis.which;

                auto it = pm_ConnectedGamepads.find(f_JoystickID);

                if (it != pm_ConnectedGamepads.end())
                {
                    float x = SDL_GetGamepadAxis(it->second.ID, SDL_GAMEPAD_AXIS_LEFTX) / JOYSTICK_MAX_STICK_VALUE;
                    float y = SDL_GetGamepadAxis(it->second.ID, SDL_GAMEPAD_AXIS_LEFTY) / JOYSTICK_MAX_STICK_VALUE;

                    it->second.Input.StickPosition = { x, y };
                }

                break;
            }

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                break;

            //////////////////// Joystick OwO ////////////////////

            case SDL_EVENT_JOYSTICK_ADDED:
            {
                SDL_JoystickID f_JoystickID = e.gdevice.which;
                SDL_Joystick* f_JoystickHandle = SDL_OpenJoystick(f_JoystickID);

                if (f_JoystickHandle)
                {
                    pm_ConnectedJoysticks[f_JoystickID] = Joystick
                    {
                        .JoystickID = f_JoystickID,
                        .ID = f_JoystickHandle,
                        .Input = {}
                    };

                    pm_IsJoystickConnected = true;

                    input_logger->PEACH_LOG
                    (
                        format("Joystick connected with ID: {}", f_JoystickID),
                        "InputManager::PollEvents",
                        LogManager::LogLevel::Info
                    );
                }

                break;
            }        
            case SDL_EVENT_JOYSTICK_REMOVED:
            {
                SDL_JoystickID f_JoystickID = e.gdevice.which;

                if (pm_ConnectedJoysticks.contains(f_JoystickID))
                {
                    SDL_CloseJoystick(pm_ConnectedJoysticks[f_JoystickID].ID);

                    pm_ConnectedJoysticks.erase(f_JoystickID);

                    input_logger->PEACH_LOG
                    (
                        format("Gamepad disconnected with ID: {}", f_JoystickID),
                        "InputManager::PollEvents",
                        LogManager::LogLevel::Info
                    );

                    pm_IsJoystickConnected = not pm_ConnectedJoysticks.empty();
                }

                break;
            }
            case SDL_EVENT_JOYSTICK_AXIS_MOTION:
            {
                SDL_JoystickID f_JoystickID = e.gaxis.which;

                auto it = pm_ConnectedJoysticks.find(f_JoystickID);

                if (it != pm_ConnectedJoysticks.end())
                {
                    it->second.Input.StickPosition.x = SDL_GetJoystickAxis(it->second.ID, SDL_GAMEPAD_AXIS_LEFTX) / JOYSTICK_MAX_STICK_VALUE;
                    it->second.Input.StickPosition.y = SDL_GetJoystickAxis(it->second.ID, SDL_GAMEPAD_AXIS_LEFTY) / JOYSTICK_MAX_STICK_VALUE;
                }

                break;
            }
            //////////////////// Audio Device Stuff ////////////////////

            case SDL_EVENT_AUDIO_DEVICE_ADDED:
                break;
            case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
                break;
            case SDL_EVENT_AUDIO_DEVICE_REMOVED:
                break;

            //////////////////// Window Stuff ////////////////////

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                pm_WindowCloseRequests.push_back(e.window.windowID);
                break;
            }
        }

        // After the loop: compute `wasPressed`, `wasReleased`
        for (size_t lv_Index = 0; lv_Index < SDL_SCANCODE_COUNT; ++lv_Index)
        {
            bool wasDown =  pm_LastFrameInput.KeyboardEvent.IsKeyDown[lv_Index].IsActive;
            InputState currDown = pm_CurrentFrameInput.KeyboardEvent.WasKeyPressed[lv_Index];

            pm_CurrentFrameInput.KeyboardEvent.WasKeyPressed[lv_Index] = { not wasDown and currDown.IsActive, false };
            pm_CurrentFrameInput.KeyboardEvent.WasKeyReleased[lv_Index] = { wasDown and not currDown.IsActive, false };
        }
    }
}
