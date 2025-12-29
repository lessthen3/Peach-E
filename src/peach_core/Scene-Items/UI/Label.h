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

    struct Label : public PeachUserInterfaceNode //Just a generic text label
    {
        Rectangle m_Shape;

        Label(string fp_DesiredID)
        {
            m_Type = NodeType::Label;
            m_PeachID = fp_DesiredID;
        }

    };

}// namespace PUI
}// namespace PeachCore