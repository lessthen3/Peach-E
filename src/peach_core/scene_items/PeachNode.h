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

///STL
#include <string>
#include <vector>
#include <algorithm>

///PeachCore
#include "peach_api/StatusCodes.h"
#include "peach_api/NodeDef.h"

#include "math/DoubleBuffered.h"

namespace PeachCore {

    struct PeachNode
    {
        virtual ~PeachNode() = default;

        explicit
            PeachNode
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeType fp_Type,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE
        )
            :
            m_ID(fp_Index, fp_Generation, fp_Type),
            m_Flags(fp_Flags),
            m_ParentNode(PEACH_NODE_NULL_ID)
        {}

        std::string m_PeachName; //used to identify node uniquely, is a string so that users can just type: "node.remove("myNodeName")"

        const PEACH_NodeID m_ID; //can only be set at object creation uwu
        PEACH_NodeFlags m_Flags;
        PEACH_NodeID m_ParentNode; //default initialized to it's own ID so that if a node is its own parent we know its a loner (;w;)

    protected:
        std::vector<PEACH_NodeID> pm_Children; //don't need to explicitly clean this up since the destructors will clean up everything properly when the owning node exits scope

    public:

        [[nodiscard]] virtual const std::vector<PEACH_NodeID>&
            GetChildren()
            const noexcept
        {
            return pm_Children;
        }

        virtual void
            AddChild(PEACH_NodeID fp_Child)
        {
            pm_Children.push_back(fp_Child);
        }

        [[nodiscard]] virtual bool
            RemoveChild(PEACH_NodeID fp_Child)
        {
            size_t f_OldSize = pm_Children.size();
            std::erase(pm_Children, fp_Child);
            return pm_Children.size() < f_OldSize;
        }

        [[nodiscard]] virtual inline bool
            HasPeachNode(PEACH_NodeID fp_DesiredNode)
        {
            return std::find(pm_Children.begin(), pm_Children.end(), fp_DesiredNode) == pm_Children.end(); // Check if the element was found
        }
    };

    /*
        double buffering transform to avoid lock and block overhead, individual primitives are hardware atomic however split across components not so much.

        this is because register writes across the memory bus are indivisible per the standard set out and agreed on by Intel, AMD, and ARM has its own. 

        so when an individual 64 bit value or whatever fits in a register is written and its guaranteed not to be like 4 bytes corrupted or whatever, however for larger objects each individual operation
        is split across multiple reg operations for example the 3 components of a glm::vec3, so for a transform we elect to double buffer and give the index being written so the reading thread can know
        which transform is valid for reading without half mangled data. similiar to how a back buffer works w screen pixel buffers.

        don't really needa swap buffers here tho, can just do some array ptr math and point to a different version since this is not a hardware buffer uwu owo

        for PS Vita its ARMv7 and is a 32 bit system, it handles 64 bit values across two different 32 bit registers and can only guarantee atomic hardware safety for 32 bit values only
    */

    struct RenderNode2D final : public PeachNode
    {
        virtual ~RenderNode2D() = default;

        RenderNode2D
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE,
            const uint8_t fp_DrawOrder = 0
        ) 
            : 
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_RENDER_2D, fp_Flags),
            DrawGroup(fp_DrawOrder)
        {}

        DoubleBuffered::Transform2D m_Transform;
        uint32_t DrawGroup; //can't imagine there'll be more than 256 drawing layers, at that point integer overflow is the least of ur worries lmfao, whatever uint32_t ig
        DoubleBuffered::UVs m_UVs;
    };

    struct RenderNode3D final : public PeachNode
    {
        virtual ~RenderNode3D() = default;

        RenderNode3D
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE
        )
            :
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_RENDER_3D, fp_Flags)
        {}

        DoubleBuffered::Transform3D m_Transform;
    };

    struct AudioNode2D final : public PeachNode
    {
        virtual ~AudioNode2D() = default;

        AudioNode2D
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE
        )
            :
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_AUDIO_2D, fp_Flags)
        {}

        DoubleBuffered::Transform2D m_Transform;
        //AudioID AudioHandle;
        double Volume = 0.0f;
        bool IsLooping{ false };
    };

    struct AudioNode3D final : public PeachNode
    {
        virtual ~AudioNode3D() = default;

        AudioNode3D
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE
        )
            :
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_AUDIO_3D, fp_Flags)
        {}

        DoubleBuffered::Transform3D m_Transform;
        uint64_t AudioHandle = 0;
        double Volume = 0.0f;
        bool IsLooping{ false };
    };

    struct InterfaceNode : public PeachNode
    {
        virtual ~InterfaceNode() = default;

        InterfaceNode
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE
        )
            :
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_INTERFACE, fp_Flags)
        {}

        DoubleBuffered::Transform2D m_Transform;
        bool m_Dirty = true; // needs redraw/layout dirty little kitten >w<
    };

    struct PhysicsNode2D final : public PeachNode
    {
        virtual ~PhysicsNode2D() = default;

        PhysicsNode2D
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE
        )
            :
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_PHYSICS_2D, fp_Flags)
        {}

        DoubleBuffered::Transform2D m_Transform;
    };

    struct PhysicsNode3D final : public PeachNode
    {
        virtual ~PhysicsNode3D() = default;

        PhysicsNode3D
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE
        )
            :
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_PHYSICS_3D, fp_Flags)
        {}

        DoubleBuffered::Transform3D m_Transform;
    };

    struct UtilityNode final : public PeachNode
    {
        virtual ~UtilityNode() = default;

        UtilityNode
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE
        )
            :
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_UTILITY, fp_Flags)
        {}
    };
}//namespace PeachCore

namespace PeachCore::PUI {

    struct TextStyle
    {
        float CurrentFontSize = 0.0f;
        std::string TypefaceName;
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
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE,
            const NodeType fp_InterfaceType = NodeType::None
        )
            : 
            PeachNode(fp_Index, fp_Generation, PEACH_TYPE_INTERFACE, fp_Flags),
            m_Type(fp_InterfaceType)
        {}

        const NodeType m_Type; //default to no type, i like default constructors, they're cool
    };

}//namespace PeachCore::PUI