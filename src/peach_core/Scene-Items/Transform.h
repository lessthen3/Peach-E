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

///GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace PeachCore {

    struct Transform2D
    {
    private:
        glm::vec2 pm_Position;
        float       pm_Rotation;          // radians, CCW = +'ve
        glm::vec2 pm_Scale;
        glm::vec2 pm_Origin;    // pivot in local space (e.g. sprite center)

        glm::mat4 pm_LocalMatrix{ 1.0f };

        bool pm_IsDirty{ true }; //>W< used to track whether the matrix needs to be recalculated owo!, IMPORTANT: intialize to true since after construction just make sure its updated props

    public:
        Transform2D()
            : pm_Position({ 0.0f, 0.0f })
            , pm_Rotation({ 0.0f })
            , pm_Scale({ 1.0f, 1.0f })
            , pm_Origin({ 0.0f, 0.0f })
        {}

        Transform2D
        (
            const glm::vec2& fp_Position = { 0.0f, 0.0f },
            float fp_RotationRadians = { 0.0f },
            const glm::vec2& fp_Scale = { 1.0f, 1.0f },
            const glm::vec2& fp_Origin = { 0.0f, 0.0f }
        )
            : pm_Position(fp_Position)
            , pm_Rotation(fp_RotationRadians)
            , pm_Scale(fp_Scale)
            , pm_Origin(fp_Origin) 
        {}

        // --- Getters ---
        const glm::vec2& GetPosition() const noexcept { return pm_Position; }
        float                 GetRotation() const noexcept { return pm_Rotation; }   // radians
        const glm::vec2& GetScale()    const noexcept { return pm_Scale; }
        const glm::vec2& GetOrigin()   const noexcept { return pm_Origin; }

        // --- Setters ---
        void SetPosition(const glm::vec2& fp_Position) noexcept { pm_Position = fp_Position; }
        void SetRotation(float fp_RotationRadians)      noexcept { pm_Rotation = fp_RotationRadians; }
        void SetScale(const glm::vec2& fp_Scale)        noexcept { pm_Scale = fp_Scale; }
        void SetOrigin(const glm::vec2& fp_Origin)      noexcept { pm_Origin = fp_Origin; }

        // --- Incremental ops ---

        void 
            Translate(const glm::vec2& fp_Offset) 
            noexcept
        {
            pm_Position += fp_Offset;
            pm_IsDirty = true;
        }

        // angle in radians
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

        // --- Direction helpers (useful for audio & physics) ---

        // "right" (local +X) vector in world space
        glm::vec2 
            Right() 
            const noexcept
        {
            const float c = cos(pm_Rotation);
            const float s = sin(pm_Rotation);
            return { c, s };
        }

        // "up" (local +Y) vector in world space
        glm::vec2 
            Up() 
            const noexcept
        {
            const float c = cos(pm_Rotation);
            const float s = sin(pm_Rotation);
            return { -s, c };   // rotate (0,1) by pm_Rotation
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

                pm_IsDirty = false;
            }

            return pm_LocalMatrix;
        }
    };

     struct Transform3D
    {
    public:
        Transform3D() = default;

    private:
        //Intiailize everything as identity matrices uwu owo!

        glm::mat4 pm_ProjectionMatrix{ 1.0f };
        glm::mat4 pm_GetModelViewMatrix{ 1.0f };
        glm::mat4 pm_ViewMatrix{ 1.0f };
        glm::mat4 pm_OrthographicMatrix{ 1.0f };

        //API reachpoint vars for human friendly shtuff >w<

        glm::vec3 pm_Position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 pm_Scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 pm_Rotation{ 0.0f, 0.0f, 0.0f };

        //Deterimines whether local matrix needs to be updated on request owo

        bool pm_IsDirty{ false };

    public:
    //    glm::mat4
    //        GetProjectionMatrix(float fov, float width, float height, float zNear, float zFar)
    //        const
    //    {
    //        float aspectRatio = width / height;
    //        //ProjectionMatrix.identity();
    //        //ProjectionMatrix.perspective(fov, aspectRatio, zNear, zFar);
    //        return ProjectionMatrix;
    //    }

    //    glm::mat4
    //        GetOrthographicMatrix(float fov, float width, float height, float zNear, float zFar)
    //        const
    //    {
    //        //OrthographicMatrix.identity();
    //        //OrthographicMatrix.orthoSymmetric(width, height, zNear, zFar);
    //        return OrthographicMatrix;
    //    }

    //    glm::mat4
    //        GetModelViewMatrix(const glm::vec3& fp_Rotation, glm::mat4 vMatrix)
    //        const
    //    {
    //        //GetModelViewMatrix.identity().translate(gameItem.getPosition())
    //        //    .rotateX(glm::radians(-fp_Rotation.x))
    //        //    .rotateY(glm::radians(-fp_Rotation.y))
    //        //    .rotateZ(glm::radians(-fp_Rotation.z))
    //        //    .scale(gameItem.getScale());

    //        return  glm::mat4(vMatrix*GetModelViewMatrix);
    //    }

        glm::mat4
            GetViewMatrix(const glm::vec3& fp_CameraPos)
            noexcept
        {
            pm_ViewMatrix = glm::mat4(1.0f);
            //pm_ViewMatrix = glm::rotate(pm_ViewMatrix, pm_Rotation);
            pm_ViewMatrix = glm::translate(pm_ViewMatrix , fp_CameraPos);

            return pm_ViewMatrix;
        }
    };
}