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
#include "InputManager.h"

#include <fmt/format.h>

namespace PeachCore {

    [[nodiscard]] bool
        InputManager::Initialize
        (
            const uint32_t fp_InitialWindowWidth,
            const uint32_t fp_InitialWindowHeight,
            const string& fp_LogOutputDirectory,
            const uint32_t fp_LogFlags
        )
    {
        //////////////////// Initialize Logger ////////////////////

        input_logger = LogManager::get_single().CreateUniqueLogger("InputManager", fp_LogFlags, fp_LogOutputDirectory);

        if (not input_logger)
        {
            PEACH_PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: InputManager failed to initialize the input_logger >O<");
            return false;
        }

        input_logger->Info("InputLogger successfully initialized", "InputManager");

        m_CurrentMainWindowState.Width = fp_InitialWindowWidth;
        m_CurrentMainWindowState.Height = fp_InitialWindowHeight;

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
            input_logger->Error
            (
                fmt::format("Action named : '{}' was not found in the Input Map, did you misspell or forget to add '{}' to the Input Map?", fp_ActionName, fp_ActionName),
                "InputManager::WasPressed"
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
            default:
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
            input_logger->Error
            (
                fmt::format("Action named : '{}' was not found in the Input Map, did you misspell or forget to add '{}' to the Input Map?", fp_ActionName, fp_ActionName),
                "InputManager::WasPressed"
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
            default:
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

        SDL_Event wv_Event;

        while (SDL_PollEvent(&wv_Event))
        {
            switch (wv_Event.type)
            {
            //////////////////// Keyboard Input Stuff ////////////////////

            case SDL_EVENT_KEY_DOWN:
                pm_CurrentFrameInput.KeyboardEvent.IsKeyDown[wv_Event.key.scancode] = { true, false };
                break;
            case SDL_EVENT_KEY_UP:
                pm_CurrentFrameInput.KeyboardEvent.IsKeyDown[wv_Event.key.scancode] = { false, false };
                break;

            //////////////////// Mouse Input Handling ////////////////////

            case SDL_EVENT_MOUSE_MOTION:
                pm_CurrentFrameInput.MouseEvent.Delta = {{wv_Event.motion.xrel, wv_Event.motion.yrel}};
                pm_CurrentFrameInput.MouseEvent.Position = {{wv_Event.motion.x, wv_Event.motion.y}};
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                pm_CurrentFrameInput.MouseEvent.Scroll = {{wv_Event.wheel.x, wv_Event.wheel.y}};
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                pm_CurrentFrameInput.MouseEvent.MouseButtonsDown[wv_Event.button.button] = { true, false };
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                pm_CurrentFrameInput.MouseEvent.MouseButtonsDown[wv_Event.button.button] = { true, false };
                break;

            //////////////////// Stylus Input ////////////////////

            case SDL_EVENT_PEN_PROXIMITY_IN: //add pen when it comes into proximity, sdl has no other way to detect connected styluses so thisll work for now
            {
                SDL_PenID f_PenID = wv_Event.pproximity.which;

                // Register a new stylus if it doesn't exist
                if (not pm_ConnectedStyluses.contains(f_PenID))
                {
                    pm_ConnectedStyluses[f_PenID] = Stylus
                    {
                        .PenID = f_PenID,
                        .Input = {}
                    };

                    input_logger->Info
                    (
                        fmt::format("Stylus with ID: '{}' entered proximity", f_PenID),
                        "InputManager::PollEvents"
                    );
                }

                break;
            }

            case SDL_EVENT_PEN_DOWN:
            case SDL_EVENT_PEN_MOTION:
            {
                SDL_PenID f_PenID = wv_Event.pmotion.which;

                if (pm_ConnectedStyluses.contains(f_PenID))
                {
                    pm_ConnectedStyluses[f_PenID].Input.Position = {{wv_Event.pmotion.x, wv_Event.pmotion.y}};
                }

                break;
            }

            case SDL_EVENT_PEN_AXIS:
            {
                SDL_PenID f_PenID = wv_Event.paxis.which;

                if (pm_ConnectedStyluses.contains(f_PenID))
                {
                    switch(wv_Event.paxis.axis)
                    {
                    case SDL_PEN_AXIS_PRESSURE:
                        pm_ConnectedStyluses[f_PenID].Input.Pressure = wv_Event.paxis.value;
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
                    case SDL_PEN_AXIS_COUNT:
                        break;

                    default: //XXX: dont really need default here since we handle all cases but just feels right owo
                        break;
                    }
                }

                break;
            }

            case SDL_EVENT_PEN_UP:
            {
                SDL_PenID f_PenID = wv_Event.pmotion.which;

                if (pm_ConnectedStyluses.contains(f_PenID))
                {
                    pm_ConnectedStyluses[f_PenID].Input.Pressure = 0.0f;
                }

                break;
            }

            case SDL_EVENT_PEN_PROXIMITY_OUT: //remove pen from connected styluses when out of proximity
            {
                SDL_PenID f_PenID = wv_Event.pproximity.which;

                if (pm_ConnectedStyluses.contains(f_PenID))
                {
                    pm_ConnectedStyluses.erase(f_PenID);

                    input_logger->Info
                    (
                        fmt::format("Stylus with ID: '{}' left proximity", f_PenID),
                        "InputManager::PollEvents"
                    );
                }

                break;
            }

            //////////////////// Gamepad UwU ////////////////////

            case SDL_EVENT_GAMEPAD_ADDED:
            {
                SDL_JoystickID f_JoystickID = wv_Event.gdevice.which;
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

                    input_logger->Info
                    (
                        fmt::format("Gamepad connected with ID: {}", f_JoystickID),
                        "InputManager::PollEvents"
                    );
                }

                break;
            }
            case SDL_EVENT_GAMEPAD_REMOVED:
            {
                SDL_JoystickID f_JoystickID = wv_Event.gdevice.which;

                if (pm_ConnectedGamepads.contains(f_JoystickID))
                {
                    SDL_CloseGamepad(pm_ConnectedGamepads[f_JoystickID].ID);

                    pm_ConnectedGamepads.erase(f_JoystickID);

                    input_logger->Info
                    (
                        fmt::format("Gamepad disconnected with ID: {}", f_JoystickID),
                        "InputManager::PollEvents"
                    );

                    pm_IsGamepadConnected = not pm_ConnectedGamepads.empty();
                }

                break;
            }
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            {
                SDL_JoystickID f_JoystickID = wv_Event.gaxis.which;

                auto it = pm_ConnectedGamepads.find(f_JoystickID);

                if (it != pm_ConnectedGamepads.end())
                {
                    float x = SDL_GetGamepadAxis(it->second.ID, SDL_GAMEPAD_AXIS_LEFTX) / JOYSTICK_MAX_STICK_VALUE;
                    float y = SDL_GetGamepadAxis(it->second.ID, SDL_GAMEPAD_AXIS_LEFTY) / JOYSTICK_MAX_STICK_VALUE;

                    it->second.Input.StickPosition = {{x, y}};
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
                SDL_JoystickID f_JoystickID = wv_Event.gdevice.which;
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

                    input_logger->Info
                    (
                        fmt::format("Joystick connected with ID: {}", f_JoystickID),
                        "InputManager::PollEvents"
                    );
                }

                break;
            }        
            case SDL_EVENT_JOYSTICK_REMOVED:
            {
                SDL_JoystickID f_JoystickID = wv_Event.gdevice.which;

                if (pm_ConnectedJoysticks.contains(f_JoystickID))
                {
                    SDL_CloseJoystick(pm_ConnectedJoysticks[f_JoystickID].ID);

                    pm_ConnectedJoysticks.erase(f_JoystickID);

                    input_logger->Info
                    (
                        fmt::format("Gamepad disconnected with ID: {}", f_JoystickID),
                        "InputManager::PollEvents"
                    );

                    pm_IsJoystickConnected = not pm_ConnectedJoysticks.empty();
                }

                break;
            }
            case SDL_EVENT_JOYSTICK_AXIS_MOTION:
            {
                SDL_JoystickID f_JoystickID = wv_Event.gaxis.which;

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

            //////////////////// Touch Inputs ////////////////////

            case SDL_EVENT_FINGER_MOTION:
                break;
            case SDL_EVENT_FINGER_DOWN:
                break;
            case SDL_EVENT_FINGER_UP:
                break;
            case SDL_EVENT_FINGER_CANCELED:
                break;

            case SDL_EVENT_PINCH_BEGIN:
                break;
            case SDL_EVENT_PINCH_END:
                break;
            case SDL_EVENT_PINCH_UPDATE:
                break;

            //////////////////// Camera Stuff ig idk ////////////////////

            case SDL_EVENT_CAMERA_DEVICE_ADDED:
                break;

            //////////////////// Window Stuff ////////////////////

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                pm_WindowCloseRequests.push_back(wv_Event.window.windowID);
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                m_CurrentMainWindowState.IsKeyboardFocus.store(false, std::memory_order_relaxed);
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                m_CurrentMainWindowState.IsKeyboardFocus.store(true, std::memory_order_relaxed);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                m_CurrentMainWindowState.Width.store(wv_Event.window.data1, std::memory_order_relaxed);
                m_CurrentMainWindowState.Height.store(wv_Event.window.data2, std::memory_order_relaxed); //idek what data2 is lmfao
                break;
            case SDL_EVENT_WINDOW_OCCLUDED:
                m_CurrentMainWindowState.IsOccluded.store(true, std::memory_order_relaxed);
                break;
            case SDL_EVENT_WINDOW_EXPOSED: // window became visible again
                m_CurrentMainWindowState.IsOccluded.store(false, std::memory_order_relaxed);
                m_CurrentMainWindowState.IsSurfaceReady.store(true, std::memory_order_release);
                break;
            ///NOT SURE IF THESE ARE NEEDED SINCE THERE IS A WINDOW FLAG FOR MOUSE FOCUS ALREADY
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                m_CurrentMainWindowState.IsMouseFocus.store(false, std::memory_order_relaxed);
                break;
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                m_CurrentMainWindowState.IsMouseFocus.store(true, std::memory_order_relaxed);
                break;

            case SDL_EVENT_WINDOW_HIDDEN:
                m_CurrentMainWindowState.IsHidden.store(true, std::memory_order_release);
                                m_CurrentMainWindowState.IsSurfaceReady.store(false, std::memory_order_release);
                break;
            case SDL_EVENT_WINDOW_RESTORED:
            case SDL_EVENT_WINDOW_SHOWN:
                m_CurrentMainWindowState.IsHidden.store(false, std::memory_order_release);
                m_CurrentMainWindowState.IsSurfaceReady.store(true, std::memory_order_release);
                break;
            case SDL_EVENT_WINDOW_MAXIMIZED:
                m_CurrentMainWindowState.SetMaximized();
                break;
            case SDL_EVENT_WINDOW_MINIMIZED:
                m_CurrentMainWindowState.SetMinimized();
                m_CurrentMainWindowState.IsHidden.store(true, std::memory_order_release);
                m_CurrentMainWindowState.IsSurfaceReady.store(false, std::memory_order_release);
                break;

            case SDL_EVENT_WILL_ENTER_BACKGROUND:
            case SDL_EVENT_DID_ENTER_BACKGROUND:
                m_CurrentMainWindowState.IsHidden.store(true, std::memory_order_release);
                break;

            case SDL_EVENT_DID_ENTER_FOREGROUND:
                m_CurrentMainWindowState.IsHidden.store(false, std::memory_order_release);
                //flag swapchain for recreation since the underlying surface may have been lost
                //during backgrounding; let renderer detect via VK_ERROR_SURFACE_LOST and recreate
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
