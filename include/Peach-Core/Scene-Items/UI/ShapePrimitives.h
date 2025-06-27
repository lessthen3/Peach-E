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

#include <vector>


namespace PeachCore{
namespace PUI{

    struct Rectangle //(x, y) dictates top left corner, width and height dictate how far the bottom right vert is extended
    {
        float PosX  = 0.0f;
        float PosY = 0.0f;
        float Width = 0.0f; 
        float Height = 0.0f;

        inline bool
            IsWithinRectangle(const float fp_X, const float fp_Y)
            const noexcept
        {
            return
                (
                    (abs(fp_X) > abs(PosX) and abs(fp_X) < (abs(PosX) + abs(Width)))
                    and
                    (abs(fp_Y) > abs(PosY) and abs(fp_Y) < (abs(PosY) + abs(Height)))
                );
        }
    };
    
}// namespace PUI
}// namespace PeachCore