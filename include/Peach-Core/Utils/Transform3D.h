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
#include <glm/gtc/matrix_transform.hpp>

namespace PeachCore
{
    class Transform3D
    {
        glm::mat4 ProjectionMatrix;
        glm::mat4 GetModelViewMatrix;
        glm::mat4 ViewMatrix;
        glm::mat4 OrthographicMatrix;

    public:
        Transform3D()
        {
            GetModelViewMatrix = glm::mat4();
            ProjectionMatrix = glm::mat4();
            ViewMatrix = glm::mat4();
            OrthographicMatrix = glm::mat4();
        }

        glm::mat4
            GetProjectionMatrix(float fov, float width, float height, float zNear, float zFar)
            const
        {
            float aspectRatio = width / height;
            ProjectionMatrix.identity();
            ProjectionMatrix.perspective(fov, aspectRatio, zNear, zFar);
            return ProjectionMatrix;
        }

        glm::mat4
            GetOrthographicMatrix(float fov, float width, float height, float zNear, float zFar)
            const
        {
            OrthographicMatrix.identity();
            OrthographicMatrix.orthoSymmetric(width, height, zNear, zFar);
            return OrthographicMatrix;
        }

        glm::mat4
            GetModelViewMatrix(const glm::vec3& fp_Rotation, glm::mat4 vMatrix)
            const
        {
            GetModelViewMatrix.identity().translate(gameItem.getPosition())
                .rotateX(glm::radians(-fp_Rotation.x))
                .rotateY(glm::radians(-fp_Rotation.y))
                .rotateZ(glm::radians(-fp_Rotation.z))
                .scale(gameItem.getScale());

            return  glm::mat4(vMatrix).mul(GetModelViewMatrix);
        }

        glm::mat4 
            GetViewMatrix(Camera3D camera)
            const
        {
            glm::vec3 cameraPos = camera.getPosition();
            glm::vec3 rotation = camera.getRotation();

            ViewMatrix.identity();

            ViewMatrix.rotate(glm::radians(rotation.x), glm::vec3(1, 0, 0))
                .rotate(glm::radians(rotation.y), glm::vec3(0, 1, 0));

            ViewMatrix.translate(-cameraPos.x, -cameraPos.y, -cameraPos.z);

            return ViewMatrix;
        }
    };
}