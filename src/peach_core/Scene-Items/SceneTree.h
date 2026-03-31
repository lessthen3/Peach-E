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

//this struct owns the every node in the scene and handles all responsibilities regarding management of nodes, controlled via the GameManager

///PeachCore
#include "PeachNode.h"
#include "../Utils/Serializer.h"
#include "../Utils/ChunkedPool.h"

#include "UI/PeachConsole.h"

///STL
#include <queue>

namespace PeachCore {

    struct SceneTree 
    {
    public:
        SceneTree() = default;
        ~SceneTree() = default;

    private:
        string pm_SceneName; // >w< rawr
        bool pm_IsPaused = false; //stops all PeachNodes that are pausable

        uint64_t pm_NextAvailableID = 1; //peach node id's are only scene context sensitive, if the scene changes then previously used id's can be used again since nodes are tied to the scenetree instance

        // Node Name : Node
        vector<unique_ptr<PeachNode>> pm_PeachNodes; //use raw pointers for local function bound operations on PeachNodes via SceneTreeManager

        //vector<unique_ptr<PeachNode>>


        unordered_map<string, PeachNodeID> pm_StringToNodeID; //used for when node names are changed by user, to help between the engine ID tracking and the user identifying the node

        queue<uint64_t> pm_PeachNodesQueuedForRemoval; //this holds the lower 56 bits only since the index is all we care ab in the vector uwu

        shared_ptr<Logger> scene_logger = nullptr;

        // ObjectID : SceneTreeItem : Associated Update Package, used for updating all relevant data at the same time
        //map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllCurrentlyActivePeachNodes;

    public:
        PEACH_STATUS_CODE
            InitializeNewScene
            (
                const vector<uint8_t>& fp_SceneBinaryData, 
                shared_ptr<Logger> fp_Logger
            )
        {
            if (not fp_Logger)
            {
                return PEACH_ERROR_NULLPTR_REF_PASSED;
            }

            scene_logger = fp_Logger;

            size_t f_BinaryPointer = 0;
            bool f_BinaryIsValid = true;

            while (f_BinaryIsValid)
            {
                switch (fp_SceneBinaryData[f_BinaryPointer])
                {

                default:
                    scene_logger->Fatal("FATAL ERROR WHILE TRYING TO CONSTRUCT SCENE, invalid binary data was generated probably not your fault mb >w<", pm_SceneName);
                    f_BinaryIsValid = false;
                    break;
                }
            }

            return PEACH_OK;
        }

        [[nodiscard]] PEACH_STATUS_CODE
            QueueNodeForRemoval(const string& fp_NodeName)
        {
            auto f_NodeIterator = pm_StringToNodeID.find(fp_NodeName);

            if (f_NodeIterator == pm_StringToNodeID.end())
            {
                return PEACH_ERROR_INVALID_NODE_REMOVAL_NAME;
            }
            else
            {
                pm_PeachNodesQueuedForRemoval.push(GetNodeIndex(f_NodeIterator->second));
            }

            return PEACH_OK;
        }

        [[nodiscard]] PEACH_STATUS_CODE
            QueueNodeForRemoval(const PeachNodeID fp_NodeID)
        {
            uint64_t f_IndexBits = GetNodeIndex(fp_NodeID);

            if (f_IndexBits >= pm_PeachNodes.size()) //check for bounds since the user passes this
            {
                scene_logger->Error(fmt::format("Attempted to remove a peach node with invalid ID: {}, Node ID is out of bounds oof", fp_NodeID), "SceneTree"); //print node id for identification however the index bits are the relevant part aka the high 56 bits uwu
                return PEACH_ERROR_INVALID_NODE_REMOVAL_ID;
            }

            PeachNode* f_NodeReference = pm_PeachNodes[f_IndexBits].get();

            if (not f_NodeReference) //not sure ab this one since the recursive removal function already always checks nulls 
            {
                scene_logger->Error(fmt::format("Attempted to remove a peach node with invalid ID: {}, Tried to reference stale node reference that is set -> NULL", fp_NodeID), "SceneTree");
                return PEACH_ERROR_INVALID_NODE_REMOVAL_ID;
            }
            else if (f_NodeReference->ID != fp_NodeID)
            {
                scene_logger->Error(fmt::format("Attempted to remove a peach node with invalid ID: {}, Node ID was improperly indexed at creation due to internal engine error PLEASE REPORT THIS BUG", fp_NodeID), "SceneTree");
                return PEACH_ERROR_INVALID_NODE_REMOVAL_ID;
            }

            pm_PeachNodesQueuedForRemoval.push(f_IndexBits);

            return PEACH_OK;
        }

        [[nodiscard]] PEACH_STATUS_CODE
            ReparentPeachNode //returns true if operation was successful, returns false otherwise
            (
                const PeachNodeID fp_OriginalParent,
                const PeachNodeID fp_NewParent,
                const PeachNodeID fp_ChildNode
            )
        {
            //uint64_t 

            return PEACH_OK;
        }

        PeachNodeID
            DuplicateNode(const PeachNodeID fp_DesiredNode)
        {

        }

        PEACH_STATUS_CODE
            GetTreeString(PeachNodeID fp_DesiredNode, string& fp_StringContainer); //returns entire tree from parent node as a string

        PEACH_STATUS_CODE
            GetPathInTreeString(PeachNodeID fp_DesiredNode, string& fp_StringContainer); //returns the path string relative scene root/

        PEACH_STATUS_CODE
            GetRelativePathString(PeachNodeID fp_ParentNode, PeachNodeID fp_DesiredNode, string& fp_StringContainer); //returns the path string relative to a specific node

        PEACH_STATUS_CODE
            IsNodeInTree(PeachNodeID fp_DesiredNode)
        {

            return PEACH_OK;
        }

        bool
            RenameNode
            (
                const PeachNodeID fp_DesiredNode, 
                const string& fp_NodeName
            )
        {

            return true;
        }

        void 
            Pause()
            noexcept
        { 
            pm_IsPaused = true;
        }

        void 
            Resume() 
            noexcept
        {
            pm_IsPaused = false; 
        }

        [[nodiscard]] string 
            GetName() 
            const noexcept
        { 
            return pm_SceneName;
        }

        void
            CleanSceneTree()
        {
            while (not pm_PeachNodesQueuedForRemoval.empty())
            {
                PeachNodeID f_NodeID = pm_PeachNodesQueuedForRemoval.front(); //Only index bits are pushed into the removal queue uwu
                pm_PeachNodesQueuedForRemoval.pop();

                RemoveEntireTree(std::move(pm_PeachNodes[f_NodeID]));
            }
        }

        void
            AddNode(unique_ptr<PeachNode>&& fp_PeachGameObject)
        {
            //make unique here and push that back
            //pm_PeachNodes.push_back(fp_PeachGameObject);
        }

    private:
        bool
            RemoveEntireTree(unique_ptr<PeachNode>&& fp_ParentNode) //can do a ownership transfer since the node is gonna be destroyed anyways uwu
        {
            if (not fp_ParentNode)
            {
                scene_logger->Error("Attempted to remove a peach node that is currently invalid and tried to reference stale node reference that is set -> NULL", "SceneTree");
                return false;
            }

            //recursion base case is that when the lowest level of the "tree" is hit it will just skip the for loop and recurse back up uwu
            //don't need to remove ID's for children from parent node since the entire tree is being removed if being called by this method

            for (const PeachNodeID lv_ChildNodeID : fp_ParentNode->GetChildren()) //dont need to perform bounds checks since the nodeid can only be added if it satisfies the vector bounds at creation uwu
            {
                RemoveEntireTree(std::move(pm_PeachNodes[GetNodeIndex(lv_ChildNodeID)]));//needa get index bits again since the child nodes are full ID's and not the lower 56 bits uwu
            }

            pm_StringToNodeID.erase(fp_ParentNode->m_PeachName); //remove node from string lookup
            //fp_ParentNode.reset(); //reset actual node, the slot remains as a dead space since it's not really worth it to refill the slot since generation would be needed and just adding more to the list is better since even w a million nodes it'll only be 8MB on the heap uwu

            return true; //destructor will hit and destroy entire node no need to call reest() here uwu
        }
    };
}
