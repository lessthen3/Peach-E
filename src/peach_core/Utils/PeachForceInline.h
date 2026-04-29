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


#if defined(_MSC_VER)
    #define PEACH_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define PEACH_FORCEINLINE inline __attribute__((always_inline))
#else
    // Fallback for anything else
    #define PEACH_FORCEINLINE inline
#endif