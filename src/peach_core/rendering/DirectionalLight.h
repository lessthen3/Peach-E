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

///cglm
#include <cglm/cglm.h>
#include <cglm/struct.h>

namespace PeachCore {

    struct DirectionalLight2D
    {
    };

}

namespace PeachCore
{
    struct DirectionalLight3D
    {
        vec4s pm_Colour;
        vec3s pm_Direction;
        float pm_Intensity;

        ////////////////////////////////////////////// Constructor //////////////////////////////////////////////
    
        explicit
            DirectionalLight3D
            (
                const vec4s fp_Colour, 
                const vec3s fp_Direction, 
                const float fp_Intensity
            )
            :
            pm_Colour(fp_Colour),
            pm_Direction(fp_Direction),
            pm_Intensity(fp_Intensity)
        {}

        ////////////////////////////////////////////// Setter and Getters //////////////////////////////////////////////

         [[nodiscard]] vec4s 
             GetColour()
             const noexcept
         {
            return pm_Colour;
        }

        void 
            SetColour(const vec4s fp_Colour)
            noexcept
        {
             pm_Colour = fp_Colour;
        }

        [[nodiscard]] vec3s
            GetDirection() 
            const noexcept
        {
            return pm_Direction;
        }

        void 
            SetDirection(const vec3s fp_Direction)
            noexcept
        {
            pm_Direction = fp_Direction;
        }

         [[nodiscard]] float
             GetIntensity()
             const noexcept
         {
            return pm_Intensity;
        }

        void 
            SetIntensity(float fp_Intensity) 
            noexcept
        {
            pm_Intensity = fp_Intensity;
        }
    };
}
