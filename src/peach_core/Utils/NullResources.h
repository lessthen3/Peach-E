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

#include <cstdint>

namespace PeachCore::NullResources {

    using namespace std;

    extern const unsigned char PEACH_NULL_TEXTURE[];

    extern const unsigned char PEACH_NULL_FONT[];

    [[nodiscard]] extern constexpr size_t
        GetDefaultTextureSize();

    [[nodiscard]] extern constexpr size_t
        GetDefaultFontSize();
}