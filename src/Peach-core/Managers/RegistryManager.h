#pragma once

#include <string>
#include <entt/entt.hpp>

namespace PeachCore {

    struct RegistryManager
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

    public:
        void AddPeachComponent();
        void RemovePeachComponent();
        void GetPeachComponent();
    };
}
