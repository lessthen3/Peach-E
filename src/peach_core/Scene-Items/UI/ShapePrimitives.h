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
#pragma once

///PeachCore
#include "Utils/Logger.h"

///STL
#include <vector>
#include <variant>
#include <type_traits>

///CSTDL
#include <stdint.h>

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


/*
 *  ShapePrimitives — pure CPU-side geometry for hit detection / layout / AABB queries.
 *
 *  Design intentions:
 *   - ZERO GPU coupling. These structs never see a renderer. The renderer always
 *     gets a quad + a mat4 uniform. What we describe here is the *logical* shape
 *     the CPU uses for IsWithin, AABB, winding order, etc.
 *
 *   - ZERO heap per shape (except PolygonShape whose vertex list is inherently
 *     dynamic — that's unavoidable and fine since polygons are rare in UI).
 *
 *   - NO virtual dispatch. std::visit over the variant compiles to a jump table
 *     (one indexed indirect branch), same overhead as a vtable call minus the
 *     extra pointer dereference to the heap object. We also eliminate the
 *     unique_ptr indirection entirely.
 *
 *   - Shapes carry NO Transform2D. A Transform2D with a glm::mat4 inside it
 *     is ~100 bytes of GPU-oriented data that has zero business being in a
 *     struct whose only job is CPU point-in-shape tests. Position lives on
 *     the PUI Node. IsWithin and GetAABB take an fp_Origin explicitly.
 *
 *   - ShapeType enum kept for external API clarity (readable switch statements
 *     in game code, serialization, editor inspector). Bitmask removed — these
 *     are never ORed, sequential values are correct.
 *
 *   sizeof(ShapePrimitive) breakdown:
 *     largest member  = TriangleShape (3x glm::vec2 = 24 bytes)
 *     variant overhead = 8 bytes (discriminant + alignment padding)
 *     total           ≈ 32 bytes
 *
 *   Old approach: 8 bytes (unique_ptr) + ~150 bytes scattered on heap (Transform2D
 *   + vtable ptr + shape params). New approach: 32 bytes, inline, cache-local. uwu
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


    // ─── ShapeType enum ────────────────────────────────────────────────────────
      // Sequential, not bitmask. Useful for serialization, inspector, external APIs.
      // std::variant::index() is the runtime truth — this is just a readable alias.

    enum class ShapeType : uint8_t
    {
        NoShape = 0,
        Rectangle = 1,
        Circle = 2,
        Ellipse = 3,
        Capsule = 4,
        Triangle = 5,
        Polygon = 6
    };

    // ─── Geometry parameter structs ────────────────────────────────────────────
   // These are pure data. No methods, no virtuals, no transforms.
   // All measurements are in local/node space — fp_Origin offsets them at call time.

    struct RectShape
    {
        float Width = 0.f;  // extends right  from fp_Origin
        float Height = 0.f;  // extends downward from fp_Origin (Y-down screen space)
    };

    struct CircleShape
    {
        float Radius = 0.f;  // fp_Origin = center
    };

    struct EllipseShape
    {
        float SemiMajor = 0.f;  // half-width  along X, fp_Origin = center
        float SemiMinor = 0.f;  // half-height along Y
    };

    struct CapsuleShape
    {
        // Both centers in *local* space — add fp_Origin at call time.
        // Capsule = Minkowski sum of line segment + circle of Radius.
        // Typical vertical capsule: LocalCenter1 = {0, -HalfLen}, LocalCenter2 = {0, +HalfLen}
        glm::vec2 LocalCenter1{ 0.f,  0.5f };
        glm::vec2 LocalCenter2{ 0.f, -0.5f };
        float     Radius = 0.f;
    };

    struct TriangleShape
    {
        // Vertices in *local* space, CCW winding (required for IsWithin).
        // Default: equilateral with centroid at origin-ish.
        glm::vec2 A{ 0.f,  1.f };
        glm::vec2 B{ 1.f, -1.f };
        glm::vec2 C{ -1.f, -1.f };
    };

    struct PolygonShape
    {
        // Vertices in *local* space, CCW winding REQUIRED — call AssertWindingCCW()
        // after construction. Convex polygons only for IsWithin. For concave UI shapes,
        // decompose at load time (editor does this) and store as multiple PolygonShapes.
        std::vector<glm::vec2> Vertices;

        // Computes signed area via shoelace, flips winding in-place if CW.
        // Call after constructing from external data (e.g. editor polygon tool).
        void
            AssertWindingCCW()
            noexcept
        {
            if (Vertices.size() < 3)
            {
                return;
            }

            // Shoelace formula — positive area = CCW in Y-down screen space
            float f_SignedArea = 0.f;

            for (size_t lv_I = 0; lv_I < Vertices.size(); ++lv_I)
            {
                const glm::vec2& fv_Curr = Vertices[lv_I];
                const glm::vec2& fv_Next = Vertices[(lv_I + 1) % Vertices.size()];
                f_SignedArea += (fv_Curr.x * fv_Next.y) - (fv_Next.x * fv_Curr.y);
            }

            // NOTE: in Y-down screen space CCW gives *negative* signed area by the
            // standard math convention. If you're using Y-up (glm default world space),
            // flip the comparison. For PUI screen space: negative = CCW = correct.
            if (f_SignedArea > 0.f) // CW — flip
            {
                std::reverse(Vertices.begin(), Vertices.end());
            }
        }
    };


    // ─── ShapePrimitive ────────────────────────────────────────────────────────
    // The actual storage type. Put this inline in your PUI nodes — no unique_ptr,
    // no heap, no pointer chase.

    using ShapePrimitive = std::variant
    <
        RectShape,
        CircleShape,
        EllipseShape,
        CapsuleShape,
        TriangleShape,
        PolygonShape
    >;

    // ─── Free-function interface ───────────────────────────────────────────────
    // These replace virtual dispatch. std::visit compiles to an indexed jump table.
    // fp_Origin = the node's world-space position (top-left for Rect/Triangle/Polygon,
    //             center for Circle/Ellipse/Capsule — match how the GPU interprets it).

    // IsWithin — CPU hit detection for mouse events, focus, etc.
    [[nodiscard]] bool
        IsWithin
        (
            const ShapePrimitive& fp_Shape,
            const glm::vec2& fp_Origin,
            const glm::vec2& fp_TestPoint
        )
        noexcept;

    // GetAABB — returns {x, y, width, height} in world space.
    // Used for layout, dirty-region culling, focus rings, tooltips.
    [[nodiscard]] glm::vec4
        GetAABB
        (
            const ShapePrimitive& fp_Shape,
            const glm::vec2& fp_Origin
        )
        noexcept;

    // GetShapeType — readable enum from variant discriminant.
    // Useful for serialization / editor inspector without a full visit.
    [[nodiscard]] ShapeType
        GetShapeType(const ShapePrimitive& fp_Shape)
        noexcept;

    // GetLocalVertices — debug/editor only, returns logical outline vertices.
    // Returns heap-allocated vector — this is a cold-path query, that's fine.
    [[nodiscard]] std::vector<glm::vec2>
        GetLocalVertices(const ShapePrimitive& fp_Shape)
        noexcept;

    // Resize helpers — free functions keep the shape structs as plain data.
    // Return false + log on invalid input (negative dimensions etc).
    [[nodiscard]] bool ResizeRect(RectShape& fp_Shape, float fp_W, float fp_H, Logger* fp_Logger) noexcept;
    [[nodiscard]] bool ResizeCircle(CircleShape& fp_Shape, float fp_Radius, Logger* fp_Logger) noexcept;
    [[nodiscard]] bool ResizeEllipse(EllipseShape& fp_Shape, float fp_Major, float fp_Minor, Logger* fp_Logger) noexcept;
    [[nodiscard]] bool ResizeCapsule(CapsuleShape& fp_Shape, float fp_Radius, Logger* fp_Logger) noexcept;

}// namespace PeachCore::PUI