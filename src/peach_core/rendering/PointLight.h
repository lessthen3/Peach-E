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

    struct Attenuation
    {
        float Constant = 1.0f;
        float Linear = 0.0f;
        float Exponent = 0.0f;
        float _________Padding = 0.0f;
    };

    static constexpr Attenuation DEFAULT_ATTENUATION = {1.0f, 0.07f, 0.017f, 0.0f};
}

namespace PeachCore {

    struct PointLight
    {
        Attenuation m_Attenuation;
        vec4s m_Colour;
        vec3s m_Position;
        float m_Intensity;

        explicit
            PointLight
            (
                const vec4s fp_Colour, 
                const vec3s fp_Position, //for 2D we just use x and y, the struct needs to be aligned 16 bytes and might as well use every byte
                const float fp_Intensity, 
                const Attenuation fp_Attenuation = DEFAULT_ATTENUATION
            )
            :
            m_Attenuation(fp_Attenuation),
            m_Colour(fp_Colour),
            m_Position(fp_Position),
            m_Intensity(fp_Intensity)
        {}

        ////////////////////////////////////////////// Setter and Getters //////////////////////////////////////////////

        void 
            SetAttenuation(const Attenuation fp_Attenuation) //no bad alloc here and its stack allocd passed by val OwOs
            noexcept
        {
            m_Attenuation = fp_Attenuation;
        }
    } ;
}

static_assert(alignof(PeachCore::PointLight) == 16, "PointLight must be aligned 16 bytes for std 140 layout as a UBO");

//if attenuation size is greater than 16 bytes then itll fuck the entire alignment of point light
static_assert(sizeof(PeachCore::Attenuation) == 16, "PeachCore::Attenuation is not 16 bytes wide this will cause UBO issues for layout std=140");
