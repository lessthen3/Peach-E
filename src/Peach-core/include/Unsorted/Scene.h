#pragma once

#include <string>
#include <vector>
#include <memory>

#include "PeachNode.h"

namespace PeachCore {

	class Scene {
    public:

        Scene(const std::string& fp_Name) : pm_Name(fp_Name), pm_IsPaused(false) {}
        ~Scene();

        void AddNode(std::shared_ptr<PeachNode> fp_PeachGameObject) {
            //make unique here and push that back
            pm_PeachNodes.push_back(fp_PeachGameObject);
        }

        //void RemoveNode(const std::string& fp_NodeName) {
        //    pm_PeachNodes.erase(std::remove_if(pm_PeachNodes.begin(), pm_PeachNodes.end(),
        //        [&fp_NodeName](std::shared_ptr<PeachNode> L_PeachGameObject) {
        //            return L_PeachGameObject->name == fp_NodeName;
        //        }), pm_PeachNodes.end());
        //}

        void Update(float fp_TimeSinceLastFrame) {
            if (pm_IsPaused) return;
            for (auto& node : pm_PeachNodes) {
                node->Update(fp_TimeSinceLastFrame);
            }
        }

        void Draw() {
            if (pm_IsPaused) return;
            for (auto& node : pm_PeachNodes) {
                node->Draw();
            }
        }

        void Pause() { pm_IsPaused = true; }
        void Resume() { pm_IsPaused = false; }

        std::string GetName() const { return pm_Name; }

    private:
        std::string pm_Name; // >w< rawr
        bool pm_IsPaused; //stops all PeachNodes that are pausable
        std::vector<std::shared_ptr<PeachNode>> pm_PeachNodes; //use raw pointers for local function bound operations on PeachNodes via SceneTreeManager

	};
}