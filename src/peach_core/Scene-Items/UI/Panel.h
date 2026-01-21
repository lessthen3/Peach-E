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

#include "../PeachNode.h"
#include "ShapePrimitives.h"

namespace PeachCore {
namespace PUI {

    struct Panel : public Node
    {
        Rectangle m_Shape;

        Panel(const string& fp_NodeName, uint64_t fp_NodeID, const uint8_t fp_Flags, const ShapeType fp_ButtonShape) : Node(fp_NodeName, fp_NodeID, fp_Flags, NodeType::Panel)
        {
            //OwO!
        }

    };

}// namespace PUI
}// namespace PeachCore