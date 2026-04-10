/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
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
#include "Utils/Serializer.h"
#include "Utils/ChunkedPool.h"

#include "UI/PeachConsole.h"

///STL
#include <queue>
#include <variant>

/// Future STL?
#include <plf_colony/plf_colony.h>

namespace PeachCore {

    using SceneNodeStorage = std::tuple<
        plf::colony<RenderNode2D>,
        plf::colony<RenderNode3D>,
        plf::colony<PhysicsNode2D>,
        plf::colony<PhysicsNode3D>,
        plf::colony<AudioNode2D>,
        plf::colony<AudioNode3D>,
        plf::colony<UtilityNode>,
        plf::colony<InterfaceNode>
    > ;

    struct PeachNodeRecordKeeper
    {
        uint32_t Render2D = 0;
        uint32_t Render3D = 0;

        uint32_t Physics2D = 0;
        uint32_t Physics3D = 0;

        uint32_t Audio2D = 0;
        uint32_t Audio3D = 0;

        uint32_t UtilityNode = 0;
        uint32_t InterfaceNode = 0;
    };

    struct SceneTree 
    {
    public:
        SceneTree() = default;
        ~SceneTree() = default;

    private:
        string pm_SceneName; // >w< rawr
        bool pm_IsPaused = false; //stops all PeachNodes that are pausable

        SceneNodeStorage pm_SceneNodes;

        //idk for hash collisions we need a system of adding numbers like godot seems like the easiest thing to do owo
        unordered_map<string, PEACH_NodeID> pm_StringToNodeID; //used for when node names are changed by user, to help between the engine ID tracking and the user identifying the node

        queue<PEACH_NodeID> pm_PeachNodesQueuedForRemoval; //this holds the lower 56 bits only since the index is all we care ab in the vector uwu

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
                pm_PeachNodesQueuedForRemoval.push(f_NodeIterator->second);
            }

            return PEACH_OK;
        }

        [[nodiscard]] PEACH_STATUS_CODE
            QueueNodeForRemoval(const PEACH_NodeID fp_NodeID)
        {
            //if (fp_NodeID.Index >= pm_SceneNodes[fp_NodeID.Type]) //check for bounds since the user passes this
            //{
            //    scene_logger->Error(fmt::format("Attempted to remove a peach node with invalid ID: {}, Node ID is out of bounds oof", fp_NodeID), "SceneTree"); //print node id for identification however the index bits are the relevant part aka the high 56 bits uwu
            //    return PEACH_ERROR_INVALID_NODE_REMOVAL_ID;
            //}

            //PeachNode* f_NodeReference = pm_SceneNodes[fp_NodeID.Type];

            //if (not f_NodeReference) //not sure ab this one since the recursive removal function already always checks nulls 
            //{
            //    scene_logger->Error(fmt::format("Attempted to remove a peach node with invalid ID: {}, Tried to reference stale node reference that is set -> NULL", fp_NodeID), "SceneTree");
            //    return PEACH_ERROR_INVALID_NODE_REMOVAL_ID;
            //}
            //else if (f_NodeReference->ID != fp_NodeID)
            //{
            //    scene_logger->Error(fmt::format("Attempted to remove a peach node with invalid ID: {}, Node ID was improperly indexed at creation due to internal engine error PLEASE REPORT THIS BUG", fp_NodeID), "SceneTree");
            //    return PEACH_ERROR_INVALID_NODE_REMOVAL_ID;
            //}

            //pm_PeachNodesQueuedForRemoval.push(f_IndexBits);

            return PEACH_OK;
        }

        [[nodiscard]] PEACH_STATUS_CODE
            ReparentPeachNode //returns true if operation was successful, returns false otherwise
            (
                const PEACH_NodeID fp_OriginalParent,
                const PEACH_NodeID fp_NewParent,
                const PEACH_NodeID fp_ChildNode
            )
        {
            //uint64_t 

            return PEACH_OK;
        }

        PEACH_NodeID
            DuplicateNode(const PEACH_NodeID fp_DesiredNode)
        {

        }

        PEACH_STATUS_CODE
            GetTreeString(PEACH_NodeID fp_DesiredNode, string& fp_StringContainer); //returns entire tree from parent node as a string

        PEACH_STATUS_CODE
            GetPathInTreeString(PEACH_NodeID fp_DesiredNode, string& fp_StringContainer); //returns the path string relative scene root/

        PEACH_STATUS_CODE
            GetRelativePathString(PEACH_NodeID fp_ParentNode, PEACH_NodeID fp_DesiredNode, string& fp_StringContainer); //returns the path string relative to a specific node

        PEACH_STATUS_CODE
            IsNodeInTree(PEACH_NodeID fp_DesiredNode)
        {

            return PEACH_OK;
        }

        bool
            RenameNode 
            (
                const PEACH_NodeID fp_DesiredNode, 
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
                PEACH_NodeID f_NodeID = pm_PeachNodesQueuedForRemoval.front(); //Only index bits are pushed into the removal queue uwu
                pm_PeachNodesQueuedForRemoval.pop();

                //RemoveEntireTree(std::move(pm_PeachNodes[f_NodeID]));
            }
        }

        //[[nodiscard]] plf::colony<RenderNode2D>::iterator
        //    AddRenderNode2D
        //    (
        //        const string& fp_Name,
        //        const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE,
        //        const uint8_t fp_DrawOrder = 0
        //    )
        //{
        //    uint32_t f_Gen = pm_GenerationCounter++; // or per-colony counter
        //    uint32_t f_Index = static_cast<uint32_t>(pm_RenderNodes2D.size()); // stable after insertion

        //    auto f_It = pm_RenderNodes2D.emplace(fp_Name, f_Index, f_Gen, fp_Flags, fp_DrawOrder);

        //    pm_StringToNodeID[fp_Name] = f_It->m_ID;

        //    return f_It; // stable iterator — this IS your handle
        //}

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

            for (const PEACH_NodeID lv_ChildNodeID : fp_ParentNode->GetChildren()) //dont need to perform bounds checks since the nodeid can only be added if it satisfies the vector bounds at creation uwu
            {
                //RemoveEntireTree(std::move(pm_PeachNodes[GetNodeIndex(lv_ChildNodeID)]));//needa get index bits again since the child nodes are full ID's and not the lower 56 bits uwu
            }

            pm_StringToNodeID.erase(fp_ParentNode->m_PeachName); //remove node from string lookup
            //fp_ParentNode.reset(); //reset actual node, the slot remains as a dead space since it's not really worth it to refill the slot since generation would be needed and just adding more to the list is better since even w a million nodes it'll only be 8MB on the heap uwu

            return true; //destructor will hit and destroy entire node no need to call reest() here uwu
        }
    };
}
