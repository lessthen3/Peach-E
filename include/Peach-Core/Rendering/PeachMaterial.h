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
#include "PeachTexture.h"

///STL
#include <glm/glm.hpp>

namespace PeachCore {

    const glm::vec4 DEFAULT_COLOUR = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    struct PeachMaterial
    {
         glm::vec4 pm_AmbientColour = DEFAULT_COLOUR;
         glm::vec4 pm_DiffuseColour = DEFAULT_COLOUR;
         glm::vec4 pm_SpecularColour = DEFAULT_COLOUR;

         float pm_Reflectance = 0;

         unique_ptr<PeachTexture> pm_Texture = nullptr;

        ////////////////////////////////////////////// Constructor //////////////////////////////////////////////

         PeachMaterial() = default;

         PeachMaterial(unique_ptr<PeachTexture>&& fp_Texture, const glm::vec4& fp_AmbientColour, const glm::vec4& fp_DiffuseColour, const glm::vec4& fp_SpecularColour, const float fp_Reflectance)
         {
            pm_AmbientColour = fp_AmbientColour;
            pm_DiffuseColour = fp_DiffuseColour;
            pm_SpecularColour = fp_SpecularColour;
            pm_Texture = move(fp_Texture);
            pm_Reflectance = fp_Reflectance;
        }

        ////////////////////////////////////////////// Setter and Getters //////////////////////////////////////////////

         [[nodiscard]] glm::vec4
             GetAmbientColour() 
             const noexcept
         {
            return pm_AmbientColour;
        }

         void 
             SetAmbientColour(glm::vec4 pm_AmbientColour)
         {
            pm_AmbientColour = pm_AmbientColour;
        }

         [[nodiscard]] glm::vec4
             GetDiffuseColour()
             const noexcept
         {
            return pm_DiffuseColour;
        }

         void 
             SetDiffuseColour(glm::vec4 pm_DiffuseColour)
         {
            pm_DiffuseColour = pm_DiffuseColour;
        }

         [[nodiscard]] glm::vec4 
             GetSpecularColour() 
             const noexcept
         {
            return pm_SpecularColour;
        }

         void 
             SetSpecularColour(glm::vec4 fp_SpecularColour)
             noexcept
         {
            pm_SpecularColour = fp_SpecularColour;
        }

         [[nodiscard]] float
             GetReflectance()
             const noexcept
         {
            return pm_Reflectance;
        }

         void 
             SetReflectance(float fp_Reflectance)
             noexcept
         {
            pm_Reflectance = fp_Reflectance;
        }

         bool 
             IsTextured()
             const noexcept
         {
            return pm_Texture != nullptr;
        }

         [[nodiscard]] const PeachTexture*
             GetTexture() 
             const noexcept
         {
            return pm_Texture.get();
        }

         void 
             SetTexture(unique_ptr<PeachTexture>&& fp_Texture) 
             noexcept
         {
            pm_Texture = move(fp_Texture);
        }

    };
}