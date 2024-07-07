#pragma once

#include <string>


#include <unordered_map>
#include <stdexcept>

namespace PeachCore {

    class PeachEngineManager {
    public:
        static PeachEngineManager& Registry() {
            static PeachEngineManager instance;
            return instance;
        }

    private:
        PeachEngineManager() = default;

        PeachEngineManager(const PeachEngineManager&) = delete;
        PeachEngineManager& operator=(const PeachEngineManager&) = delete;
    };

}
