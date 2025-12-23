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

///PeachCore
#include "../Managers/StatusCodes.h"

namespace PeachCore {

    using namespace std; //this should be here so i dont affect anybody who links against peach

    using PeachNodeID = uint64_t; //node id is , high 56 bits is an index and the low 8 bits are the node type uwu

    constexpr uint64_t PEACH_NODE_TYPE_MASK = 0xFFull;        // lower 8 bits
    constexpr uint64_t PEACH_NODE_INDEX_MASK = ~PEACH_NODE_TYPE_MASK;
    constexpr uint64_t PEACH_NODE_TYPE_BITS = 8U;

    constexpr uint64_t PEACH_NODE_NULL_ID = 0; //Represents no ID, or invalid ID

    static inline PeachNodeID
        MakeNodeID(uint64_t fp_Index, uint8_t fp_NodeType)
        noexcept
    {
        // index goes into the upper 56 bits, type in lower 8
        return (fp_Index << PEACH_NODE_TYPE_BITS) | static_cast<uint64_t>(fp_NodeType);
    }

    static inline uint8_t
        GetNodeType(PeachNodeID fp_NodeID)
        noexcept
    {
        return static_cast<uint8_t>(fp_NodeID & PEACH_NODE_TYPE_MASK);
    }

    static inline uint64_t
        GetNodeIndex(PeachNodeID fp_NodeID)
        noexcept
    {
        return fp_NodeID >> PEACH_NODE_TYPE_BITS;
    }

    enum PeachNodeType : uint8_t
    {
        Blank,
        Render,
        Physics,
        Audio
    };

    class PeachNode
    {
    public:
        virtual ~PeachNode();

        PeachNode(const string& fp_Name) 
        {
            m_Name = fp_Name;
        }

        PeachNode() {} //??????????????????

        bool IsPausable = true;
        bool IsVisible = true;
        bool IsActive = true;

        string m_Name; //this is the name set by the user inside the scene tree, so that a user script can call smth like GetNode("MyNamedNode") and find the appropriate thing

        PeachNodeID ID = PEACH_NODE_NULL_ID;
        PeachNodeID pm_ParentNode = PEACH_NODE_NULL_ID; //this is fine since te parent handle should never be null because the lifetime of the child is strictly tied to its parent

        vector<PeachNodeID> pm_Children; //don't need to explicitly clean this up since the destructors will clean up everything properly when the owning node exits scope

        void
            AddChild(PeachNodeID fp_Child)
        {
            pm_Children.push_back(fp_Child);
        }


        PeachNode* 
            GetPeachNode(); //returns a reference to the desired PeachNode

        bool 
            ReparentPeachNode(); //returns true if operation was successful, returns false otherwise

        void
            PrintTree();

        bool 
            IsInsideTree();

        [[nodiscard]] bool 
            HasPeachNode();

        void 
            GetViewPort(); //?????????? why does godot have this lmfao

        string
            GetPathInTree();

        PeachNode*
            FindChild(uint64_t fp_DesiredPeachNode); //returns first instance of child found matching the name

        PeachNode 
            Duplicate();
    };

}