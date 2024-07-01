#pragma once

#include <vector>
#include <string>
#include "../Peach-Prefabs/PeachEntity.h"

namespace PeachCore {

    struct Scene {
        std::string Name;
        std::vector<PeachEntity> ListOfEntities;
    };

    class SceneTreeManager {
    public:
        static SceneTreeManager& SceneTree() {
            static SceneTreeManager instance;
            return instance;
        }

        void ChangeScene(const std::string& fp_FilePathToDesiredScene);

        void AddEntityToCurrentSceneTree(PeachEntity fp_Entity);
        void PauseAllEntities();

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