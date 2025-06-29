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

#include "PeachUserInterfaceNode.h"
#include "ShapePrimitives.h"

namespace PeachCore {
namespace PUI {

    struct Panel : public PeachUserInterfaceNode
    {
        Rectangle m_Shape;

        Panel(string fp_DesiredID)
        {
            m_Type = NodeType::Panel;
            m_PeachID = fp_DesiredID;
        }

    };

}// namespace PUI
}// namespace PeachCore