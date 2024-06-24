#pragma once

#include <SFML/Graphics.hpp>
#include "PeachEObject.h"

struct Transform2D: public MinGEObject {
private:
    sf::Vector2f position;
    float rotation;
    sf::Vector2f Scale;
    sf::Transform Transform;

public:
    Transform2D()
        : position(sf::Vector2f(0.0f, 0.0f)), rotation(0.0f), Scale(sf::Vector2f(1.0f, 1.0f)) {}

    Transform2D(const sf::Vector2f& pos, float rot, const sf::Vector2f& scl)
        : position(pos), rotation(rot), Scale(scl) {}

    // Getters
    const sf::Vector2f& GetPosition() const { return position; }
    const float GetRotation() const { return rotation; }
    const sf::Vector2f& GetScale() const { return Scale; }
    const sf::Transform& GetTransform() const { return Transform; }

    // Setters
    void SetPosition(const sf::Vector2f& pos) { position = pos; }
    void SetRotation(const float rot) { rotation = rot; }
    void SetScale(const sf::Vector2f& scl) { Scale = scl; }
    void SetTransform(const sf::Transform& fp_transform) { Transform = fp_transform; }

    // Translate (move) by an offset
    void Translate(const sf::Vector2f& offset) { position += offset; }

    // Rotate by an angle (in degrees)
    void Rotate(const float& angle) { rotation += angle; }

    // Scale uniformly or non-uniformly
    void ScaleBy(const sf::Vector2f& factor) { Scale.x *= factor.x; Scale.y *= factor.y; }

    // Apply the transform to a point
    sf::Vector2f TransformPoint(const sf::Vector2f& point) {
        Transform.translate(position);
        Transform.rotate(rotation);
        Transform.scale(Scale);
        return Transform.transformPoint(point);
    }
};
