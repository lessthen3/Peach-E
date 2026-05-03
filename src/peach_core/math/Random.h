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

#include <stdint.h>
#include <stddef.h>
#include "utils/PeachForceInline.h"
#include "utils/PeachPanic.h"

namespace PeachCore::Math{
// Platform secure random, always delegates to OS entropy source.

#if defined(PEACH_PLATFORM_VITA)
#   include <psp2/kernel/rng.h>

    // sceKernelGetRandomNumber caps at 64 bytes per call so we chunk it
    PEACH_FORCEINLINE void 
        SecureRandomNumber
        (
            uint8_t* fp_Buffer, 
            const size_t fp_Size
        )
    {
        PEACH_PANIC_IF(not fp_Buffer, "Tried to pass nullptr ref to Math::SecureRandomNumber()");

        size_t f_Remaining = fp_Size;
        uint8_t* f_Cursor = fp_Buffer;

        while (f_Remaining > 0)
        {
            size_t f_ChunkSize = f_Remaining < 64 ? f_Remaining : 64;
            sceKernelGetRandomNumber(f_Cursor, static_cast<SceSize>(f_ChunkSize));
            f_Cursor    += f_ChunkSize;
            f_Remaining -= f_ChunkSize;
        }
    }

#elif defined(PEACH_PLATFORM_IS_WINDOWS) //dunno if i wanna inline this one cause i dont wanna pollute TUs with windows.h here owo
#   include <windows.h>
#   include <bcrypt.h>
#   pragma comment(lib, "bcrypt.lib")

    PEACH_FORCEINLINE void 
        SecureRandomNumber
        (
            uint8_t* fp_Buffer, 
            const size_t fp_Size
        )
    {
        PEACH_PANIC_IF(not fp_Buffer, "Tried to pass nullptr ref to Math::SecureRandomNumber()");

        BCryptGenRandom
        (
            nullptr,
            fp_Buffer,
            static_cast<ULONG>(fp_Size),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG
        );
    }

#elif defined(PEACH_PLATFORM_MACOS) || defined(PEACH_PLATFORM_IOS) || defined(PEACH_PLATFORM_TVOS) || defined(PEACH_PLATFORM_FREEBSD)
#   include <stdlib.h>

    // arc4random_buf is always available on Apple + BSD, never blocks, no fd to manage, seeded by the kernel automatically

    PEACH_FORCEINLINE void 
        SecureRandomNumber
        (
            uint8_t* fp_Buffer, 
            const size_t fp_Size
        )
    {
        PEACH_PANIC_IF(not fp_Buffer, "Tried to pass nullptr ref to Math::SecureRandomNumber()");

        arc4random_buf(fp_Buffer, fp_Size);
    }

#elif defined(PEACH_PLATFORM_LINUX)  || defined(PEACH_PLATFORM_ANDROID)
#   include <sys/random.h>

    PEACH_FORCEINLINE void 
        SecureRandomNumber
        (
            uint8_t* fp_Buffer, 
            const size_t fp_Size
        )
    {
        PEACH_PANIC_IF(not fp_Buffer, "Tried to pass nullptr ref to Math::SecureRandomNumber()");

        size_t f_Remaining = fp_Size;
        uint8_t* f_Cursor  = fp_Buffer;

        // getrandom is capped at 256 bytes per call when using /dev/urandom
        // (GRND_NONBLOCK), chunk it to be safe across all kernel versions
        while (f_Remaining > 0)
        {
            size_t wv_ChunkSize = f_Remaining < 256 ? f_Remaining : 256;

            ssize_t wv_Result = getrandom(f_Cursor, wv_ChunkSize, 0);
            if (wv_Result < 0) [[unlikely]]
            {
                if (errno == EINTR) 
                {                    
                    continue; // Interrupted, try again
                } 
                
                // Fixed-size stack buffer, no heap allocation in the event of some catastrophic kernel event
                char f_ErrBuf[128];
                
                // GNU version of strerror_r returns a char*
                // XSI version returns an int. This is the "safe" way to get the text.
                const char* f_ErrMsg = strerror_r(errno, f_ErrBuf, sizeof(f_ErrBuf));

                PEACH_PANIC_IF(false && "getrandom failed!", f_ErrMsg);
            }

            f_Cursor    += wv_Result;
            f_Remaining -= wv_Result;
        }
    }

#elif defined(PEACH_PLATFORM_HAIKU)
#   include <cstdio> // Haiku doesn't have getrandom, /dev/urandom is the correct path

    PEACH_FORCEINLINE void 
        SecureRandomNumber
        (
            uint8_t* fp_Buffer, 
            const size_t fp_Size
        )
    {
        PEACH_PANIC_IF(not fp_Buffer, "Tried to pass nullptr ref to Math::SecureRandomNumber()");

        FILE* f_Urandom = fopen("/dev/urandom", "rb");
        fread(fp_Buffer, 1, fp_Size, f_Urandom);
        fclose(f_Urandom);
    }

#elif defined(PEACH_PLATFORM_WASM)
#   include <unistd.h>

    // Emscripten provides getentropy which delegates to crypto.getRandomValues in the browser

    PEACH_FORCEINLINE void 
        SecureRandomNumber
        (
            uint8_t* fp_Buffer,
            const size_t fp_Size
        )
    {
        PEACH_PANIC_IF(not fp_Buffer, "Tried to pass nullptr ref to Math::SecureRandomNumber()");

        getentropy(fp_Buffer, fp_Size);
    }

#else
    #error "SecureRandomNumber: unsupported platform - add an RNG implementation"
#endif

}//namespace PeachCore::Math