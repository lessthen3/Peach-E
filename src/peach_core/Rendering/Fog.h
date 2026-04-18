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

///PeachCore
#include <glm/glm.hpp>

namespace PeachCore {

    struct Fog2D
    {
    };

}

namespace PeachCore {

    struct Fog3D
    {
         bool pm_IsActive = true;
         glm::vec4 pm_Colour;
         float pm_Density = 1;

        ////////////////////////////////////////////// Constructor //////////////////////////////////////////////

         Fog3D() = default;

         Fog3D(const bool fp_IsActive, const glm::vec4& fp_Colour, const float fp_Density)
         {
             pm_IsActive = fp_IsActive;
             pm_Colour = fp_Colour;
             pm_Density = fp_Density;
        }

        ////////////////////////////////////////////// Setter and Getters //////////////////////////////////////////////

         [[nodiscard]] bool
             IsActive()
             const noexcept
         {
            return pm_IsActive;
        }

         void 
             SetActive(bool fp_IsActive)
         {
             pm_IsActive = fp_IsActive;
        }

         [[nodiscard]] glm::vec4
             GetColour() 
             const noexcept
         {
            return pm_Colour;
        }

         void 
             SetColour(const glm::vec4& fp_Colour)
         {
             pm_Colour = fp_Colour;
        }

         [[nodiscard]] float 
             GetDensity() 
             const noexcept
         {
            return pm_Density;
        }

         void 
             SetDensity(const float fp_Density)
         {
             pm_Density = fp_Density;
        }

    };
}

