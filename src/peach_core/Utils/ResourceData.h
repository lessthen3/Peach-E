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
#include "Serializer.h"

///GLM
#include <glm/glm.hpp>

///STL
#include <string>

namespace PeachCore
{
    using namespace std;

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 UV;
        glm::vec4 Tangent; // vec4 because the w component stores the bitangent handedness sign
    };

    struct Material
    {

    };

    struct PBRMaterial
    {
        glm::vec4 AlbedoColor;
        float Roughness;
        float Metallic;
        float AO;
        float EmissiveStrength;
    };

    struct MeshData 
    {
        vector<Vertex> Vertices;
        vector<uint32_t> Indices;
        // material ids etc.
    };

    struct Bone
    {
        string Name;
        int ParentIndex;
        glm::mat4 OffsetMatrix;
    };

    struct AnimationKeyframe
    {
        float Time;
        // transforms per bone…
    };

    struct AnimationClip 
    {
        string Name;
        float Duration;
        vector<AnimationKeyframe> Keyframes;
    };

    struct AnimationData 
    {
        vector<Bone> Bones;
        vector<AnimationClip> Clips;
    };
}
