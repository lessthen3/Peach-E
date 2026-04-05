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

///GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace PeachCore {

    struct Transform2D
    {
    private:
        glm::vec2 pm_Position;
        float pm_Rotation;          // radians, CCW = +'ve
        glm::vec2 pm_Scale;
        glm::vec2 pm_Origin;    // pivot in local space (e.g. sprite center)

        glm::mat4 pm_LocalMatrix{ 1.0f };

        bool pm_IsDirty{ true }; //>W< used to track whether the matrix needs to be recalculated owo!, IMPORTANT: intialize to true since after construction just make sure its updated props

    public:
        Transform2D()
            : pm_Position({ 0.0f, 0.0f })
            , pm_Rotation( 0.0f )
            , pm_Scale({ 1.0f, 1.0f })
            , pm_Origin({ 0.0f, 0.0f })
        {}

        Transform2D
        (
            const glm::vec2& fp_Position,
            float fp_RotationRadians,
            const glm::vec2& fp_Scale,
            const glm::vec2& fp_Origin
        )
            : pm_Position(fp_Position)
            , pm_Rotation(fp_RotationRadians)
            , pm_Scale(fp_Scale)
            , pm_Origin(fp_Origin) 
        {}

        ////////////////////////////////////////////// Getters //////////////////////////////////////////////

        const glm::vec2& GetPosition() const noexcept { return pm_Position; }
        float                 GetRotation() const noexcept { return pm_Rotation; }   // radians
        const glm::vec2& GetScale()    const noexcept { return pm_Scale; }
        const glm::vec2& GetOrigin()   const noexcept { return pm_Origin; }

        ////////////////////////////////////////////// Setters //////////////////////////////////////////////

        void 
            SetPosition(const glm::vec2& fp_Position) 
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
            SetScale(const glm::vec2& fp_Scale) 
            noexcept
        {
            pm_Scale = fp_Scale;
            pm_IsDirty = true;
        }

        void 
            SetOrigin(const glm::vec2& fp_Origin) 
            noexcept
        {
            pm_Origin = fp_Origin;
            pm_IsDirty = true;
        }

        ////////////////////////////////////////////// Incremental ops //////////////////////////////////////////////

        void 
            Translate(const glm::vec2& fp_Offset) 
            noexcept
        {
            pm_Position += fp_Offset;
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
            ScaleBy(const glm::vec2& fp_Factor)
            noexcept
        {
            pm_Scale *= fp_Factor;
            pm_IsDirty = true;
        }

        ////////////////////////////////////////////// Direction helpers, useful for audio + physics //////////////////////////////////////////////
        
        glm::vec2 
            Right()  // (local +X) vector in world space
            const noexcept
        {
            return { cos(pm_Rotation), sin(pm_Rotation) };
        }
        
        glm::vec2 
            Up()  // (local +Y) vector in world space
            const noexcept
        {
            return { -sin(pm_Rotation), cos(pm_Rotation) };   // rotate (0,1) by pm_Rotation
        }

        const glm::mat4& 
            GetLocalMatrix()
            noexcept
        {
            if(pm_IsDirty)
            {
                pm_LocalMatrix = glm::mat4(1.0f);

                pm_LocalMatrix = glm::translate(pm_LocalMatrix, glm::vec3(pm_Position, 0.0f));
                pm_LocalMatrix = glm::rotate(pm_LocalMatrix, pm_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
                pm_LocalMatrix = glm::scale(pm_LocalMatrix, glm::vec3(pm_Scale, 1.0f));
                pm_LocalMatrix = glm::translate(pm_LocalMatrix, glm::vec3(-pm_Origin, 0.0f));

                pm_IsDirty = false;
            }

            return pm_LocalMatrix;
        }
    };

     struct Transform3D
    {
    private:
        //Intiailize as and identity matrices uwu owo!
        glm::mat4 pm_LocalMatrix{ 1.0f };

        //API reachpoint vars for human friendly shtuff >w<

        glm::vec3 pm_Position{ 0.0f, 0.0f, 0.0f };
        glm::quat pm_Rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // identity quaternion — NOT Euler
        glm::vec3 pm_Scale{ 1.0f, 1.0f, 1.0f };

        //Deterimines whether local matrix needs to be updated on request owo
        bool  pm_IsDirty{ true }; // true so first GetLocalMatrix() always computes

    public:
        Transform3D() = default;

    public:
        void SetPosition(const glm::vec3& fp_Pos)    noexcept { pm_Position = fp_Pos;  pm_IsDirty = true; }
        void SetScale(const glm::vec3& fp_Scale)  noexcept { pm_Scale = fp_Scale; pm_IsDirty = true; }

        void SetRotation(const glm::quat& fp_Quat)   noexcept { pm_Rotation = fp_Quat; pm_IsDirty = true; }

        // Euler convenience — converts to quat internally, no Euler state stored
        void
            SetRotationEuler
            (
                const glm::vec3& fp_EulerRadians
            )
            noexcept
        {
            pm_Rotation = glm::quat(fp_EulerRadians); // glm constructs quat from Euler
            pm_IsDirty = true;
        }

        void
            Rotate
            (
                float fp_AngleRadians,
                const glm::vec3& fp_Axis
            )
            noexcept
        {
            pm_Rotation = glm::normalize(glm::angleAxis(fp_AngleRadians, fp_Axis) * pm_Rotation);
            pm_IsDirty = true;
        }

        void Translate(const glm::vec3& fp_Offset) noexcept { pm_Position += fp_Offset; pm_IsDirty = true; }

        // "Forward" in local space, -Z by convention, matches OpenGL and glm
        //third hand rule uwu
        glm::vec3 Forward() const noexcept { return pm_Rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
        glm::vec3 Right()      const noexcept { return pm_Rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
        glm::vec3 Up()         const noexcept { return pm_Rotation * glm::vec3(0.0f, 1.0f, 0.0f); }

        const glm::mat4&
            GetLocalMatrix()
            noexcept
        {
            if (pm_IsDirty)
            {                                                                                                   // quat → rotation matrix
                pm_LocalMatrix = glm::translate(glm::mat4(1.0f), pm_Position) * glm::mat4_cast(pm_Rotation) * glm::scale(glm::mat4(1.0f), pm_Scale);
                pm_IsDirty = false;
            }

            return pm_LocalMatrix;
        }
    };
}