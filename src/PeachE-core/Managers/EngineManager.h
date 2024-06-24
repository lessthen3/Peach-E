#pragma once
//#include "SceneTreeManager.h"
#include <map>

class EngineManager {
public:
    static EngineManager& Engine() {
        static EngineManager instance;
        return instance;
    }

    std::string CurrentlySelectedRenderer = "Nothing";

    std::map<std::string, Scene> DictionaryOfAllScenes = {};

};