#pragma once


#include "../Utilities2D/Vector2D.h"

namespace PeachCore {

    class Camera2D {
    public:
        Camera2D(float fp_ScreenWidth, float fp_ScreenHeight)
            : pm_Position(0.0f, 0.0f), pm_Rotation(0.0f), pm_Scale(1.0f),
            m_ScreenWidth(fp_ScreenWidth), m_ScreenHeight(fp_ScreenHeight) {
            UpdateViewMatrix();
        }

        void SetPosition(const Vector2f& position) {
            pm_Position = position;
            UpdateViewMatrix();
        }

        void SetRotation(float rotation) {
            pm_Rotation = rotation;
            UpdateViewMatrix();
        }

        void SetScale(float scale) {
            pm_Scale = scale;
            UpdateViewMatrix();
        }

        const float* GetViewMatrix() const {
            return pm_ViewMatrix;
        }

    private:
        void UpdateViewMatrix() {
            //bx::mtxIdentity(pm_ViewMatrix);
            //bx::mtxScale(pm_ViewMatrix, pm_Scale, pm_Scale, 1.0f);
            //bx::mtxRotateZ(pm_ViewMatrix, pm_Rotation);
            //bx::mtxTranslate(pm_ViewMatrix, -pm_Position.x, -pm_Position.y, 0.0f);

            //// Convert to projection space
            //float ortho[16];
            //bx::mtxOrtho(ortho, 0.0f, m_ScreenWidth, m_ScreenHeight, 0.0f, 0.1f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
            //bx::mtxMul(pm_ViewMatrix, pm_ViewMatrix, ortho);
        }

        Vector2f pm_Position;
        float pm_Rotation;
        float pm_Scale;
        float m_ScreenWidth, m_ScreenHeight;
        float pm_ViewMatrix[16];
    };

}