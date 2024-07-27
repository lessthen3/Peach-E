#include "../../../include/Peach-Core2D/Rendering2D/PeachCamera2D.h"

namespace PeachCore 
{

    PeachCamera2D::~PeachCamera2D()
    {
        if(pm_View)
        {
            pm_View.reset(nullptr);
        }

        Disable();
    }

    PeachCamera2D::PeachCamera2D(sf::RenderWindow& fp_RenderWindow)
        : pm_CurrentWindow(fp_RenderWindow) 
    {
        pm_View = make_unique<sf::View>();

        // Initialize the view to the size of the window
        pm_View->setSize(fp_RenderWindow.getSize().x, fp_RenderWindow.getSize().y);
        pm_View->setCenter(fp_RenderWindow.getSize().x / 2, fp_RenderWindow.getSize().y / 2);
    }

    void PeachCamera2D::SetCenter(float fp_X, float fp_Y) 
    {
        pm_View->setCenter(fp_X, fp_Y);
    }

    void PeachCamera2D::SetSize(const float fp_Width, const float fp_Height) 
    {
        pm_View->setSize(fp_Width, fp_Height);
    }

    void PeachCamera2D::SetRotation(float fp_Angle) 
    {
        pm_Rotation = fp_Angle;
    }

    void PeachCamera2D::SetZoom(float fp_Zoom) 
    {
        pm_Scale = fp_Zoom;
    }
    
    void PeachCamera2D::Move(float fp_Dx, float fp_Dy) 
    {
        pm_GlobalPosition.x += fp_Dx;
        pm_GlobalPosition.y += fp_Dy;
    }

    void PeachCamera2D::Rotate(float fp_Angle) 
    {
        pm_Rotation += fp_Angle;
    }

    void PeachCamera2D::Zoom(float fp_Zoom) 
    {
        //m_Camera.zoom += fp_Zoom;
        //if (m_Camera.zoom < 0.1f) 
        //    {m_Camera.zoom = 0.1f;} // Prevent zoom from becoming too small
    }

    void PeachCamera2D::Enable() 
    {
        pm_CurrentWindow.setView(*pm_View);
    }

    void PeachCamera2D::Disable()
    {
        return; //>w<
    }
}
