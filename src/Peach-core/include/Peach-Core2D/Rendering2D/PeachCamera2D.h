#pragma once

#include <SFML/Graphics.hpp>


#include <glm/glm.hpp>

namespace PeachCore {

    class PeachCamera2D 
    {
    public:
        PeachCamera2D(sf::RenderWindow& fp_RenderWindow);
        PeachCamera2D() = default;
        ~PeachCamera2D();

        void SetCenter(float x, float y);
        void SetSize(float width, float height);
        void SetRotation(float angle);
        void SetZoom(float zoom);

        void Move(float dx, float dy);
        void Rotate(float dAngle);
        void Zoom(float dZoom);

        void Enable();
        void Disable();

    private:
        sf::RenderWindow& pm_CurrentWindow;
        sf::View pm_View;


        //Vector2f pm_Position;
        float pm_Rotation;
        float pm_Scale;
        float pm_ScreenWidth, pm_ScreenHeight;

        sf::Vector2f pm_GlobalPosition;
    };

}
