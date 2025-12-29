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

    struct PointLight2D
    {
    };

}

namespace PeachCore
{
     struct PointLight3D
     {
         struct
         {
             float Constant = 1;
             float Linear = 0;
             float Exponent = 0;
         } Attenuation;

         glm::vec4 pm_Colour;
         glm::vec3 pm_Position;
         float pm_Intensity;

        ////////////////////////////////////////////// Constructor //////////////////////////////////////////////

         PointLight3D() = default;

         PointLight3D(const glm::vec4& fp_Colour, const glm::vec3& fp_Position, const float fp_Intensity)
         {
             pm_Colour = fp_Colour;
             pm_Position = fp_Position;
             pm_Intensity = fp_Intensity;
        }

         PointLight3D(const glm::vec4& fp_Colour, const glm::vec3& fp_Position, const float fp_Intensity, const glm::vec3& fp_Attenuation)
         {
             pm_Colour = fp_Colour;
             pm_Position = fp_Position;
             pm_Intensity = fp_Intensity;

             Attenuation.Constant = fp_Attenuation.x;
             Attenuation.Linear = fp_Attenuation.y;
             Attenuation.Exponent = fp_Attenuation.z;
        }

         ////////////////////////////////////////////// Setter and Getters //////////////////////////////////////////////

         void 
             SetAttenuation(const glm::vec3& fp_Attenuation)
         {
             Attenuation.Constant = fp_Attenuation.x;
             Attenuation.Linear = fp_Attenuation.y;
             Attenuation.Exponent = fp_Attenuation.z;
         }
    };
}