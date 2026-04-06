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

#include "../PeachNode.h"
#include "ShapePrimitives.h"

namespace PeachCore {
namespace PUI {

    struct Slider : public Node
    {
        Rectangle m_Shape;

        Slider
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE,
            const ShapeType fp_ButtonShape = ShapeType::NO_SHAPE
        ) 
            : Node(fp_NodeName, fp_Index, fp_Generation, fp_Flags, NodeType::Panel)
        {
            //>w<!
        }

    };

}// namespace PUI
}// namespace PeachCore