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

#include <memory>

namespace PeachCore {

    struct Camera2D
    {

    };
}

namespace PeachCore {

    using namespace std;

    struct Plane
    {
        glm::vec3 normal;
        float distance;

        // Constructor to initialize and normalize the plane
        Plane(const glm::vec3& fp_Normal, float fp_Distance)
            : normal(fp_Normal), distance(fp_Distance)
        {
            normalize();
        }

        void normalize()
        {
            float mag = glm::length(normal);
            normal = normal / mag;
            distance = distance / mag;
        }

        // Calculate the signed distance from the plane to a point
        float distanceToPoint(const glm::vec3& point) const
        {
            return glm::dot(normal, point) + distance;
        }
    };

    struct Frustum
    {
        Plane FrustumPlanes[6];

        void 
            UpdateFrustum(const glm::mat4& fp_ProjViewMatrix)
        {
            // Left Plane
            FrustumPlanes[0].normal.x = fp_ProjViewMatrix[0][3] + fp_ProjViewMatrix[0][0];
            FrustumPlanes[0].normal.y = fp_ProjViewMatrix[1][3] + fp_ProjViewMatrix[1][0];
            FrustumPlanes[0].normal.z = fp_ProjViewMatrix[2][3] + fp_ProjViewMatrix[2][0];
            FrustumPlanes[0].distance = fp_ProjViewMatrix[3][3] + fp_ProjViewMatrix[3][0];

            // Right Plane
            FrustumPlanes[1].normal.x = fp_ProjViewMatrix[0][3] - fp_ProjViewMatrix[0][0];
            FrustumPlanes[1].normal.y = fp_ProjViewMatrix[1][3] - fp_ProjViewMatrix[1][0];
            FrustumPlanes[1].normal.z = fp_ProjViewMatrix[2][3] - fp_ProjViewMatrix[2][0];
            FrustumPlanes[1].distance = fp_ProjViewMatrix[3][3] - fp_ProjViewMatrix[3][0];

            // Top Plane
            FrustumPlanes[2].normal.x = fp_ProjViewMatrix[0][3] - fp_ProjViewMatrix[0][1];
            FrustumPlanes[2].normal.y = fp_ProjViewMatrix[1][3] - fp_ProjViewMatrix[1][1];
            FrustumPlanes[2].normal.z = fp_ProjViewMatrix[2][3] - fp_ProjViewMatrix[2][1];
            FrustumPlanes[2].distance = fp_ProjViewMatrix[3][3] - fp_ProjViewMatrix[3][1];

            // Bottom Plane
            FrustumPlanes[3].normal.x = fp_ProjViewMatrix[0][3] + fp_ProjViewMatrix[0][1];
            FrustumPlanes[3].normal.y = fp_ProjViewMatrix[1][3] + fp_ProjViewMatrix[1][1];
            FrustumPlanes[3].normal.z = fp_ProjViewMatrix[2][3] + fp_ProjViewMatrix[2][1];
            FrustumPlanes[3].distance = fp_ProjViewMatrix[3][3] + fp_ProjViewMatrix[3][1];

            // Near Plane
            FrustumPlanes[4].normal.x = fp_ProjViewMatrix[0][3] + fp_ProjViewMatrix[0][2];
            FrustumPlanes[4].normal.y = fp_ProjViewMatrix[1][3] + fp_ProjViewMatrix[1][2];
            FrustumPlanes[4].normal.z = fp_ProjViewMatrix[2][3] + fp_ProjViewMatrix[2][2];
            FrustumPlanes[4].distance = fp_ProjViewMatrix[3][3] + fp_ProjViewMatrix[3][2];

            // Far Plane
            FrustumPlanes[5].normal.x = fp_ProjViewMatrix[0][3] - fp_ProjViewMatrix[0][2];
            FrustumPlanes[5].normal.y = fp_ProjViewMatrix[1][3] - fp_ProjViewMatrix[1][2];
            FrustumPlanes[5].normal.z = fp_ProjViewMatrix[2][3] - fp_ProjViewMatrix[2][2];
            FrustumPlanes[5].distance = fp_ProjViewMatrix[3][3] - fp_ProjViewMatrix[3][2];

            // Normalize all the FrustumPlanes
            for (int i = 0; i < 6; i++)
            {
                FrustumPlanes[i].normalize(); //ye
            }
        }


        bool
            IsBoxVisible(const glm::vec3& min, const glm::vec3& max) //used for checking if the box encapsulating the object is inside the frustum
            const
        {
            for (int i = 0; i < 6; i++)
            {
                if 
                (
                    FrustumPlanes[i].distanceToPoint(min) < 0 and
                    FrustumPlanes[i].distanceToPoint(glm::vec3(max.x, min.y, min.z)) < 0 and
                    FrustumPlanes[i].distanceToPoint(glm::vec3(min.x, max.y, min.z)) < 0 and
                    FrustumPlanes[i].distanceToPoint(glm::vec3(max.x, max.y, min.z)) < 0 and
                    FrustumPlanes[i].distanceToPoint(glm::vec3(min.x, min.y, max.z)) < 0 and
                    FrustumPlanes[i].distanceToPoint(glm::vec3(max.x, min.y, max.z)) < 0 and
                    FrustumPlanes[i].distanceToPoint(glm::vec3(min.x, max.y, max.z)) < 0 and
                    FrustumPlanes[i].distanceToPoint(max) < 0
                )
                {
                    return false; // :^)
                }
            }
            return true;
        }
    };

    struct Camera3D
    {
    private:
        glm::vec3 pm_Position; //keeps track of current position of the camera
        glm::vec3 pm_Forwards; //always points forwards relative to the camera
        glm::vec3 pm_Upwards; //always point upwards relative to the camera
        glm::vec3 pm_Sideways; //always points perpindicular to forwards and upwards
        glm::vec3 pm_GlobalUp; //contains information about the vector that defines the worldspace's basis up vector

    public:
        // Euler Angles
        float m_HorizontalRotation; //controls left-right movement
        float m_VerticalRotation; //controls up-down movement

        // Camera options
        float m_FOV;
        float m_NearClippingPlane;
        float m_FarClippingPlane;
        float m_AspectRatio;

        // Matrices
        glm::mat4 m_ModelViewMatrix;
        glm::mat4 m_ProjectionMatrix;

        Camera3D(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float aspect, float nearP, float farP)
            : pm_Forwards(glm::vec3(0.0f, 0.0f, -1.0f)), pm_GlobalUp(up), m_HorizontalRotation(yaw), m_VerticalRotation(pitch),
            m_FOV(fov), m_AspectRatio(aspect), m_NearClippingPlane(nearP), m_FarClippingPlane(farP), pm_Position(position)
        {
            UpdateCameraOrientationVectors();
            UpdateCameraMatrices();
        }

        void
            UpdateCameraOrientationVectors() //book keeping for the camera orientation vectors
        {
            // Calculate the new front vector
            glm::vec3 f_Front
            (
                {
                    cos(glm::radians(m_HorizontalRotation)) * cos(glm::radians(m_VerticalRotation)),
                    sin(glm::radians(m_VerticalRotation)),
                    sin(glm::radians(m_HorizontalRotation)) * cos(glm::radians(m_VerticalRotation))
                }
            );

            pm_Forwards = glm::normalize(f_Front);

            // Recalculate the right and up vector
            pm_Sideways = glm::normalize(glm::cross(f_Front, pm_GlobalUp));
            pm_Upwards = glm::normalize(glm::cross(pm_Sideways, f_Front));

            UpdateCameraMatrices();
        }

        void 
            UpdateCameraMatrices()
        {
            m_ModelViewMatrix = glm::lookAt(pm_Position, pm_Position + pm_Forwards, pm_Upwards);
            m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
        }

        void
            Move(const glm::vec3& fp_Delta)
        {
            pm_Position += fp_Delta;
        }

        glm::vec3
            GetPosition()
            const noexcept
        {
            return pm_Position;
        }

        glm::vec3
            GetRotation()
            const noexcept
        {
            return glm::vec3();
        }

    };
}