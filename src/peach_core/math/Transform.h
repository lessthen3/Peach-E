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

///cglm
#include <cglm/cglm.h>
#include <cglm/struct.h>

///PeachCore
#include "utils/PeachForceInline.h"

///CSTD
#include <cmath>

namespace PeachCore::Math {

    struct Transform2D
    {
    private:
        vec2s pm_Position;
        float pm_Rotation;          // radians, CCW = +'ve
        vec2s pm_Scale;
        vec2s pm_Origin;    // pivot in local space (e.g. sprite center)

        mat4s pm_LocalMatrix; //don't needa set this because only one constructor, and it will set it through RegenerateLocalMatrix()
        bool pm_IsDirty{ false }; //>W< used to track whether the matrix needs to be recalculated owo!, IMPORTANT: intialize to false  to make sure RegenerateLocalMatrix() hits its is dirty flag >w<

    public:
        Transform2D()
            : pm_Position({{ 0.0f, 0.0f }})
            , pm_Rotation( 0.0f )
            , pm_Scale({{ 1.0f, 1.0f }})
            , pm_Origin({{ 0.0f, 0.0f }})
        {
            RegenerateLocalMatrix();
        }

        Transform2D
        (
            const vec2s fp_Position,
            float fp_RotationRadians,
            const vec2s fp_Scale,
            const vec2s fp_Origin
        )
            : pm_Position(fp_Position)
            , pm_Rotation(fp_RotationRadians)
            , pm_Scale(fp_Scale)
            , pm_Origin(fp_Origin) 
        {
            RegenerateLocalMatrix();
        }

        ////////////////////////////////////////////// Getters //////////////////////////////////////////////

        vec2s GetPosition() const noexcept { return pm_Position; }
        float GetRotation() const noexcept { return pm_Rotation; }   // radians
        vec2s GetScale()    const noexcept { return pm_Scale;    } 
        vec2s GetOrigin()   const noexcept { return pm_Origin;   }

        ////////////////////////////////////////////// Setters //////////////////////////////////////////////

        void 
            SetPosition(const vec2s fp_Position) 
            noexcept
        {
            pm_Position = fp_Position;
            pm_IsDirty = true;
        }

        void 
            SetRotation(float fp_Radians)
            noexcept
        {
            pm_Rotation = fp_Radians;
            pm_IsDirty = true;
        }

        void 
            SetScale(const vec2s fp_Scale) 
            noexcept
        {
            pm_Scale = fp_Scale;
            pm_IsDirty = true;
        }

        void 
            SetOrigin(const vec2s fp_Origin) 
            noexcept
        {
            pm_Origin = fp_Origin;
            pm_IsDirty = true;
        }

        ////////////////////////////////////////////// Incremental ops //////////////////////////////////////////////

        void 
            Translate(const vec2s fp_Offset) 
            noexcept
        {
            pm_Position = glms_vec2_add(pm_Position, fp_Offset);
            pm_IsDirty = true;
        }

        ////////////////////////////////////////////// angle in radians //////////////////////////////////////////////

        void 
            Rotate(const float fp_DeltaRadians)
            noexcept
        {
            pm_Rotation += fp_DeltaRadians;
            pm_IsDirty = true;
        }

        void
            ScaleBy(const vec2s fp_Factor)
            noexcept
        {
            pm_Scale = glms_vec2_mul(pm_Scale, fp_Factor);
            pm_IsDirty = true;
        }

        ////////////////////////////////////////////// Direction helpers, useful for audio + physics //////////////////////////////////////////////
        
        vec2s 
            Right()  // (local +X) vector in world space
            const noexcept
        {
            return {{cos(pm_Rotation), sin(pm_Rotation)}};
        }
        
        vec2s 
            Up()  // (local +Y) vector in world space
            const noexcept
        {
            return {{-sin(pm_Rotation), cos(pm_Rotation)}};   // rotate (0,1) by pm_Rotation
        }

    [[nodiscard]] PEACH_FORCEINLINE const mat4s& 
            GetLocalMatrix()
            noexcept
        {
            if(pm_IsDirty)
            {
                RegenerateLocalMatrix();
            }

            return pm_LocalMatrix;
        }

        PEACH_FORCEINLINE void
            RegenerateLocalMatrix()
            noexcept
        {
            pm_LocalMatrix = GLMS_MAT4_IDENTITY_INIT;

            pm_LocalMatrix = glms_translate(pm_LocalMatrix, {{pm_Position.x, pm_Position.y, 0.0f}});
            pm_LocalMatrix = glms_rotate(pm_LocalMatrix, pm_Rotation, {{0.0f, 0.0f, 1.0f}});
            pm_LocalMatrix = glms_scale(pm_LocalMatrix, {{pm_Scale.x, pm_Scale.y, 1.0f}});
            pm_LocalMatrix = glms_translate(pm_LocalMatrix, {{-pm_Origin.x, -pm_Origin.y, 0.0f}});

            pm_IsDirty = false;
        }

        [[nodiscard]] PEACH_FORCEINLINE const mat4s&
            GetLocalMatrixNoRegenerate()
            const noexcept
        {
            return pm_LocalMatrix;
        }
    };

    struct Transform3D //standard TRS matrix
    {
    private:
        //API reachpoint vars for human friendly shtuff >w<

        vec3s pm_Position{{0.0f, 0.0f, 0.0f}};
        versors pm_Rotation{{ 0.0f, 0.0f, 0.0f, 1.0f }}; // identity quat: [x,y,z,w] per cglm v0.4.0+
        vec3s pm_Scale{{ 1.0f, 1.0f, 1.0f }};

        //Intiailize as and identity matrices uwu owo!
        mat4s pm_LocalMatrix = GLMS_MAT4_IDENTITY_INIT;
        //Deterimines whether local matrix needs to be updated on request owo
        bool  pm_IsDirty{ true }; // true so first GetLocalMatrix() always computes

    public:
        Transform3D() = default;

    public:
        void SetPosition(const vec3s fp_Pos)    noexcept { pm_Position = fp_Pos;  pm_IsDirty = true; }
        void SetScale(const vec3s fp_Scale)  noexcept { pm_Scale = fp_Scale; pm_IsDirty = true; }

        void SetRotation(const versors fp_Quat) noexcept { pm_Rotation = fp_Quat; pm_IsDirty = true; }

        // Euler convenience — converts to quat internally, no Euler state stored
        void
            SetRotationEuler
            (
                const vec3s fp_EulerRadians
            )
            noexcept
        {
            pm_Rotation = glms_euler_xyz_quat(fp_EulerRadians); // cglm constructs quat from Euler
            pm_IsDirty = true;
        }

        void
            Rotate
            (
                float fp_AngleRadians,
                const vec3s fp_Axis
            )
            noexcept
        {
            versors f_Delta = glms_quatv(fp_AngleRadians, fp_Axis);
            pm_Rotation = glms_quat_normalize(glms_quat_mul(f_Delta, pm_Rotation));
            pm_IsDirty = true;
        }

        void 
            Translate(const vec3s fp_Offset)
            noexcept
        {
            pm_Position = glms_vec3_add(pm_Position, fp_Offset);
            pm_IsDirty = true;
        }

        // Forward = -Z local, matches OpenGL/glm convention        
         //third hand rule uwu
        vec3s 
            Forward() 
            const noexcept
        {
            vec3s f_Local = {{ 0.0f, 0.0f, -1.0f }};
            return glms_quat_rotatev(pm_Rotation, f_Local);
        }

        vec3s 
            Right() 
            const noexcept
        {
            vec3s f_Local = {{ 1.0f, 0.0f, 0.0f }};
            return glms_quat_rotatev(pm_Rotation, f_Local);
        }

        vec3s 
            Up() 
            const noexcept
        {
            vec3s f_Local = {{ 0.0f, 1.0f, 0.0f }};
            return glms_quat_rotatev(pm_Rotation, f_Local);
        }

        const mat4s&
            GetLocalMatrix()
            noexcept
        {
            if (pm_IsDirty)
            {
                // T * R * S
                mat4s f_T = glms_translate_make(pm_Position);
                mat4s f_R = glms_quat_mat4(pm_Rotation);
                mat4s f_S = glms_scale_make(pm_Scale);

                pm_LocalMatrix = glms_mat4_mul(glms_mat4_mul(f_T, f_R), f_S);
                pm_IsDirty = false;
            }

            return pm_LocalMatrix;
        }
    };
}