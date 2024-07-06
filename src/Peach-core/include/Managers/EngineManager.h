#pragma once

#include <string>


#include <unordered_map>
#include <stdexcept>

namespace PeachCore {

    class EngineManager {
    public:
        static EngineManager& Registry() {
            static EngineManager instance;
            return instance;
        }

    private:
        EngineManager() = default;

        EngineManager(const EngineManager&) = delete;
        EngineManager& operator=(const EngineManager&) = delete;
    };

}
