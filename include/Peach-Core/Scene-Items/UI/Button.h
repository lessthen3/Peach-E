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

    struct Button final : public PeachUserInterfaceNode
    {
        unique_ptr<Shape> m_Shape = nullptr;

        Button(const string& fp_DesiredID, const ShapeType fp_ButtonShape)
        {
            m_Type = NodeType::Button;
            m_PeachID = fp_DesiredID;

            switch(fp_ButtonShape)
            {
                case ShapeType::Rectangle:
                    m_Shape = make_unique<Rectangle>();
                    break;
                case ShapeType::Circle:
                    m_Shape = make_unique<Circle>();
                    break;
                case ShapeType::Ellipse:
                    m_Shape = make_unique<Ellipse>();
                    break;
                case ShapeType::Capsule:
                    m_Shape = make_unique<Capsule>();
                    break;
                case ShapeType::Triangle:
                    m_Shape = make_unique<Triangle>();
                    break;
                default:
                    //handle error here later idfk
                    break;
            }
        }

    };


}// namespace PUI
}// namespace PeachCore