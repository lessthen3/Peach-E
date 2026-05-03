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

///cglm
#include <cglm/cglm.h>
#include <cglm/struct.h>

///STL
#include <string>
#include <vector>

namespace PeachCore::Test {
    static const std::vector<float> CUBE_VERTICES =
{
    // Front face (z = +1, normal = +Z)
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,

    // Back face (z = -1, normal = -Z)
     1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,

    // Left face (x = -1, normal = -X)
    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

    // Right face (x = +1, normal = +X)
     1.0f, -1.0f,  1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,

    // Top face (y = +1, normal = +Y)
    -1.0f,  1.0f,  1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,

    // Bottom face (y = -1, normal = -Y)
    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
};

// 6 faces × 2 triangles × 3 indices = 36 indices, CCW winding
static const std::vector<unsigned int> CUBE_INDICES =
{
     0,  1,  2,    0,  2,  3,   // Front
     4,  5,  6,    4,  6,  7,   // Back
     8,  9, 10,    8, 10, 11,   // Left
    12, 13, 14,   12, 14, 15,   // Right
    16, 17, 18,   16, 18, 19,   // Top
    20, 21, 22,   20, 22, 23,   // Bottom
};
}

namespace PeachCore {
    
    struct Vertex
    {
        vec3s Position;
        vec3s Normal;
        vec2s UV;
        vec4s Tangent; // vec4 because the w component stores the bitangent handedness sign
    };

    struct Material
    {

    };

    struct PBRMaterial
    {
        vec4s AlbedoColor;
        float Roughness;
        float Metallic;
        float AO;
        float EmissiveStrength;
    };

    struct MeshData 
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
        // material ids etc.
    };

    struct Bone
    {
        std::string Name;
        uint32_t ParentIndex;
        mat4s OffsetMatrix;
    };

    struct AnimationKeyframe
    {
        float Time;
        // transforms per bone…
    };

    struct AnimationClip 
    {
        std::string Name;
        float Duration;
        std::vector<AnimationKeyframe> Keyframes;
    };

    struct AnimationData 
    {
        std::vector<Bone> Bones;
        std::vector<AnimationClip> Clips;
    };
}
