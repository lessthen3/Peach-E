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

#include "utils/PeachForceInline.h"


namespace PeachCore::Math {

    [[nodiscard]] static PEACH_FORCEINLINE float 
        Lerp(float t, float a, float b) 
    {
        return a + t * (b - a);
    }

    /*
        These LERP functions are used for interpolating sprite positions between physics update frames if the rendering fps is > 60 since physics
        will always update at a constant update interval of 60 times a second, equally spaced apart. This way you'll get "smoother" graphics if u wanna
        crank up the fps uwu



        The idea is that the most recent position will be used, and if the last used position idfk idk if thisll work since i cant predict the next frame, and if i use the current frame data and last frames, then the visuals will be outta sync
        with the current real position which is no good for gameplay, and tryna do predictions like that could be a bad route if the render time oversteps its processing tiime.
    */

    // PEACH_FORCEINLINE float
    //     Lerp(const float fp_Start, const float fp_End, const float fp_Rate)
    //     noexcept
    // {
    //     return fp_Start * (1 - fp_Rate) + fp_End * fp_Rate;
    // }

    // inline const vec2s
    //     Lerp(const vec2s fp_Start, const vec2s fp_End, const vec2s fp_Rate)
    //     const noexcept
    // {
    //     return {{0.0f, 0.0f}};
    // }
}