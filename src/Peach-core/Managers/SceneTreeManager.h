#pragma once

#include <vector>
#include <string>
#include "../Entity-Components/PeachComponent.h"

namespace PeachCore {

    struct Scene {
        std::string Name;
        std::vector<PeachComponent> ListOfEntities;
    };

    class SceneTreeManager {
    public:
        static SceneTreeManager& SceneTree() {
            static SceneTreeManager instance;
            return instance;
        }

        void ChangeScene(const std::string& fp_FilePathToDesiredScene);

        void AddEntityToCurrentSceneTree(PeachComponent fp_Entity);

    public:
        Scene currentScene;

    private:
        void UnloadCurrentScene();
        Scene LoadDesiredScene(const std::string& fp_FilePathToDesiredScene) const;

    private:
        SceneTreeManager() = default;
        ~SceneTreeManager() = default;
        SceneTreeManager(const SceneTreeManager&) = delete;
        SceneTreeManager& operator=(const SceneTreeManager&) = delete;

    };
}