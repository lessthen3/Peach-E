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

#include <glm/glm.hpp>

namespace PeachCore {

    struct DirectionalLight2D
    {
    };

}

namespace PeachCore
{
    struct DirectionalLight3D
    {
        glm::vec4 pm_Colour;
        glm::vec3 pm_Direction;
        float pm_Intensity;

        ////////////////////////////////////////////// Constructor //////////////////////////////////////////////

        DirectionalLight3D() = default;
    
        DirectionalLight3D(const glm::vec4& fp_Colour, const glm::vec3& fp_Direction, const float fp_Intensity)
         {
            pm_Colour = fp_Colour;
            pm_Direction = fp_Direction;
            pm_Intensity = fp_Intensity;
        }

        ////////////////////////////////////////////// Setter and Getters //////////////////////////////////////////////

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

         [[nodiscard]] glm::vec3
             GetDirection() 
             const noexcept
         {
            return pm_Direction;
        }

         void 
             SetDirection(const glm::vec3& fp_Direction)
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
             SetIntensity(float intensity) 
         {
            intensity = intensity;
        }
    };
}
