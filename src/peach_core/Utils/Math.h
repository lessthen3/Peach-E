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


namespace PeachCore::Math {

    [[nodiscard]] static inline float 
        Lerp(float t, float a, float b) 
    {
        return a + t * (b - a);
    }

}