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

namespace PeachCore::PUI {

    struct ScrollArea : public Node
    {
        Rectangle m_Shape;

        ScrollArea(const string& fp_NodeName, uint64_t fp_NodeID, const uint8_t fp_Flags, const ShapeType fp_ButtonShape) : Node(fp_NodeName, fp_NodeID, fp_Flags, NodeType::ScrollArea)
        {
            //>O<!
        }

    };

}// namespace PeachCore::PUI