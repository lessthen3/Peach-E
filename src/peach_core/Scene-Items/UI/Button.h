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

#include "Scene-Items/PeachNode.h"
#include "ShapePrimitives.h"

namespace PeachCore::PUI {

    struct Button final : public Node
    {
        /* 
            Inline; no heap allocation, no pointer chasing; ShapePrimitive is 40 bytes. The whole Button lives in the heap alloc'd plf::colony block uwu

            this is better for cache locality and avoids excessive dereferncing which should make things easier on the mmu, significantly speeding up ui operations.

            no vtables and type checking failures w bad dynamic_casts, just bad variant access errors uwu!

            the call site should be the one to decide whether this type lives on the heap or stack, more flexible and gives more choice to the caller. wanna keep this the least opionated it can be.
            the only cost is that because its a union there will be gaps in memory, but like the OS allocator is always gonna have those so whatever the types are relatively within the same size owo
         */ 
        ShapePrimitive m_Shape; //default called in constructor owo

        explicit
            Button
        (
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE,
            ShapePrimitive fp_Shape = RectShape{}  // pass any shape directly
        )
            : 
            Node(fp_Index, fp_Generation, fp_Flags, NodeType::Button)
        {}

        // Hit test — just thread through to the free function.
       // The Node's position is whatever your layout system puts in here.
        [[nodiscard]] bool
            IsWithin(const vec2s fp_WorldOrigin, const vec2s fp_TestPoint) //compiler will inline the inner function, but the weight is too heavy for the entire function to be inlined tbh
            const noexcept
        {
            return PUI::IsWithin(m_Shape, fp_WorldOrigin, fp_TestPoint);
        }
    };
}// namespace PeachCore