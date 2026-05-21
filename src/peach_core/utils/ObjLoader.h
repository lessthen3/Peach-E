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

///CSTD
#include <stdint.h>

///STL
#include <vector>

///PeachCore
#include "debug/StatusCodes.h"
#include "debug/Logger.h"

namespace PeachCore::OBJ {

    constexpr int32_t IDX_NO_VALUE = -1;

    using StringVector = std::vector<std::string>; //2 layers of std is too much for me, any amount of stds is bad in any context >w<

    struct InterleavedMesh //interleaved {v3pos, v2uv, v3normals, v4tangent}
    {
        std::vector<float>     Vertices;
        std::vector<uint32_t> Indices;
    };


    PEACH_STATUS_CODE
        LoadMesh
        (
            std::string fp_ObjFilePathOwO,
            InterleavedMesh& fp_OutMesh,
            Logger* logger
        );
}
