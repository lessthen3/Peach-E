#pragma once
class RegistryManager
{
public:
    static RegistryManager& Registry() {
        static RegistryManager instance;
        return instance;
    }

private:
    RegistryManager() = default;
    ~RegistryManager() = default;

    RegistryManager(const RegistryManager&) = delete;
    RegistryManager& operator=(const RegistryManager&) = delete;
};

