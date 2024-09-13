#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <iostream>

#include "../General/Scene.h"

namespace PeachCore {


    class SceneTreeManager {
    public:
        static SceneTreeManager& SceneTree() {
            static SceneTreeManager instance;
            return instance;
        }

        ~SceneTreeManager() = default;

        //void ChangeScene(const std::string& fp_FilePathToDesiredScene);

        void AddEntityToCurrentSceneTree(PeachNode fp_Entity);
        void PauseAllEntities();

    public:
        //Scene m_CurrentScene;

        void ChangeScene(const std::string& sceneName);
        void AddScene(const std::shared_ptr<Scene>& scene);
        void RemoveScene(const std::string& sceneName);

        void Update(float fp_TimeSinceLastFrame);
        void Draw();

        void PauseCurrentScene();
        void ResumeCurrentScene();

        void UnloadCurrentScene();
        Scene* LoadDesiredScene(const std::string& fp_FilePathToDesiredScene) const;

        void AddChild();

    private:
        SceneTreeManager();

        SceneTreeManager(const SceneTreeManager&) = delete;
        SceneTreeManager& operator=(const SceneTreeManager&) = delete;

        std::unordered_map<std::string, std::shared_ptr<Scene>> pm_Scenes;
        std::shared_ptr<Scene> m_CurrentScene;

    };
}