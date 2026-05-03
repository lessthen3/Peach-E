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

///cglm
#include <cglm/cglm.h>
#include <cglm/struct.h>

namespace PeachCore {

    static constexpr vec4s DEFAULT_COLOUR = {{1.0f, 1.0f, 1.0f, 1.0f}};

    struct PeachMaterial
    {
        PeachTexture pm_Texture;
        float pm_Reflectance;

        vec4s pm_AmbientColour;
        vec4s pm_DiffuseColour;
        vec4s pm_SpecularColour;

        ////////////////////////////////////////////// Constructor //////////////////////////////////////////////

        //  explicit
        //     PeachMaterial
        //  (
        //     PeachTexture&& fp_Texture, 
        //      const float fp_Reflectance,
        //      const vec4s fp_AmbientColour, 
        //      const vec4s fp_DiffuseColour,
        //      const vec4s fp_SpecularColour
        //  )
        //      :
        //      pm_Texture(std::move(fp_Texture)),
        //      pm_Reflectance(fp_Reflectance),
        //      pm_AmbientColour(fp_AmbientColour),
        //      pm_DiffuseColour(fp_DiffuseColour),
        //      pm_SpecularColour(fp_SpecularColour)
        // {}

        explicit
            PeachMaterial()
            :
            pm_Texture(0, 0),
            pm_Reflectance(0),
            pm_AmbientColour(DEFAULT_COLOUR),
            pm_DiffuseColour(DEFAULT_COLOUR),
            pm_SpecularColour(DEFAULT_COLOUR)
        {}
            
        

        ////////////////////////////////////////////// Setter and Getters //////////////////////////////////////////////

         void 
             SetAmbientColour(const vec4s fp_AmbientColour)
             noexcept
         {
            pm_AmbientColour = fp_AmbientColour;
        }

         void 
             SetDiffuseColour(const vec4s fp_DiffuseColour)
             noexcept
         {
            pm_DiffuseColour = fp_DiffuseColour;
        }

         void
             SetSpecularColour(const vec4s fp_SpecularColour)
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

        //  void
        //      SetTexture(PeachTexture&& fp_Texture)
        //      noexcept
        //  {
        //      pm_Texture = std::move(fp_Texture);
        //  }

         [[nodiscard]] vec4s
             GetDiffuseColour()
             const noexcept
         {
             return pm_DiffuseColour;
         }

         [[nodiscard]] vec4s
             GetSpecularColour() 
             const noexcept
         {
            return pm_SpecularColour;
        }

         [[nodiscard]] vec4s
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
    };
}