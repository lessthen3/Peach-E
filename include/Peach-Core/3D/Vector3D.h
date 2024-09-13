#pragma once

#include <string>

#include "../../Managers/LogManager.h"


namespace PeachCore {

    struct Vector3 {
        float X, Y, Z;

        Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : X(x), Y(y), Z(z) {}

        Vector3 operator*(float scalar) const {
            return { X * scalar, Y * scalar, Z * scalar };
        }

        Vector3& operator*=(float scalar) {
            X *= scalar;
            Y *= scalar;
            Z *= scalar;
            return *this;
        }

        Vector3 operator+(const Vector3& other) const {
            return { X + other.X, Y + other.Y, Z + other.Z };
        }

        Vector3& operator+=(const Vector3& other) {
            X += other.X;
            Y += other.Y;
            Z += other.Z;
            return *this;
        }

        bool operator==(const Vector3& other) const {
            return X == other.X && Y == other.Y && Z == other.Z;
        }

        void normalize() {
            float magnitude = sqrt(X * X + Y * Y + Z * Z);
            if (magnitude == 0.0f) 
            { 
                LogManager::MainLogger().Warn("Attempted to normalize a Vector3f with 0 magnitude", "Vector3D");
                return; 
            }
            
            X /= magnitude;
            Y /= magnitude;
            Z /= magnitude;
            
        }

        std::string toString() const {
            return "(" + std::to_string(X) + ", " + std::to_string(Y) + ", " + std::to_string(Z) + ")";
        }
    };
}