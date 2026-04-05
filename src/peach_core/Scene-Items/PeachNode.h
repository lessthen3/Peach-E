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
#include <memory>

///PeachCore
#include "../peach_api/StatusCodes.h"
#include "../Utils/Logger.h"
#include "Transform.h"

namespace PeachCore {

    enum class PeachNodeFlags : uint8_t //fuck the type system when it comes to flags uwu
    {
        NONE = 0,

        IS_PAUSABLE = 1u << 0,
        IS_ACTIVE = 1u << 1,
        IS_VISIBLE = 1u << 2,
        IS_QUEUED_FOR_REMOVAL = 1u << 3
    };

    inline 
        PeachNodeFlags operator|(PeachNodeFlags fp_FuckCpp, PeachNodeFlags fp_FuckYou) //fuck C++ CoodOEOs MSelLLLSlelS Ss brb ima write C++ like java and be confused why it doesn't work uwu
        noexcept
    {
        return static_cast<PeachNodeFlags>(static_cast<uint8_t>(fp_FuckCpp) | static_cast<uint8_t>(fp_FuckYou));
    }

    inline
        PeachNodeFlags operator&(PeachNodeFlags fp_FuckCpp, PeachNodeFlags fp_FuckYou) //fuck C++ CoodOEOs MSelLLLSlelS Ss brb ima write C++ like java and be confused why it doesn't work uwu
        noexcept
    {
        return static_cast<PeachNodeFlags>(static_cast<uint8_t>(fp_FuckCpp) & static_cast<uint8_t>(fp_FuckYou));
    }

    enum class PeachNodeType : uint8_t // this doesnt really need to be a bit mask tbh idk, fixed it owo
    {
        RENDER_2D,
        RENDER_3D,
        PHYSICS_2D,
        PHYSICS_3D,
        AUDIO_2D,
        AUDIO_3D,
        UTILITY,
        INTERFACE,
        BLANK
    };

    struct PeachNodeID
    {
        uint32_t Index = 0;
        PeachNodeType Type = PeachNodeType::BLANK;

        uint32_t Generation = 0;

        bool operator==(const PeachNodeID&) const = default;
    };

    constexpr PeachNodeID PEACH_NODE_NULL_ID = PeachNodeID{ 0, PeachNodeType::BLANK, 0 }; //Represents no ID, or invalid ID
}

namespace PeachCore {

    class PeachNode
    {
    public:
        virtual ~PeachNode() = default;

        explicit
            PeachNode
        (
            const string& fp_NodeName, 
            const uint32_t fp_Index,
            const PeachNodeType fp_Type,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
        )
            :
            m_PeachName(fp_NodeName), 
            m_ID(fp_Index, fp_Type, fp_Generation),
            m_Flags(fp_Flags)
        {}

        string m_PeachName; //used to identify node uniquely, is a string so that users can just type: "node.remove("myNodeName")"

        const PeachNodeID m_ID; //can only be set at object creation uwu
        PeachNodeFlags m_Flags;
        PeachNodeID m_ParentNode; //default initialized to it's own ID so that if a node is its own parent we know its a loner (;w;)

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
            size_t f_OldSize = pm_Children.size();
            std::erase(pm_Children, fp_Child);
            return pm_Children.size() < f_OldSize;
        }

        [[nodiscard]] virtual inline bool
            HasPeachNode(PeachNodeID fp_DesiredNode)
        {
            return find(pm_Children.begin(), pm_Children.end(), fp_DesiredNode) == pm_Children.end(); // Check if the element was found
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

    struct UVState
    {
        float U1 = 0.0f;
        float V1 = 0.0f;
        float U2 = 1.0f;
        float V2 = 1.0f;
    };

    struct UVDouble
    {
        UVState Slots[2];
        std::atomic<uint32_t> WriteIndex{ 0 };

        [[nodiscard]] UVState&
            GetWriteSlot()
            noexcept
        {
            return Slots[WriteIndex.load(memory_order_relaxed) & 1u];
        }

        [[nodiscard]] const UVState&
            GetReadSlot()
            const noexcept
        {
            return Slots[(WriteIndex.load(memory_order_acquire) + 1u) & 1u];
        }

        void
            CommitWrite()
            noexcept
        {
            WriteIndex.fetch_add(1u, memory_order_release);
        }

        void
            SetFrame(float fp_U1, float fp_V1, float fp_U2, float fp_V2)
            noexcept
        {
            UVState& f_Slot = GetWriteSlot();
            f_Slot.U1 = fp_U1;
            f_Slot.V1 = fp_V1;
            f_Slot.U2 = fp_U2;
            f_Slot.V2 = fp_V2;
        }
    };

    struct TransformDouble2D
    {
        Transform2D Slots[2];
        std::atomic<uint32_t> WriteIndex{ 0 }; // which slot game thread is writing

        // Main thread: get the slot currently being written to
        [[nodiscard]] Transform2D&
            GetWriteSlot()
            noexcept
        {
            return Slots[WriteIndex.load(memory_order_relaxed) & 1u];
        }

        // Render/physics thread: get the slot safe to read
        // Always the slot the main thread is NOT currently writing
        [[nodiscard]] const Transform2D&
            GetReadSlot()
            const noexcept
        {
            return Slots[(WriteIndex.load(memory_order_acquire) + 1u) & 1u];
        }

        // Main thread: call at end of tick AFTER all transform writes for this frame are done
        // release ordering so render thread's acquire in GetReadSlot sees all prior writes
        void
            CommitWrite()
            noexcept
        {
            WriteIndex.fetch_add(1u, memory_order_release);
        }

        // Convenience: write position and dirty the slot in one call, will get inlined anyways
        //void
        //    SetPosition(const glm::vec2& fp_Pos)
        //    noexcept
        //{
        //    GetWriteSlot().SetPosition(fp_Pos);
        //}

        //void
        //    Translate(const glm::vec2& fp_Offset)
        //    noexcept
        //{
        //    GetWriteSlot().Translate(fp_Offset);
        //}

        //void
        //    SetRotation(float fp_Radians)
        //    noexcept
        //{
        //    GetWriteSlot().SetRotation(fp_Radians);
        //}

        //void
        //    SetScale(const glm::vec2& fp_Scale)
        //    noexcept
        //{
        //    GetWriteSlot().SetScale(fp_Scale);
        //}

        [[nodiscard]] const glm::mat4&
            GetReadMatrix()
            const noexcept
        {
            return const_cast<Transform2D&>(GetReadSlot()).GetLocalMatrix(); // const_cast safe here: GetLocalMatrix is logically const, only mutates cached matrix
        }

        [[nodiscard]] glm::vec2
            GetPosition()
            const noexcept
        {
            return GetReadSlot().GetPosition();
        }

        [[nodiscard]] float
            GetRotation()
            const noexcept
        {
            return GetReadSlot().GetRotation();
        }

        [[nodiscard]] glm::vec2
            GetScale()
            const noexcept
        {
            return GetReadSlot().GetScale();
        }
    };

    struct TransformDouble3D
    {
        Transform3D Slots[2];
        std::atomic<uint32_t> WriteIndex{ 0 };
    };

    struct RenderNode2D final : public PeachNode
    {
        virtual ~RenderNode2D() = default;

        RenderNode2D
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE,
            const uint8_t fp_DrawOrder = 0
        ) 
            : 
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::RENDER_2D, fp_Generation, fp_Flags),
            DrawGroup(fp_DrawOrder)
        {}

        TransformDouble2D m_Transform;
        uint32_t DrawGroup; //can't imagine there'll be more than 256 drawing layers, at that point integer overflow is the least of ur worries lmfao, whatever uint32_t ig
        UVDouble m_UVs;
    };

    struct RenderNode3D final : public PeachNode
    {
        virtual ~RenderNode3D() = default;

        RenderNode3D
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
        )
            :
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::RENDER_3D, fp_Generation, fp_Flags)
        {}

        TransformDouble3D m_Transform;
    };

    struct AudioNode2D final : public PeachNode
    {
        virtual ~AudioNode2D() = default;

        AudioNode2D
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
        )
            :
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::AUDIO_2D, fp_Generation, fp_Flags)
        {}

        TransformDouble2D m_Transform;
        //AudioID AudioHandle;
        double Volume = 0.0f;
        bool IsLooping{ false };
    };

    struct AudioNode3D final : public PeachNode
    {
        virtual ~AudioNode3D() = default;

        AudioNode3D
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
        )
            :
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::AUDIO_3D, fp_Generation, fp_Flags)
        {}

        TransformDouble3D m_Transform;
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
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
        )
            :
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::INTERFACE, fp_Generation, fp_Flags)
        {}

        TransformDouble2D m_Transform;
    };

    struct PhysicsNode2D final : public PeachNode
    {
        virtual ~PhysicsNode2D() = default;

        PhysicsNode2D
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
        )
            :
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::PHYSICS_2D, fp_Generation, fp_Flags)
        {}

        TransformDouble2D m_Transform;
    };

    struct PhysicsNode3D final : public PeachNode
    {
        virtual ~PhysicsNode3D() = default;

        PhysicsNode3D
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
        )
            :
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::PHYSICS_3D, fp_Generation, fp_Flags)
        {}

        TransformDouble3D m_Transform;
    };

    struct UtilityNode final : public PeachNode
    {
        virtual ~UtilityNode() = default;

        UtilityNode
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
        )
            :
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::UTILITY, fp_Generation, fp_Flags)
        {}
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
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE,
            const NodeType fp_InterfaceType = NodeType::None
        )
            : 
            PeachNode(fp_NodeName, fp_Index, PeachNodeType::INTERFACE, fp_Generation, fp_Flags),
            m_Type(fp_InterfaceType)
        {}

        const NodeType m_Type; //default to no type, i like default constructors, they're cool
        bool m_Dirty = true; // needs redraw/layout dirty little kitten >w<
    };

}//namespace PeachCore::PUI