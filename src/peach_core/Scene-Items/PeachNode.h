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

///STL
#include <string>
#include <vector>
#include <memory>

///PeachCore
#include "../peach_api/StatusCodes.h"
#include "../Utils/Logger.h"
#include "Transform.h"

namespace PeachCore {

    constexpr uint64_t PEACH_NODE_TYPE_MASK = 0xFFull;        // lower 8 bits
    constexpr uint64_t PEACH_NODE_INDEX_MASK = ~PEACH_NODE_TYPE_MASK;
    constexpr uint64_t PEACH_NODE_TYPE_BITS = 8U;

    constexpr uint64_t PEACH_NODE_NULL_ID = 0; //Represents no ID, or invalid ID

    using PeachNodeID = uint64_t; //node id is , high 56 bits is an index and the low 8 bits are the node type uwu

    enum PeachNodeFlags : uint8_t
    {
        PEACH_NODE_HAS_NO_FLAGS = 0,

        PEACH_NODE_IS_PAUSABLE = 1 << 0,
        PEACH_NODE_IS_ACTIVE = 1 << 1,
        PEACH_NODE_IS_VISIBLE = 1 << 2,
        PEACH_NODE_IS_QUEUED_FOR_REMOVAL = 1 << 3
    };

    [[nodiscard]] static inline PeachNodeID
        MintNewNodeID(uint64_t fp_Index, uint8_t fp_NodeType)
        noexcept
    {
        return (fp_Index << PEACH_NODE_TYPE_BITS) | static_cast<uint64_t>(fp_NodeType); // index goes into the upper 56 bits, type in lower 8
    }

    [[nodiscard]] static inline uint8_t
        GetNodeType(PeachNodeID fp_NodeID)
        noexcept
    {
        return static_cast<uint8_t>(fp_NodeID & PEACH_NODE_TYPE_MASK);
    }

    [[nodiscard]] static inline uint64_t
        GetNodeIndex(PeachNodeID fp_NodeID)
        noexcept
    {
        return fp_NodeID >> PEACH_NODE_TYPE_BITS;
    }

    enum PeachNodeType : uint8_t // this doesnt really need to be a bit mask tbh idk
    {
        Blank = 0,
        Render2D = 1 << 0,
        Render3D = 1 << 1,
        Physics = 1 << 2,
        Audio = 1 << 3
    };

}

namespace PeachCore {

    class PeachNode
    {
    public:
        virtual ~PeachNode() = default;

        PeachNode
        (
            const string& fp_NodeName, 
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS
        )
            :
            m_PeachName(fp_NodeName), 
            ID(fp_NodeID), 
            pm_ParentNode(fp_ParentNodeID),
            Flags(fp_Flags)
        {}

        string m_PeachName; //used to identify node uniquely, is a string so that users can just type: "node.remove("myNodeName")"

        uint8_t Flags;

        const PeachNodeID ID; //can only be set at object creation uwu
        PeachNodeID pm_ParentNode; //default initialized to it's own ID so that if a node is its own parent we know its a loner (;w;)

    protected:
        vector<PeachNodeID> pm_Children; //don't need to explicitly clean this up since the destructors will clean up everything properly when the owning node exits scope

    public:

        [[nodiscard]] virtual const vector<PeachNodeID>&
            GetChildren()
            const noexcept
        {
            return pm_Children;
        }

        virtual void
            AddChild(PeachNodeID fp_Child)
        {
            pm_Children.push_back(fp_Child);
        }

        [[nodiscard]] virtual bool
            RemoveChild(PeachNodeID fp_Child)
        {
            auto f_ChildIterator = find(pm_Children.begin(), pm_Children.end(), fp_Child); // Find Child ID

            if (f_ChildIterator == pm_Children.end()) // Check if the element was found
            {
                return false;
            }

            pm_Children.erase(f_ChildIterator);

            return true;
        }

        [[nodiscard]] virtual inline bool
            HasPeachNode(PeachNodeID fp_DesiredNode)
        {
            return find(pm_Children.begin(), pm_Children.end(), fp_DesiredNode) == pm_Children.end(); // Check if the element was found
        }
    };

    struct RenderNode2D final : public PeachNode
    {
        virtual ~RenderNode2D() = default;

        RenderNode2D
        (
            const string& fp_NodeName,
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS,
            const uint8_t fp_DrawOrder = 0,
            const uint64_t fp_MaterialHandle = 0,
            const tuple<float, float, float, float> fp_UVs = { 0.0f, 0.0f, 0.0f, 0.0f }
        ) 
            : 
            PeachNode(fp_NodeName, fp_NodeID, fp_ParentNodeID, fp_Flags),
            DrawGroup(fp_DrawOrder),
            MaterialHandle(fp_MaterialHandle),
            UVs(fp_UVs)
        {}

        Transform2D Transform;

        uint8_t DrawGroup; //can't imagine there'll be more than 256 drawing layers, at that point integer overflow is the least of ur worries lmfao

        //TODO: fix this, need to just hold handles and metadata that maps -> descriptor sets, pipeline info
        uint64_t MaterialHandle; //actual data for graphic //used for parsing raw byte information, mainly for audio at the moment

        tuple<float, float, float, float> UVs;

    };

    struct RenderNode3D final : public PeachNode
    {
        virtual ~RenderNode3D() = default;

        RenderNode3D
        (
            const string& fp_NodeName,
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS
        )
            :
            PeachNode(fp_NodeName, fp_NodeID, fp_ParentNodeID, fp_Flags)
        {}

        Transform3D Transform;
    };

    struct AudioNode final : public PeachNode
    {
        virtual ~AudioNode() = default;

        AudioNode
        (
            const string& fp_NodeName,
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS
        )
            :
            PeachNode(fp_NodeName, fp_NodeID, fp_ParentNodeID, fp_Flags)
        {}

        Transform2D Transform;
        uint64_t AudioHandle = 0;
        double Volume = 0.0f;
        bool IsLooping{ false };
    };

    struct InterfaceNode : public PeachNode
    {
        virtual ~InterfaceNode() = default;

        InterfaceNode
        (
            const string& fp_NodeName,
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS
        )
            :
            PeachNode(fp_NodeName, fp_NodeID, fp_ParentNodeID, fp_Flags)
        {}

        Transform2D Transform;
    };

    struct PhysicsNode final : public PeachNode
    {
        virtual ~PhysicsNode() = default;

        PhysicsNode
        (
            const string& fp_NodeName,
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS
        )
            :
            PeachNode(fp_NodeName, fp_NodeID, fp_ParentNodeID, fp_Flags)
        {}
    };

    class PeachNode2D : public PeachNode
    {
    public:
        virtual ~PeachNode2D() = default;

        PeachNode2D
        (
            const string& fp_NodeName,
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS
        )
            :
            PeachNode(fp_NodeName, fp_NodeID, fp_ParentNodeID, fp_Flags)
        {}

        Transform2D m_Transform{};
    };


    struct PeachNode3D : public PeachNode
    {
        virtual ~PeachNode3D() = default;

        PeachNode3D
        (
            const string& fp_NodeName,
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS
        )
            :
            PeachNode(fp_NodeName, fp_NodeID, fp_ParentNodeID, fp_Flags)
        {}

        Transform3D m_Transform{};
    };
}//namespace PeachCore

namespace PeachCore::PUI {

    struct TextStyle
    {
        float CurrentFontSize = 0.0f;
        string TypefaceName;
        bool IsMonoSpaced = true;
    };

    enum class NodeType
    {
        None,
        Root,
        Window,
        Button,
        Label,
        Image,
        Panel,
        ScrollArea,
        MenuBar,
        MenuItem,
        Popup,
        Slider,

        Console // idfk >w<
    };

    struct Node : public PeachNode
    {
        virtual ~Node() = default;

        Node
        (
            const string& fp_NodeName,
            const PeachNodeID fp_NodeID,
            const PeachNodeID fp_ParentNodeID = PEACH_NODE_NULL_ID,
            const uint8_t fp_Flags = PEACH_NODE_HAS_NO_FLAGS,
            const NodeType fp_Type = NodeType::None
        )
            : 
            PeachNode(fp_NodeName, fp_NodeID, fp_ParentNodeID, fp_Flags), 
            m_Type(fp_Type)
        {}

        const NodeType m_Type; //default to no type, i like default constructors, they're cool
        bool m_Dirty = true; // needs redraw/layout dirty little kitten >w<
    };

}//namespace PeachCore::PUI