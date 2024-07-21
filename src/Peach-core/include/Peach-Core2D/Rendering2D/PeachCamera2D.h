#pragma once

#include "../../Unsorted/raylib-conflictfree.h"


#include <glm/glm.hpp>

namespace PeachCore {

    class PeachCamera2D {
    public:
        PeachCamera2D(float screenWidth, float screenHeight);
        PeachCamera2D() = default;

        void SetCenter(float x, float y);
        void SetSize(float width, float height);
        void SetRotation(float angle);
        void SetZoom(float zoom);

        void Move(float dx, float dy);
        void Rotate(float dAngle);
        void Zoom(float dZoom);

        void Apply();

    private:
        Camera2D m_Camera;
        float m_ScreenWidth, m_ScreenHeight;
    };

}
