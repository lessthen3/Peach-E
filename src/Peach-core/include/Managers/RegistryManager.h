#pragma once

#include <string>
#include <entt/entt.hpp>

namespace PeachCore {

    class RegistryManager {
    public:
        static RegistryManager& Registry() {
            static RegistryManager instance;
            return instance;
        }

        entt::registry& GetRegistry() {
            return registry;
        }

    private:
        RegistryManager() = default;
        RegistryManager(const RegistryManager&) = delete;
        RegistryManager& operator=(const RegistryManager&) = delete;

        entt::registry registry;
    };

} 
