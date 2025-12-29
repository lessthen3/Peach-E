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
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        // tangents, etc.
    };

    struct Material
    {

    };

    struct MeshData 
    {
        vector<Vertex>   vertices;
        vector<uint32_t> indices;
        // material ids etc.
    };

    struct Bone
    {
        string name;
        int parentIndex;
        glm::mat4 offsetMatrix;
    };

    struct AnimationKeyframe
    {
        float time;
        // transforms per bone…
    };

    struct AnimationClip 
    {
        string name;
        float duration;
        vector<AnimationKeyframe> keyframes;
    };

    struct AnimationData 
    {
        vector<Bone> bones;
        vector<AnimationClip> clips;
    };
}
