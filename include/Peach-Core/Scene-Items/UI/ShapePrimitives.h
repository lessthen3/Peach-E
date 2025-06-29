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

#include <vector>
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
the engine side of things, it makes using templates a lot easier since the shader information will be coming in a predictable format dictated and enforced
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

namespace PeachCore{
namespace PUI{

    //All data types here are designed to be default constructed for easier use as members

    static constexpr glm::vec2 QUAD_VERTS[4] = //the same verts are used for all shapes for rendering purposes, uv's are dynamically remapped
    {
        {0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}
    };


    struct Rectangle //UwU
    {
        glm::vec2 m_Position; //(x, y) dictates top left corner, width and height dictate how far the bottom right vert is extended
        float Width = 0.0f; 
        float Height = 0.0f;

        inline bool
            IsWithinRectangle(glm::vec2 fp_TestPoint)
            const noexcept
        {
            return
                (
                    (abs(fp_TestPoint.x) >= abs(m_Position.x) and abs(fp_TestPoint.x) <= (abs(m_Position.x) + abs(Width)))
                    and
                    (abs(fp_TestPoint.y) >= abs(m_Position.y) and abs(fp_TestPoint.y) <= (abs(m_Position.y) + abs(Height)))
                );
        }
    };

    struct Circle //regular circle, whenever resized the proportions stay constant
    {
        glm::mat4 m_Transform;
        float m_Radius = 0.0f;

        glm::vec2 m_CenterPosition;

        inline bool
            IsWithinCircle(glm::vec2 fp_TestPoint) //test whether the point we're trying to test is within the radius of the circle shape
            const
            {
                return glm::distance(m_CenterPosition, fp_TestPoint) <= m_Radius;
            }

        inline void
            ResizeRadius(float fp_NewRadiusSize)
            noexcept
            {
                m_Radius = fp_NewRadiusSize;
            }

    };

    struct Ellipse //oval, can be squashed or stretched vertically or horizontally
    {
        glm::mat4 m_Transform;

        glm::vec2 m_Center;
        glm::vec2 m_MajorAxis;
        glm::vec2 m_MinorAxis;

        inline bool
            IsWithinEllipse(glm::vec2 fp_TestPoint)
            {
                return true;
            }

    };

    struct Capsule //2D capsule shape, when scaled proportions are held constant
    {
        glm::mat4 m_Transform;

        inline bool
            IsWithinCapsule(glm::vec2 fp_TestPoint)
            {
                return true;
            }

    };

    struct Triangle //twiangle rawr >O<, can be squashed or stretched as much as needed  
    {
        glm::mat4 m_Transform;

        glm::vec2 m_BaseLength;
        glm::vec2 m_HeightLength;

        inline bool
            IsWithinTriangle(glm::vec2 fp_TestPoint)
            {
                return true;
            }
    };


    
}// namespace PUI
}// namespace PeachCore