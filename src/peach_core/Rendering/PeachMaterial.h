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

///PeachCore
#include "PeachTexture.h"

///STL
#include <glm/glm.hpp>

namespace PeachCore {

    static const glm::vec4 DEFAULT_COLOUR = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    struct PeachMaterial
    {
        uint64_t Texture = 0;
        float pm_Reflectance = 0;

        glm::vec4 pm_AmbientColour{ DEFAULT_COLOUR };
        glm::vec4 pm_DiffuseColour{ DEFAULT_COLOUR };
        glm::vec4 pm_SpecularColour{ DEFAULT_COLOUR };

        ////////////////////////////////////////////// Constructor //////////////////////////////////////////////

         PeachMaterial() = default;

         PeachMaterial
         (
             const uint64_t fp_Texture, 
             const float fp_Reflectance,
             const glm::vec4& fp_AmbientColour, 
             const glm::vec4& fp_DiffuseColour, 
             const glm::vec4& fp_SpecularColour
         )
             :
             Texture(fp_Texture),
             pm_Reflectance(fp_Reflectance),
             pm_AmbientColour(fp_AmbientColour),
             pm_DiffuseColour(fp_DiffuseColour),
             pm_SpecularColour(fp_SpecularColour)
         {}

        ////////////////////////////////////////////// Setter and Getters //////////////////////////////////////////////

         void 
             SetAmbientColour(const glm::vec4& fp_AmbientColour)
             noexcept
         {
            pm_AmbientColour = fp_AmbientColour;
        }

         void 
             SetDiffuseColour(const glm::vec4& fp_DiffuseColour)
             noexcept
         {
            pm_DiffuseColour = fp_DiffuseColour;
        }

         void
             SetSpecularColour(const glm::vec4& fp_SpecularColour)
             noexcept
         {
             pm_SpecularColour = fp_SpecularColour;
         }

         void
             SetReflectance(const float fp_Reflectance)
             noexcept
         {
             pm_Reflectance = fp_Reflectance;
         }

         void
             SetTexture(const uint64_t fp_Texture)
             noexcept
         {
             Texture = fp_Texture;
         }

         [[nodiscard]] const glm::vec4&
             GetDiffuseColour()
             const noexcept
         {
             return pm_DiffuseColour;
         }

         [[nodiscard]] const glm::vec4&
             GetSpecularColour() 
             const noexcept
         {
            return pm_SpecularColour;
        }

         [[nodiscard]] const glm::vec4&
             GetAmbientColour()
             const noexcept
         {
             return pm_AmbientColour;
         }

         [[nodiscard]] float
             GetReflectance()
             const noexcept
         {
            return pm_Reflectance;
        }

         [[nodiscard]] uint64_t
             GetTextureID() 
             const noexcept
         {
            return Texture;
        }

         [[nodiscard]] bool
             IsTextured()
             const noexcept
         {
             return Texture != 0;
         }
    };
}