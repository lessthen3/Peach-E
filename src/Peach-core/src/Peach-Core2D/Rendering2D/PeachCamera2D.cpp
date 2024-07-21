#include "../../../include/Peach-Core2D/Rendering2D/PeachCamera2D.h"

namespace 
    PeachCore 
{
    PeachCamera2D::PeachCamera2D(float fp_ScreenWidth, float fp_ScreenHeight)
        : m_ScreenWidth(fp_ScreenWidth), m_ScreenHeight(fp_ScreenHeight)
    {
        m_Camera.target = Vector2({ fp_ScreenWidth / 2.0f, fp_ScreenHeight / 2.0f });
        m_Camera.offset = Vector2({ fp_ScreenWidth / 2.0f, fp_ScreenHeight / 2.0f });
        m_Camera.rotation = 0.0f;
        m_Camera.zoom = 1.0f;
    }

    void PeachCamera2D::SetCenter(float fp_X, float fp_Y) 
    {
        m_Camera.target = Vector2({ fp_X, fp_Y });
    }

    void PeachCamera2D::SetSize(float fp_Width, float fp_Height) 
    {
        m_Camera.offset = Vector2({ fp_Width / 2.0f, fp_Height / 2.0f });
    }

    void PeachCamera2D::SetRotation(float fp_Angle) 
    {
        m_Camera.rotation = fp_Angle;
    }

    void PeachCamera2D::SetZoom(float fp_Zoom) 
    {
        m_Camera.zoom = fp_Zoom;
    }
    
    void PeachCamera2D::Move(float fp_Dx, float fp_Dy) 
    {
        m_Camera.target.x += fp_Dx;
        m_Camera.target.y += fp_Dy;
    }

    void PeachCamera2D::Rotate(float fp_Angle) 
    {
        m_Camera.rotation += fp_Angle;
    }

    void PeachCamera2D::Zoom(float fp_Zoom) 
    {
        m_Camera.zoom += fp_Zoom;
        if (m_Camera.zoom < 0.1f) 
            {m_Camera.zoom = 0.1f;} // Prevent zoom from becoming too small
    }

    void PeachCamera2D::Apply() 
    {
        BeginMode2D(m_Camera);
    }
}
