#include "../../include/Managers/SceneTreeManager.h"

namespace PeachCore {

	void SceneTreeManager::ChangeScene(const std::string& fp_FilePathToDesiredScene)
	{

	}

	void SceneTreeManager::UnloadCurrentScene()
	{

	}

	Scene* SceneTreeManager::LoadDesiredScene(const std::string& fp_FilePathToDesiredScene) const
	{
		//return Scene{ "" };
        return nullptr;
	}

    

    //void SceneTreeManager::ChangeScene(const std::string& fp_SceneName) {
    //    Scene f_DesiredScene = pm_Scenes.find(fp_SceneName);

    //    if (f_DesiredScene != pm_Scenes.end()) {
    //        m_CurrentScene = f_DesiredScene->second;
    //    }
    //    else {
    //        std::cerr << "Scene " << fp_SceneName << " not found!" << std::endl;
    //    }
    //}

    //void SceneTreeManager::AddScene(const std::shared_ptr<Scene>& fp_Scene) {
    //    scenes[fp_Scene->GetName()] = scene;
    //}

    void 
        SceneTreeManager::RemoveScene(const std::string& fp_SceneName)
    {
        pm_Scenes.erase(fp_SceneName);
    }

    void 
        SceneTreeManager::Update(float fp_TimeSinceLastFrame) 
    {
        if (m_CurrentScene)
        {
            m_CurrentScene->Update(fp_TimeSinceLastFrame);
        }
    }

    void 
        SceneTreeManager::PauseCurrentScene()
    {
        if (m_CurrentScene) 
        {
            m_CurrentScene->Pause();
        }
    }

    void 
        SceneTreeManager::ResumeCurrentScene() 
    {
        if (m_CurrentScene) 
        {
            m_CurrentScene->Resume();
        }
    }
    
}