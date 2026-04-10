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

///PeachCore
#include "../PeachNode.h"
#include "ShapePrimitives.h"

namespace PeachCore::PUI {

    struct Label : public Node //Just a generic text label
    {
        ShapePrimitive m_Shape;  // default called in constructor as rectangular

        explicit
            Label
            (
                const uint32_t fp_Index,
                const uint32_t fp_Generation,
                const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE,
                ShapePrimitive fp_Shape = RectShape{}  // pass any shape directly
            )
            :
            Node(fp_Index, fp_Generation, fp_Flags, NodeType::Label)
        {}

    };

}// namespace PeachCore::PUI