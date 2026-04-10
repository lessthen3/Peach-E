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
#include "ShapePrimitives.h"

#include <algorithm>  // std::reverse, std::clamp
#include <array>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>  // for glm::length2

namespace PeachCore::PUI {

    /*
        everything is calculated using screen coords; so we doing 1st quadrant flipped into the 4th quadrant, or just an inverted cartesian plane

        this approach makes geometry make more sense; one less place for our rendering code to break, and as a bonus avoids a small conversion flipping basis mapping
    */

    // ─── IsWithin ──────────────────────────────────────────────────────────────

    bool
        IsWithin
        (
            const ShapePrimitive& fp_Shape,
            const glm::vec2& fp_Origin,
            const glm::vec2& fp_TestPoint
        )
        noexcept
    {
        return std::visit([&fp_Origin, &fp_TestPoint](const auto& fv_S) noexcept -> bool
            {
                using T = std::decay_t<decltype(fv_S)>;

                if constexpr (std::is_same_v<T, RectShape>)
                {
                    // AABB — top-left origin, Y-down
                    return 
                        (fp_TestPoint.x >= fp_Origin.x) and
                        (fp_TestPoint.x <= fp_Origin.x + fv_S.Width) and
                        (fp_TestPoint.y >= fp_Origin.y) and
                        (fp_TestPoint.y <= fp_Origin.y + fv_S.Height);
                }
                else if constexpr (std::is_same_v<T, CircleShape>)
                {
                    // glm::length2 avoids a sqrt — compare squared distance to squared radius
                    return glm::length2(fp_TestPoint - fp_Origin) <= (fv_S.Radius * fv_S.Radius);
                }
                else if constexpr (std::is_same_v<T, EllipseShape>)
                {
                    // Standard axis-aligned ellipse test:
                    // (dx/a)^2 + (dy/b)^2 <= 1  where a=SemiMajor, b=SemiMinor
                    // fp_Origin = center
                    if (fv_S.SemiMajor == 0.f || fv_S.SemiMinor == 0.f)
                    {
                        return false;
                    }

                    glm::vec2 f_D = fp_TestPoint - fp_Origin;
                    float     f_Nx = f_D.x / fv_S.SemiMajor;
                    float     f_Ny = f_D.y / fv_S.SemiMinor;

                    return (f_Nx * f_Nx + f_Ny * f_Ny) <= 1.f;
                }
                else if constexpr (std::is_same_v<T, CapsuleShape>)
                {
                    // Capsule = Minkowski sum of segment + disc of Radius.
                    // Test: distance from fp_TestPoint to closest point on segment <= Radius.

                    glm::vec2 f_C1 = fp_Origin + fv_S.LocalCenter1;
                    glm::vec2 f_C2 = fp_Origin + fv_S.LocalCenter2;
                    glm::vec2 f_AB = f_C2 - f_C1;

                    float f_LenSq = glm::length2(f_AB);

                    glm::vec2 f_Closest;

                    if (f_LenSq < 1e-8f) // degenerate capsule (zero-length segment) = circle
                    {
                        f_Closest = f_C1;
                    }
                    else
                    {
                        float f_T = glm::clamp(glm::dot(fp_TestPoint - f_C1, f_AB) / f_LenSq, 0.f, 1.f);
                        f_Closest = f_C1 + f_T * f_AB;
                    }

                    return glm::length2(fp_TestPoint - f_Closest) <= (fv_S.Radius * fv_S.Radius);
                }
                else if constexpr (std::is_same_v<T, TriangleShape>)
                {
                    // Barycentric coordinates — same algorithm as before but vertices
                    // are offset by fp_Origin so the triangle lives in world space.
                    glm::vec2 f_A = fp_Origin + fv_S.A;
                    glm::vec2 f_B = fp_Origin + fv_S.B;
                    glm::vec2 f_C = fp_Origin + fv_S.C;

                    glm::vec2 v0 = f_C - f_A;
                    glm::vec2 v1 = f_B - f_A;
                    glm::vec2 v2 = fp_TestPoint - f_A;

                    float f_D00 = glm::dot(v0, v0);
                    float f_D01 = glm::dot(v0, v1);
                    float f_D02 = glm::dot(v0, v2);
                    float f_D11 = glm::dot(v1, v1);
                    float f_D12 = glm::dot(v1, v2);

                    float f_Denom = f_D00 * f_D11 - f_D01 * f_D01;

                    if (glm::abs(f_Denom) < 1e-8f) // degenerate triangle
                    {
                        return false;
                    }

                    float f_InvD = 1.f / f_Denom;
                    float u = (f_D11 * f_D02 - f_D01 * f_D12) * f_InvD;
                    float v = (f_D00 * f_D12 - f_D01 * f_D02) * f_InvD;

                    return (u >= 0.f) && (v >= 0.f) && (u + v < 1.f);
                }
                else if constexpr (std::is_same_v<T, PolygonShape>)
                {
                    // Left-of-every-edge test — only correct for CONVEX polygons with CCW winding.
                    // For Y-down screen space, CCW means cross product (edge x toPoint) should be
                    // *negative* (because Y is flipped vs. standard math).
                    // If you decompose concave polys in the editor into convex pieces, each piece
                    // uses this test independently and OR's the results.

                    const auto& f_Verts = fv_S.Vertices;
                    size_t      f_N = f_Verts.size();

                    if (f_N < 3)
                    {
                        return false;
                    }

                    for (size_t lv_I = 0; lv_I < f_N; ++lv_I)
                    {
                        glm::vec2 fv_Curr = fp_Origin + f_Verts[lv_I];
                        glm::vec2 fv_Next = fp_Origin + f_Verts[(lv_I + 1) % f_N];

                        glm::vec2 fv_Edge = fv_Next - fv_Curr;
                        glm::vec2 fv_ToPoint = fp_TestPoint - fv_Curr;

                        // 2D cross product (z component of 3D cross)
                        // Y-down screen space: point is "inside" when cross < 0 for CCW winding
                        if ((fv_Edge.x * fv_ToPoint.y - fv_Edge.y * fv_ToPoint.x) > 0.f)
                        {
                            return false;
                        }
                    }

                    return true;
                }

            }, fp_Shape);
    }


    // ─── GetAABB ───────────────────────────────────────────────────────────────
    // Returns {origin.x, origin.y, width, height} in world space. owo

    glm::vec4
        GetAABB
        (
            const ShapePrimitive& fp_Shape,
            const glm::vec2& fp_Origin
        )
        noexcept
    {
        return std::visit([&fp_Origin](const auto& fv_S) noexcept -> glm::vec4
            {
                using T = std::decay_t<decltype(fv_S)>;

                if constexpr (std::is_same_v<T, RectShape>)
                {
                    return { fp_Origin.x, fp_Origin.y, fv_S.Width, fv_S.Height };
                }
                else if constexpr (std::is_same_v<T, CircleShape>)
                {
                    return
                    {
                        fp_Origin.x - fv_S.Radius,
                        fp_Origin.y - fv_S.Radius,
                        fv_S.Radius * 2.f,
                        fv_S.Radius * 2.f
                    };
                }
                else if constexpr (std::is_same_v<T, EllipseShape>)
                {
                    return
                    {
                        fp_Origin.x - fv_S.SemiMajor,
                        fp_Origin.y - fv_S.SemiMinor,
                        fv_S.SemiMajor * 2.f,
                        fv_S.SemiMinor * 2.f
                    };
                }
                else if constexpr (std::is_same_v<T, CapsuleShape>)
                {
                    glm::vec2 f_C1 = fp_Origin + fv_S.LocalCenter1;
                    glm::vec2 f_C2 = fp_Origin + fv_S.LocalCenter2;
                    float f_MinX = std::min(f_C1.x, f_C2.x) - fv_S.Radius;
                    float f_MinY = std::min(f_C1.y, f_C2.y) - fv_S.Radius;
                    float f_MaxX = std::max(f_C1.x, f_C2.x) + fv_S.Radius;
                    float f_MaxY = std::max(f_C1.y, f_C2.y) + fv_S.Radius;
                    return { f_MinX, f_MinY, f_MaxX - f_MinX, f_MaxY - f_MinY };
                }
                else if constexpr (std::is_same_v<T, TriangleShape>)
                {
                    float f_MinX = std::min({ fv_S.A.x, fv_S.B.x, fv_S.C.x }) + fp_Origin.x;
                    float f_MinY = std::min({ fv_S.A.y, fv_S.B.y, fv_S.C.y }) + fp_Origin.y;
                    float f_MaxX = std::max({ fv_S.A.x, fv_S.B.x, fv_S.C.x }) + fp_Origin.x;
                    float f_MaxY = std::max({ fv_S.A.y, fv_S.B.y, fv_S.C.y }) + fp_Origin.y;
                    return { f_MinX, f_MinY, f_MaxX - f_MinX, f_MaxY - f_MinY };
                }
                else if constexpr (std::is_same_v<T, PolygonShape>)
                {
                    if (fv_S.Vertices.empty())
                    {
                        return { fp_Origin.x, fp_Origin.y, 0.f, 0.f };
                    }

                    float f_MinX = 1e30f, f_MinY = 1e30f;
                    float f_MaxX = -1e30f, f_MaxY = -1e30f;

                    for (const auto& fv_V : fv_S.Vertices)
                    {
                        float fv_Wx = fp_Origin.x + fv_V.x;
                        float fv_Wy = fp_Origin.y + fv_V.y;
                        f_MinX = std::min(f_MinX, fv_Wx);
                        f_MinY = std::min(f_MinY, fv_Wy);
                        f_MaxX = std::max(f_MaxX, fv_Wx);
                        f_MaxY = std::max(f_MaxY, fv_Wy);
                    }

                    return { f_MinX, f_MinY, f_MaxX - f_MinX, f_MaxY - f_MinY };
                }

            }, fp_Shape);
    }


    // ─── GetShapeType ──────────────────────────────────────────────────────────

    ShapeType
        GetShapeType(const ShapePrimitive& fp_Shape)
        noexcept
    {
        // variant::index() matches the order of the using declaration in the header.
        // static_assert below keeps them in sync — will scream at compile time if drift owo
        static_assert(std::variant_size_v<ShapePrimitive> == 6, "ShapePrimitive variant member count changed — update GetShapeType() and ShapeType enum");

        constexpr std::array<ShapeType, 6> f_Map =
        {
            ShapeType::Rectangle,
            ShapeType::Circle,
            ShapeType::Ellipse,
            ShapeType::Capsule,
            ShapeType::Triangle,
            ShapeType::Polygon
        };

        return f_Map[fp_Shape.index()];
    }


    // ─── GetLocalVertices ─────────────────────────────────────────────────────
    // Cold-path query for debug overlays / editor hit-shape visualizer.
    // Heap alloc is fine here — don't call this on the hot path plz kitten >w<

    std::vector<glm::vec2>
        GetLocalVertices(const ShapePrimitive& fp_Shape) // try to constexpr shapes, but this is mostly runtime based on what the dev wants owo
        noexcept
    {
        return std::visit([](const auto& fv_S) -> std::vector<glm::vec2>
            {
                using T = std::decay_t<decltype(fv_S)>;

                if constexpr (std::is_same_v<T, RectShape>)
                {
                    return
                    {
                        { 0.f,         0.f          },
                        { fv_S.Width,  0.f          },
                        { fv_S.Width,  fv_S.Height  },
                        { 0.f,         fv_S.Height  }
                    };
                }
                else if constexpr (std::is_same_v<T, CircleShape>)
                {
                    // 16-segment circle approximation for debug overlay — plenty for UI
                    constexpr int    f_Segs = 16;
                    constexpr float  f_Step = 6.283185f / f_Segs; // 2π / 16
                    std::vector<glm::vec2> f_Out;
                    f_Out.reserve(f_Segs);

                    for (int lv_I = 0; lv_I < f_Segs; ++lv_I)
                    {
                        float fv_Ang = lv_I * f_Step;
                        f_Out.push_back({ fv_S.Radius * std::cos(fv_Ang), fv_S.Radius * std::sin(fv_Ang) });
                    }

                    return f_Out;
                }
                else if constexpr (std::is_same_v<T, EllipseShape>)
                {
                    constexpr int   f_Segs = 16;
                    constexpr float f_Step = 6.283185f / f_Segs;
                    std::vector<glm::vec2> f_Out;
                    f_Out.reserve(f_Segs);

                    for (int lv_I = 0; lv_I < f_Segs; ++lv_I)
                    {
                        float fv_Ang = lv_I * f_Step;
                        f_Out.push_back({ fv_S.SemiMajor * std::cos(fv_Ang), fv_S.SemiMinor * std::sin(fv_Ang) });
                    }

                    return f_Out;
                }
                else if constexpr (std::is_same_v<T, TriangleShape>)
                {
                    return { fv_S.A, fv_S.B, fv_S.C };
                }
                else if constexpr (std::is_same_v<T, CapsuleShape>)
                {
                    // Return the two center points + radius as a hint — editor can draw its own capsule gizmo
                    return { fv_S.LocalCenter1, fv_S.LocalCenter2 };
                }
                else if constexpr (std::is_same_v<T, PolygonShape>)
                {
                    return fv_S.Vertices; // already local, just copy
                }

            }, fp_Shape);
    }


    // ─── Resize helpers ────────────────────────────────────────────────────────

    bool
        ResizeRect(RectShape& fp_Shape, float fp_W, float fp_H, Logger* fp_Logger)
        noexcept
    {
        if (fp_W < 0.f || fp_H < 0.f)
        {
            fp_Logger->Error("Negative dimension passed to ResizeRect", "ShapePrimitive");
            return false;
        }

        fp_Shape.Width = fp_W;
        fp_Shape.Height = fp_H;
        return true;
    }

    bool
        ResizeCircle(CircleShape& fp_Shape, float fp_Radius, Logger* fp_Logger)
        noexcept
    {
        if (fp_Radius < 0.f)
        {
            fp_Logger->Error("Negative radius passed to ResizeCircle", "ShapePrimitive");
            return false;
        }

        fp_Shape.Radius = fp_Radius;
        return true;
    }

    bool
        ResizeEllipse(EllipseShape& fp_Shape, float fp_Major, float fp_Minor, Logger* fp_Logger)
        noexcept
    {
        if (fp_Major < 0.f || fp_Minor < 0.f)
        {
            fp_Logger->Error("Negative semi-axis passed to ResizeEllipse", "ShapePrimitive");
            return false;
        }

        fp_Shape.SemiMajor = fp_Major;
        fp_Shape.SemiMinor = fp_Minor;
        return true;
    }

    bool
        ResizeCapsule(CapsuleShape& fp_Shape, float fp_Radius, Logger* fp_Logger)
        noexcept
    {
        if (fp_Radius < 0.f)
        {
            fp_Logger->Error("Negative radius passed to ResizeCapsule", "ShapePrimitive");
            return false;
        }

        fp_Shape.Radius = fp_Radius;
        return true;
    }

} // namespace PeachCore::PUI