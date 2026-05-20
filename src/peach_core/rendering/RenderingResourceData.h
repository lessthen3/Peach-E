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
