#pragma once
#include "../../../Peach-core/include/Managers/SceneTreeManager.h"
#include <map>

namespace PeachCore {

    class EngineManager {
    public:
        static EngineManager& Engine() {
            static EngineManager instance;
            return instance;
        }

    private:
        EngineManager() = default;
        ~EngineManager() = default;

        EngineManager(const EngineManager&) = delete;
        EngineManager& operator=(const EngineManager&) = delete;


        std::string CurrentlySelectedRenderer = "Nothing";

        std::map<std::string, PeachCore::Scene> DictionaryOfAllScenes = {};

    };
}