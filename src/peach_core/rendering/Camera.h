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
#include "utils/DoubleBuffered.h"
#include "cglm/struct/mat4.h"

namespace PeachCore {

    struct Camera2D
    {
    private:
        vec2s m_ViewportSize{ { 1280.0f, 720.0f } }; // pixels
        float m_Zoom{ 1.0f };     // 1.0 = 1 world unit = 1 pixel

        mat4s m_Projection = GLMS_MAT4_IDENTITY_INIT;
        mat4s m_View = GLMS_MAT4_IDENTITY_INIT;
        mat4s m_ViewProjection = GLMS_MAT4_IDENTITY_INIT;

        bool m_ProjDirty{ true };
        bool m_ViewDirty{ true };

        DoubleBuffered::Transform2D pm_Transform;

    public:
        explicit
            Camera2D
        (
            const vec2s fp_ViewportSize, 
            float fp_Zoom = 1.0f
        ) 
            : 
            m_ViewportSize(fp_ViewportSize), 
            m_Zoom(fp_Zoom) 
        {}

        // --- access to transform so PeachNodes / scripts can move camera ---
        DoubleBuffered::Transform2D&
            GetTransform()       
            noexcept
        {
            return pm_Transform;
        }

        const DoubleBuffered::Transform2D&
            GetTransform() 
            const noexcept
        { 
            return pm_Transform;
        }

        void 
            SetViewportSize(const vec2s fp_Size)
            noexcept
        {
            m_ViewportSize = fp_Size;
            m_ProjDirty = true;
        }

        void 
            SetZoom(float fp_Zoom)
            noexcept
        {
            m_Zoom = glm_max(fp_Zoom, 0.0001f); // avoid divide-by-zero zoom
            m_ProjDirty = true;
        }

        vec2s 
            GetViewportSize() 
            const noexcept 
        { 
            return m_ViewportSize;
        }

        float 
            GetZoom()         
            const noexcept 
        { 
            return m_Zoom;
        }

        // --- matrices ---

        const mat4s& 
            GetProjection() 
            noexcept
        {
            if (m_ProjDirty)
            {
                // World units == pixels / zoom, Bottom-left = (0,0), top-right = (width/zoom, height/zoom)
                float left = 0.0f;
                float right = m_ViewportSize.x / m_Zoom;
                float bottom = 0.0f;
                float top = m_ViewportSize.y / m_Zoom;

                // 2D, depth not super important; just use [-1,1]
                m_Projection = glms_ortho(left, right, bottom, top, -1.0f, 1.0);

                m_ProjDirty = false;
            }
            return m_Projection;
        }

        const mat4s& 
            GetView() 
            noexcept
        {
            // Ask Transform2D for matrix; it will recompute if dirty, Camera view is the inverse of its world transform

            // if (m_ViewDirty)
            // {
                m_View = glms_mat4_inv(pm_Transform.GetReadMatrix());
                m_ViewDirty = false;
            // }

            return m_View;
        }

        const mat4s& 
            GetViewProjection()
            noexcept
        {
            m_ViewProjection = glms_mat4_mul(GetProjection(), GetView());
            return m_ViewProjection;
        }

        vec2s 
            GetWorldPosition() 
            const noexcept
        {
            return pm_Transform.GetPosition();
        }
    };
}

namespace PeachCore {

    struct Plane
    {
        vec3s m_Normal;
        float m_Distance;

        // Constructor to initialize and normalize the plane
        Plane(const vec3s fp_Normal, float fp_Distance)
            : m_Normal(fp_Normal), m_Distance(fp_Distance)
        {
            Normalize();
        }

        inline void 
            Normalize()
        {
            float f_Magnitude = glms_vec3_norm(m_Normal);

            if (f_Magnitude < 1e-8f) // degenerate, don't divide
            {
                return; 
            } 

            float f_InverseMagnitude = 1.0f / f_Magnitude;
            m_Normal = glms_vec3_scale(m_Normal, f_InverseMagnitude);
            m_Distance *= f_InverseMagnitude;
        }

        // Calculate the signed distance from the plane to a point
        inline float 
            DistanceToPoint(const vec3s fp_TestPoint) 
            const
        {
            return glms_vec3_dot(m_Normal, fp_TestPoint) + m_Distance;
        }
    };

    struct Frustum
    {
        Plane m_FrustumPlanes[6];

        void 
            UpdateFrustum(const mat4 fp_ProjViewMatrix)
        {
            // Left Plane
            m_FrustumPlanes[0].m_Normal.x = fp_ProjViewMatrix[0][3] + fp_ProjViewMatrix[0][0];
            m_FrustumPlanes[0].m_Normal.y = fp_ProjViewMatrix[1][3] + fp_ProjViewMatrix[1][0];
            m_FrustumPlanes[0].m_Normal.z = fp_ProjViewMatrix[2][3] + fp_ProjViewMatrix[2][0];
            m_FrustumPlanes[0].m_Distance = fp_ProjViewMatrix[3][3] + fp_ProjViewMatrix[3][0];

            // Right Plane
            m_FrustumPlanes[1].m_Normal.x = fp_ProjViewMatrix[0][3] - fp_ProjViewMatrix[0][0];
            m_FrustumPlanes[1].m_Normal.y = fp_ProjViewMatrix[1][3] - fp_ProjViewMatrix[1][0];
            m_FrustumPlanes[1].m_Normal.z = fp_ProjViewMatrix[2][3] - fp_ProjViewMatrix[2][0];
            m_FrustumPlanes[1].m_Distance = fp_ProjViewMatrix[3][3] - fp_ProjViewMatrix[3][0];

            // Top Plane
            m_FrustumPlanes[2].m_Normal.x = fp_ProjViewMatrix[0][3] - fp_ProjViewMatrix[0][1];
            m_FrustumPlanes[2].m_Normal.y = fp_ProjViewMatrix[1][3] - fp_ProjViewMatrix[1][1];
            m_FrustumPlanes[2].m_Normal.z = fp_ProjViewMatrix[2][3] - fp_ProjViewMatrix[2][1];
            m_FrustumPlanes[2].m_Distance = fp_ProjViewMatrix[3][3] - fp_ProjViewMatrix[3][1];

            // Bottom Plane
            m_FrustumPlanes[3].m_Normal.x = fp_ProjViewMatrix[0][3] + fp_ProjViewMatrix[0][1];
            m_FrustumPlanes[3].m_Normal.y = fp_ProjViewMatrix[1][3] + fp_ProjViewMatrix[1][1];
            m_FrustumPlanes[3].m_Normal.z = fp_ProjViewMatrix[2][3] + fp_ProjViewMatrix[2][1];
            m_FrustumPlanes[3].m_Distance = fp_ProjViewMatrix[3][3] + fp_ProjViewMatrix[3][1];

            // Near Plane
            m_FrustumPlanes[4].m_Normal.x = fp_ProjViewMatrix[0][3] + fp_ProjViewMatrix[0][2];
            m_FrustumPlanes[4].m_Normal.y = fp_ProjViewMatrix[1][3] + fp_ProjViewMatrix[1][2];
            m_FrustumPlanes[4].m_Normal.z = fp_ProjViewMatrix[2][3] + fp_ProjViewMatrix[2][2];
            m_FrustumPlanes[4].m_Distance = fp_ProjViewMatrix[3][3] + fp_ProjViewMatrix[3][2];

            // Far Plane
            m_FrustumPlanes[5].m_Normal.x = fp_ProjViewMatrix[0][3] - fp_ProjViewMatrix[0][2];
            m_FrustumPlanes[5].m_Normal.y = fp_ProjViewMatrix[1][3] - fp_ProjViewMatrix[1][2];
            m_FrustumPlanes[5].m_Normal.z = fp_ProjViewMatrix[2][3] - fp_ProjViewMatrix[2][2];
            m_FrustumPlanes[5].m_Distance = fp_ProjViewMatrix[3][3] - fp_ProjViewMatrix[3][2];

            // Normalize all the FrustumPlanes
            for (int lv_Index = 0; lv_Index < 6; lv_Index++)
            {
                m_FrustumPlanes[lv_Index].Normalize(); //ye
            }
        }


        bool
            IsBoxVisible(const vec3s fp_Min, const vec3s fp_Max) //used for checking if a bounding box encapsulating the object is inside the frustum
            const
        {
            for (int lv_Index = 0; lv_Index < 6; lv_Index++)
            {
                if 
                (
                    m_FrustumPlanes[lv_Index].DistanceToPoint(fp_Min) < 0 and
                    m_FrustumPlanes[lv_Index].DistanceToPoint({{fp_Max.x, fp_Min.y, fp_Min.z}}) < 0 and
                    m_FrustumPlanes[lv_Index].DistanceToPoint({{fp_Min.x, fp_Max.y, fp_Min.z}}) < 0 and
                    m_FrustumPlanes[lv_Index].DistanceToPoint({{fp_Max.x, fp_Max.y, fp_Min.z}}) < 0 and
                    m_FrustumPlanes[lv_Index].DistanceToPoint({{fp_Min.x, fp_Min.y, fp_Max.z}}) < 0 and
                    m_FrustumPlanes[lv_Index].DistanceToPoint({{fp_Max.x, fp_Min.y, fp_Max.z}}) < 0 and
                    m_FrustumPlanes[lv_Index].DistanceToPoint({{fp_Min.x, fp_Max.y, fp_Max.z}}) < 0 and
                    m_FrustumPlanes[lv_Index].DistanceToPoint(fp_Max) < 0
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
        vec3s pm_Position; //keeps track of current position of the camera
        vec3s pm_Forwards; //always points forwards relative to the camera
        vec3s pm_Upwards; //always point upwards relative to the camera
        vec3s pm_Sideways; //always points perpindicular to forwards and upwards
        vec3s pm_GlobalUp; //contains information about the vector that defines the worldspace's basis up vector

    public:
        // Euler Angles
        float m_HorizontalRotation; //controls left-right movement
        float m_VerticalRotation; //controls up-down movement

        // Camera options
        float m_FOV;
        float m_AspectRatio;
        float m_NearClippingPlane;
        float m_FarClippingPlane;

        // Matrices
        mat4s m_ModelViewMatrix;
        mat4s m_ProjectionMatrix;

        Camera3D
        (
            vec3s position, 
            vec3s up, 
            float yaw, 
            float pitch, 
            float fov, 
            float aspect, 
            float nearP, 
            float farP
        )
            : 
            pm_Position(position),
            pm_Forwards({{0.0f, 0.0f, -1.0f}}),
            pm_GlobalUp(up), 
            m_HorizontalRotation(yaw), 
            m_VerticalRotation(pitch),
            m_FOV(fov),
            m_AspectRatio(aspect),
            m_NearClippingPlane(nearP),
            m_FarClippingPlane(farP)
        {
            UpdateCameraOrientationVectors();
            UpdateCameraMatrices();
        }

        void
            UpdateCameraOrientationVectors() //book keeping for the camera orientation vectors
        {
            vec3s f_Front =
            {{
                cosf(m_HorizontalRotation) * cosf(m_VerticalRotation),
                sinf(m_VerticalRotation),
                sinf(m_HorizontalRotation) * cosf(m_VerticalRotation)
            }};

            pm_Forwards = glms_vec3_normalize(f_Front);
            pm_Sideways = glms_vec3_normalize(glms_vec3_cross(pm_Forwards, pm_GlobalUp));
            pm_Upwards  = glms_vec3_normalize(glms_vec3_cross(pm_Sideways, pm_Forwards));

            UpdateCameraMatrices();
        }

        void 
            UpdateCameraMatrices()
        {
            m_ModelViewMatrix = glms_lookat(pm_Position, glms_vec3_add(pm_Position, pm_Forwards), pm_Upwards);
            m_ProjectionMatrix = glms_perspective(m_FOV, m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
        }

        void
            Move(const vec3s fp_Delta)
        {
            pm_Position = glms_vec3_add(pm_Position, fp_Delta);
        }

        vec3s
            GetPosition()
            const noexcept
        {
            return pm_Position;
        }

        vec3s
            GetRotation()
            const noexcept
        {
            return {{0.0f, 0.0f, 0.0f}};
        }
    };
}