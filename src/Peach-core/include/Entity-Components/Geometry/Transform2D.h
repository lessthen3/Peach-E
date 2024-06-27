//#pragma once
//
//
//#include "../PeachComponent.h"
//
//namespace PeachCore {
//
//    struct Transform2D : public PeachComponent {
//    private:
//        Vector2f pm_Position;
//        float pm_Rotation;
//        Vector2f pm_Scale;
//        Transform pm_Transform;
//
//    public:
//        Transform2D()
//            : pm_Position(sf::Vector2f(0.0f, 0.0f)), pm_Rotation(0.0f), pm_Scale(sf::Vector2f(1.0f, 1.0f)) {}
//
//        Transform2D(const sf::Vector2f& fp_Position, float fp_Rotation, const sf::Vector2f& fp_Scale)
//            : pm_Position(fp_Position), pm_Rotation(fp_Rotation), pm_Scale(fp_Scale) {}
//
//        // Getters
//        const sf::Vector2f& GetPosition() const { return pm_Position; }
//        const float GetRotation() const { return pm_Rotation; }
//        const sf::Vector2f& GetScale() const { return pm_Scale; }
//        const sf::Transform& GetTransform() const { return pm_Transform; }
//
//        // Setters
//        void SetPosition(const sf::Vector2f& fp_Position) { pm_Position = fp_Position; }
//        void SetRotation(const float fp_Rotation) { pm_Rotation = fp_Rotation; }
//        void SetScale(const sf::Vector2f& fp_Scale) { pm_Scale = fp_Scale; }
//        void SetTransform(const sf::Transform& fp_Transform) { pm_Transform = fp_Transform; }
//
//        // Translate (move) by an offset
//        void Translate(const sf::Vector2f& fp_Offset) { pm_Position += fp_Offset; }
//
//        // Rotate by an angle (in degrees)
//        void Rotate(const float& fp_Angle) { pm_Rotation += fp_Angle; }
//
//        // pm_Scale uniformly or non-uniformly
//        void ScaleBy(const sf::Vector2f& fp_Factor) { pm_Scale.x *= fp_Factor.x; pm_Scale.y *= fp_Factor.y; }
//
//        // Apply the transform to a point
//        sf::Vector2f TransformPoint(const sf::Vector2f& fp_Point) {
//            pm_Transform.translate(pm_Position);
//            pm_Transform.rotate(pm_Rotation);
//            pm_Transform.scale(pm_Scale);
//            return pm_Transform.transformPoint(fp_Point);
//        }
//    };
//}