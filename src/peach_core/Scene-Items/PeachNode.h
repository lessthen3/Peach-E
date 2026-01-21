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

    using PeachNodeID = uint64_t; //node id is , high 56 bits is an index and the low 8 bits are the node type uwu

    enum PeachNodeFlags : uint8_t
    {
        PEACH_NODE_HAS_NO_FLAGS = 0,

        PEACH_NODE_IS_PAUSABLE = 1 << 0,
        PEACH_NODE_IS_ACTIVE = 1 << 1,
        PEACH_NODE_IS_VISIBLE = 1 << 2
    };

    class PeachNode
    {
    public:
        virtual ~PeachNode() = default;

        PeachNode(const string& fp_NodeName, PeachNodeID fp_NodeID, const uint8_t fp_Flags) : m_PeachName(fp_NodeName), ID(fp_NodeID), pm_ParentNode(fp_NodeID), Flags(fp_Flags) {}

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

        [[nodiscard]] virtual bool
            HasPeachNode(PeachNodeID fp_DesiredNode)
        {
            if (find(pm_Children.begin(), pm_Children.end(), fp_DesiredNode) == pm_Children.end()) // Check if the element was found
            {
                return false;
            }

            return true;
        }
    };

    class PeachNode2D : public PeachNode
    {
    public:
        virtual ~PeachNode2D() = default;

        PeachNode2D(const string& fp_NodeName, const PeachNodeID fp_NodeID, const uint8_t fp_Flags) : PeachNode(fp_NodeName, fp_NodeID, fp_Flags) {}

        Transform2D m_Transform{};
    };
   

    struct PeachNode3D : public PeachNode
    {
        virtual ~PeachNode3D() = default;

        PeachNode3D(const string& fp_NodeName, const PeachNodeID fp_NodeID, const uint8_t fp_Flags) : PeachNode(fp_NodeName, fp_NodeID, fp_Flags) {}

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

    struct Node : public PeachNode2D
    {
        virtual ~Node() = default;
        Node(const string& fp_NodeName, PeachNodeID fp_NodeID, const uint8_t fp_Flags, const NodeType fp_Type) : PeachNode2D(fp_NodeName, fp_NodeID, fp_Flags), m_Type(fp_Type) {}

        const NodeType m_Type; //default to no type, i like default constructors, they're cool
        bool m_Dirty = true; // needs redraw/layout dirty little kitten >w<
    };

}//namespace PeachCore::PUI