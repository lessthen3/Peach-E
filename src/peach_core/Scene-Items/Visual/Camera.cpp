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
#include "Camera.h"

namespace PeachCore 
{
    //Camera2D::~Camera2D()
    //{
    //    if(pm_View)
    //    {
    //        pm_View.reset(nullptr);
    //    }

    //    Disable();
    //}

    //Camera2D::Camera2D(sf::RenderWindow& fp_RenderWindow)
    //    : pm_CurrentWindow(fp_RenderWindow) 
    //{
    //    pm_View = make_unique<sf::View>();

    //    // Initialize the view to the size of the window
    //    pm_View->setSize(fp_RenderWindow.getSize().x, fp_RenderWindow.getSize().y);
    //    pm_View->setCenter(fp_RenderWindow.getSize().x / 2, fp_RenderWindow.getSize().y / 2);
    //}

    //void Camera2D::SetCenter(float fp_X, float fp_Y) 
    //{
    //    pm_View->setCenter(fp_X, fp_Y);
    //}

    //void Camera2D::SetSize(const float fp_Width, const float fp_Height) 
    //{
    //    pm_View->setSize(fp_Width, fp_Height);
    //}

    //void Camera2D::SetRotation(float fp_Angle) 
    //{
    //    pm_Rotation = fp_Angle;
    //}

    //void Camera2D::SetZoom(float fp_Zoom) 
    //{
    //    pm_Scale = fp_Zoom;
    //}
    //
    //void Camera2D::Move(float fp_Dx, float fp_Dy) 
    //{
    //    pm_GlobalPosition.x += fp_Dx;
    //    pm_GlobalPosition.y += fp_Dy;
    //}

    //void Camera2D::Rotate(float fp_Angle) 
    //{
    //    pm_Rotation += fp_Angle;
    //}

    //void Camera2D::Zoom(float fp_Zoom) 
    //{
    //    //m_Camera.zoom += fp_Zoom;
    //    //if (m_Camera.zoom < 0.1f) 
    //    //    {m_Camera.zoom = 0.1f;} // Prevent zoom from becoming too small
    //}

    //void Camera2D::Enable() 
    //{
    //    pm_CurrentWindow.setView(*pm_View);
    //}

    //void Camera2D::Disable()
    //{
    //    return; //>w<
    //}
}
