//#pragma once
//
////#include <cmath>
//#include <string>
//
//#include "../../Managers/LogManager.h"
//
//namespace PeachCore {
//
//    struct Vector2i {
//
//        int x, y;
//
//        Vector2i(int fp_x = 0.0f, int fp_y = 0.0f) : x(fp_x), y(fp_y) {}
//
//        Vector2i operator*(const int fp_Scalar) const {
//            return Vector2i(x * fp_Scalar, y * fp_Scalar);
//        }
//
//        Vector2i& operator *= (const int fp_Other) {
//            x *= fp_Other;
//            y *= fp_Other;
//            return *this;
//        }
//
//        Vector2i operator+(const Vector2i& fp_Other) const {
//            return Vector2i(x + fp_Other.x, y + fp_Other.y);
//        }
//
//        Vector2i& operator+=(const Vector2i& fp_Other) {
//            x += fp_Other.x;
//            y += fp_Other.y;
//            return *this;
//        }
//
//        bool operator==(const Vector2i& fp_Other) const {
//            if (x == fp_Other.x && y == fp_Other.y) { return true; }
//            else { return false; }
//        }
//
//        inline void Normalize() 
//        {
//            float magnitude = std::sqrt(x*x + y*y);
//
//            if (magnitude == 0.0f)
//            {
//                LogManager::MainLogger().Warn("Attempted to normalize a Vector2i with 0 magnitude", "Vector2D");
//                return;
//            }
//            x /= magnitude;
//            y /= magnitude;
//        }
//
//        std::string ToString() const {
//            return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
//        }
//
//    };
//
//    struct Vector2f {
//        float x, y;
//
//        Vector2f(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
//
//        Vector2f operator*(float fp_Scalar) const {
//            return { x * fp_Scalar, y * fp_Scalar };
//        }
//
//        Vector2f& operator*=(float fp_Scalar) {
//            x *= fp_Scalar;
//            y *= fp_Scalar;
//            return *this;
//        }
//
//        Vector2f operator+(const Vector2f& fp_Other) const {
//            return { x + fp_Other.x, y + fp_Other.y };
//        }
//
//        Vector2f& operator+=(const Vector2f& fp_Other) {
//            x += fp_Other.x;
//            y += fp_Other.y;
//            return *this;
//        }
//
//        bool operator==(const Vector2f& fp_Other) const {
//            return x == fp_Other.x && y == fp_Other.y;
//        }
//
//        void Normalize() 
//        {
//            float magnitude = std::sqrt(x * x + y * y);
//
//            if (magnitude == 0.0f)
//            {
//                LogManager::MainLogger().Warn("Attempted to normalize a Vector2f with 0 magnitude", "Vector2D");
//                return;
//            }
//                x /= magnitude;
//                y /= magnitude;
//        }
//
//        Vector2f Normalized()
//        {
//            float magnitude = std::sqrt(x * x + y * y);
//
//            if (magnitude == 0.0f)
//            {
//                LogManager::MainLogger().Warn("Attempted to normalize a Vector2f with 0 magnitude", "Vector2D");
//                return Vector2f(); //returns 0-vector since thats the only vector that satisfies the 0 magnitude condition 
//            }
//            x /= magnitude;
//            y /= magnitude;
//
//            return Vector2f(x, y);
//        }
//
//        std::string ToString() const {
//            return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
//        }
//    };
//
//}