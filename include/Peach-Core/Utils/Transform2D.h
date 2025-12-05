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

namespace PeachCore {

    class Transform2D
    {
    private:
        glm::vec2 pm_Position;
        float pm_Rotation;
        glm::vec2 pm_Scale;

    public:
        Transform2D() = default;

        Transform2D(const glm::vec2& fp_Position, float fp_Rotation, const glm::vec2& fp_Scale)
            : pm_Position(fp_Position), pm_Rotation(fp_Rotation), pm_Scale(fp_Scale) {}

        // Getters
        const glm::vec2& GetPosition() const { return pm_Position; }
        const float GetRotation() const { return pm_Rotation; }
        const glm::vec2& GetScale() const { return pm_Scale; }

        // Setters
        void 
            SetPosition(const glm::vec2& fp_Position) 
        { 
            pm_Position = fp_Position; 
        }

        void 
            SetRotation(const float fp_Rotation)
        { 
            pm_Rotation = fp_Rotation; 
        }

        void 
            SetScale(const glm::vec2& fp_Scale)
        { 
            pm_Scale = fp_Scale;
        }

        //void SetTransform(const sf::Transform& fp_Transform) { pm_Transform = fp_Transform; }

        // Translate (move) by an offset
        //void Translate(const glm::vec2& fp_Offset) { pm_Position += fp_Offset; }

        // Rotate by an angle (in degrees)
        void 
            Rotate(const float fp_Angle) 
        { 
            pm_Rotation += fp_Angle; 
        }

        // pm_Scale uniformly or non-uniformly
        //void ScaleBy(const glm::vec2& fp_Factor) { pm_Scale.x *= fp_Factor.x; pm_Scale.y *= fp_Factor.y; }

        // Apply the transform to a point
        //glm::vec2 TransformPoint(const glm::vec2& fp_Point) {
        //    pm_Transform.translate(pm_Position);
        //    pm_Transform.rotate(pm_Rotation);
        //    pm_Transform.scale(pm_Scale);
        //    return pm_Transform.transformPoint(fp_Point);
        //}
    };
}