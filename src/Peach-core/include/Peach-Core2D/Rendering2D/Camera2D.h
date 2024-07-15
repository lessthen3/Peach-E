#pragma once


#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

namespace PeachCore {

    class Camera2D {
    public:
        Camera2D(sf::RenderWindow& window)
            : m_Window(window) {
            // Initialize the view to the size of the window
            m_View.setSize(window.getSize().x, window.getSize().y);
            m_View.setCenter(window.getSize().x / 2, window.getSize().y / 2);
        }

        void setCenter(float x, float y) {
            m_View.setCenter(x, y);
        }

        void setSize(float width, float height) {
            m_View.setSize(width, height);
        }

        void apply() {
            m_Window.setView(m_View);
        }

    private:
        sf::RenderWindow& m_Window;
        sf::View m_View;

        //Vector2f pm_Position;
        float pm_Rotation;
        float pm_Scale;
        float m_ScreenWidth, m_ScreenHeight;
    };

}