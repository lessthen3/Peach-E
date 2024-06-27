class InputManager
{
public:
    static InputManager& Input() {
        static InputManager instance;
        return instance;
    }

private:
    InputManager() = default;
    ~InputManager() = default;

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
};