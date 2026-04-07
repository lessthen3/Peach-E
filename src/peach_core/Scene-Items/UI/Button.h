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

    struct Button final : public Node
    {
        unique_ptr<Shape> m_Shape = nullptr;

        Button
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PEACH_NodeFlags fp_Flags = PEACH_FLAGS_NONE,
            const ShapeType fp_ButtonShape = ShapeType::NO_SHAPE
        )
            : 
            Node(fp_NodeName, fp_Index, fp_Generation, fp_Flags, NodeType::Button)
        {
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

}// namespace PeachCore