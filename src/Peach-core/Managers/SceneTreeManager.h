#pragma once

#include <vector>
#include <string>
#include "../GameObject.h"

struct Scene {
    std::string Name;
    std::vector<GameObject> ListOfEntities;
};

class SceneTreeManager {
public:
    static SceneTreeManager& SceneTree() {
        static SceneTreeManager instance;
        return instance;
    }

    void ChangeScene(const std::string& fp_FilePathToDesiredScene);

    void AddEntityToCurrentSceneTree(GameObject fp_Entity);

public:
    Scene currentScene;

private:
    void UnloadCurrentScene();
    Scene LoadDesiredScene(const std::string& fp_FilePathToDesiredScene) const;

};