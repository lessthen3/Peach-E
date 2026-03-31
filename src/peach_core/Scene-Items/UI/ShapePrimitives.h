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

///PeachCore
#include "../../Utils/Logger.h"
#include "../Transform.h"

///STL
#include <vector>
#include <cstdint>

///GLM
#include <glm/glm.hpp>

/*
the way this class works is that we create shape primitives for hit detection, however we always pass a quad to the shader for rendering,

Then the shader will remap the uv's according to the passed uniforms, since the gpu should handle drawing and software rendering is a waste of time in 2025

let the gpu do what it's good at and let the cpu do what it needs to do without impeding it

So the quad verts are completely static since inside the shader we pass a generic mat4 uniform as the transform, where all the data related to

Scale, Rotation, and Translation will be contained within, and will be applied as a right multiplication for drawing within the frag shader

this should dramatically reduce cpu overhead and complexity on the code side, and speed up UI stuff since the gpu will handle it.

Hit detection for mouse events will be handled by the cpu, and those calculations are much easier than creating a custom mesh for each component,

or generating a interpolated bezier to fit the shape we want, this way the shader can be much more accurate and allow for cooler customization

at virtually 0 cost to the cpu.


checking for a hit on a circle or doing a simple AABB check for a rect or both for a capsule should be much simpler and faster for a cpu.

for rounded corners for Rectangles, and Triangles, we'll just let the shader handle that as well and use a generic AABB for hit detection,

I can possibly add detection support for rounded corners, but just seems largely uneccessary and not super beneficial for UI stuff atm. 


For the UI, the shader graph will do a lot of heavy lifting, I just believe having a nice shader graph will be a much nicer workflow for me deving 
the engine as well as using peach-e

For example, not having to rewrite tedious geometry equations for common effects like wrap around style stripe movement, border outlines, etc; for
the engine side of things, it makes using templates a lot easier since the shader information will be coming in a predictable fmt::format dictated and enforced
by the shader graph. 

for anyone that wants to write glsl by hand I can just hand them a template and specify what can be done within peach-e's restraints, and I can sleep
easy with that decision since, if ur already that hardcore u wont mind and will hopefully understand the intention behind my design decisions and most
importantly agree with them for the most part. 

the shader graph will be a nice alternative for game designers or game makers that want maximum beauty for the minimum effort, since shaders should be
that. I find shader systems to be far too complicated and weird in other game engines i've used before, and i just want pretty fonts and colours >w<
especially for something that is heavily iterated on like overall aesthetic of a game. 
*/

//Maybe i should do a union or variant(typesafe union) to encap everything so i can stack alloc them or maybe i should just heap alloc
//and do a unique_ptr storage to avoid slicing and preserve memory footprint on the cpu cache since there could be a bunch of assets
//not sure, i mean they'll be held in a vector of unique_ptr's anyways idk needa be able to encap them in a type alias so that things like
// sliders can have a circular, oval, capsule or rectangular shape, dont think ill be seeing a triangle slider but ya never know game devs are wild

namespace PeachCore::PUI {

    //All data types here are designed to be default constructed for easier use as members

    //the same verts are used for all shapes for rendering purposes, uv's are dynamically remapped, it also represents the Bounding box for any ui element uwu, useful for alignment and bounds checking
    static constexpr float QUAD_VERTS[8] =
    {
        0.f, 0.f,
        1.f, 0.f,
        1.f, 1.f,
        0.f, 1.f
    };


    enum class ShapeType : uint8_t
    {
        NO_SHAPE = 0,
        Rectangle = 1 << 0,
        Circle = 1 << 1,
        Ellipse = 1 << 2,
        Capsule = 1 << 3,
        Triangle = 1 << 4,
        Polygon = 1 << 5
    };

    struct Shape
    {
        virtual ~Shape() = default;

        Shape(const ShapeType fp_ShapeType) : ShapeType(fp_ShapeType) {}

        //Position of the shape for use by Peach Engine so that a game dev/me can call a simple method like PUINode.move(new_vector) for the CPU side of things
        //The position is what the CPU uses for hit detection completely separate from whats drawn, but should very closely reflect the rendered position on screen

        //scale rotation and transform with respect to the GPU, so the transform is primarily for the shader to tell the GPU how to render the shape in terms of screen pixel coords
        //This transform operates on the QUAD_VERTS attribute and not the engine's interpretation of the Shape
        Transform2D Transform;
         
        const ShapeType ShapeType;

        virtual inline bool 
            IsWithin(const glm::vec2& fp_TestPoint) const = 0;
    };

    struct Rectangle final : public Shape//UwU
    {
        //(x, y) dictates top left corner, width and height dictate how far the bottom right vert is extended
        float pm_Width = 0.0f; 
        float pm_Height = 0.0f;
        
        Rectangle() : Shape(ShapeType::Rectangle) {}

        [[nodiscard]] inline bool
            IsWithin(const glm::vec2& fp_TestPoint)
            const noexcept override
        {
            return
            (
                (fp_TestPoint.x >= Transform.GetPosition().x and fp_TestPoint.x <= Transform.GetPosition().x + pm_Width)
                and
                (fp_TestPoint.y >= Transform.GetPosition().y and fp_TestPoint.y <= Transform.GetPosition().y + pm_Height) //plus because the pos is the top left corner is (0,0), and +'ve = below
            );
        }

        inline bool
            Resize(float fp_Width, float fp_Height, Logger*const logger)
            noexcept
        {
            if (fp_Width < 0.0f)
            {
                logger->Error("Tried to pass a negative value for width to a Rectangle shape primitive", "ShapePrimitive");
                return false;
            }
            else if (fp_Height < 0.0f)
            {
                logger->Error("Tried to pass a negative value for height to a Rectangle shape primitive", "ShapePrimitive");
                return false;
            }
            else
            {
                pm_Width = fp_Width;
                pm_Height = fp_Height;

                return true;
            }
        }
    };

    struct Circle final : public Shape //regular circle, whenever resized the proportions stay constant
    {
        //(x, y) dictates the center position of the circle
        float m_Radius = 0.0f;

        Circle() : Shape(ShapeType::Circle) {}

        [[nodiscard]] inline bool
            IsWithin(const glm::vec2& fp_TestPoint) //test whether the point we're trying to test is within the radius of the circle shape
            const noexcept override
        {
            return glm::distance(Transform.GetPosition(), fp_TestPoint) <= m_Radius;
        }

        inline bool
            ResizeRadius(float fp_NewRadiusSize, Logger*const logger)
            noexcept
        {
            if (fp_NewRadiusSize < 0.0f)
            {
                logger->Error("Tried to pass a negative value for radius to a Circle shape primitive", "ShapePrimitive");
                return false;
            }

            m_Radius = fp_NewRadiusSize;

            return true; 
        }

    };

    struct Ellipse final : public Shape //oval, can be squashed or stretched vertically or horizontally
    {
        //(x, y) dictates the center position of the ellipse

        //we need two points to define the major and minor axis of an ellipse so it can be resized appropriately, the m_Position variable just dictates the transform of the ellipse as a whole
        glm::vec2 m_MajorAxis{ 0.0f, 0.0f };
        glm::vec2 m_MinorAxis{ 0.0f, 0.0f };

        Ellipse() : Shape(ShapeType::Ellipse) {}


        [[nodiscard]] inline bool
            IsWithin(const glm::vec2& fp_TestPoint)
            const noexcept override
        {
            return true;
        }

    };

    struct Capsule final : public Shape //2D capsule shape, when scaled proportions are held constant
    {
        //(x, y) dictates the center position of the rectangle of the capsule idk this is up for debate

        Capsule() : Shape(ShapeType::Capsule) {}


        [[nodiscard]] inline bool
            IsWithin(const glm::vec2& fp_TestPoint)
            const noexcept override
        {
            return true;
        }
    };

    struct Triangle final : public Shape //twiangle rawr >O<, can be squashed or stretched as much as needed  
    {
        //(x, y) dictates the centroid position of the triangle

        glm::vec2 m_BaseLength{ 0.0f, 0.0f };
        glm::vec2 m_HeightLength{ 0.0f, 0.0f };

        Triangle() : Shape(ShapeType::Triangle) {}


        [[nodiscard]] inline bool
            IsWithin(const glm::vec2& fp_TestPoint)
            const noexcept override
        {
            return true;
        }
    };

    struct Polygon final : public Shape //twiangle rawr >O<, can be squashed or stretched as much as needed  
    {
        //(x, y) dictates the center position of the polygon, idk how to figure that out tbh

        vector<glm::vec2> pm_Vertices;

        Polygon() : Shape(ShapeType::Polygon) {}


        [[nodiscard]] inline bool
            IsWithin(const glm::vec2& fp_TestPoint)
            const noexcept override
        {
            return true;
        }

    };    
}// namespace PeachCore::PUI