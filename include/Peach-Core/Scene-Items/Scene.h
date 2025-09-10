/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include <string>
#include <vector>
#include <memory>

#include "PeachNode.h"

namespace PeachCore {

    class Scene 
    {
    public:
        Scene() = default;

        //Scene(const string& fp_Name)
        //{
        //    pm_Name = fp_Name;
        //}

        ~Scene()
        {

        }
        // ObjectID : SceneTreeItem : Associated Update Package, used for updating all relevant data at the same time
//map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllCurrentlyActivePeachNodes;
//map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllPeachNodesQueuedForRemoval;
        void AddNode(shared_ptr<PeachNode> fp_PeachGameObject)
        {
            //make unique here and push that back
            pm_PeachNodes.push_back(fp_PeachGameObject);
        }

        //void RemoveNode(const string& fp_NodeName) 
        // {
        //    pm_PeachNodes.erase(remove_if(pm_PeachNodes.begin(), pm_PeachNodes.end(),
        //        [&fp_NodeName](shared_ptr<PeachNode> L_PeachGameObject) {
        //            return L_PeachGameObject->name == fp_NodeName;
        //        }), pm_PeachNodes.end());
        //}

        void 
            Update(float fp_TimeSinceLastFrame) 
        {
            if (pm_IsPaused) return;

            for (auto& node : pm_PeachNodes) 
            {
                //node->Update(fp_TimeSinceLastFrame);
            }
        }

        void Pause() { pm_IsPaused = true; }
        void Resume() { pm_IsPaused = false; }

        string GetName() const { return pm_Name; }

    private:
        string pm_Name; // >w< rawr
        bool pm_IsPaused = false; //stops all PeachNodes that are pausable
        vector<shared_ptr<PeachNode>> pm_PeachNodes; //use raw pointers for local function bound operations on PeachNodes via SceneTreeManager

    };
}