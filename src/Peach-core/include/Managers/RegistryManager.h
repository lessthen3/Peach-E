#pragma once

#include <string>


#include <unordered_map>
#include <stdexcept>

namespace PeachCore {

    class RegistryManager {
    public:
        static RegistryManager& Registry() {
            static RegistryManager instance;
            return instance;
        }

    private:
        RegistryManager() = default;

        RegistryManager(const RegistryManager&) = delete;
        RegistryManager& operator=(const RegistryManager&) = delete;
    };

}
