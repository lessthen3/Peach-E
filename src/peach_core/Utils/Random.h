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

#include <cstdint>
#include <cstddef>

namespace PeachCore::Random{
// Platform secure random — always delegates to OS entropy source.
// Never use userspace RNG for crypto material, key generation, or nonces.

#if defined(PEACH_PLATFORM_VITA)
    #include <psp2/kernel/rng.h>

    // sceKernelGetRandomNumber caps at 64 bytes per call so we chunk it
    inline void SecureRandomNumber(uint8_t* fp_Buffer, size_t fp_Size)
    {
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

#elif defined(PEACH_PLATFORM_IS_WINDOWS)
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")

    inline void SecureRandomNumber(uint8_t* fp_Buffer, size_t fp_Size)
    {
        BCryptGenRandom(
            nullptr,
            fp_Buffer,
            static_cast<ULONG>(fp_Size),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG
        );
    }

#elif defined(PEACH_PLATFORM_MACOS)  || \
        defined(PEACH_PLATFORM_IOS)    || \
        defined(PEACH_PLATFORM_TVOS)   || \
        defined(PEACH_PLATFORM_FREEBSD)

    // arc4random_buf is always available on Apple + BSD, never blocks,
    // no fd to manage, seeded by the kernel automatically
    #include <stdlib.h>

    inline void SecureRandomNumber(uint8_t* fp_Buffer, size_t fp_Size)
    {
        arc4random_buf(fp_Buffer, fp_Size);
    }

#elif defined(PEACH_PLATFORM_LINUX)  || \
        defined(PEACH_PLATFORM_ANDROID)

    #include <sys/random.h>

    inline void SecureRandomNumber(uint8_t* fp_Buffer, size_t fp_Size)
    {
        size_t f_Remaining = fp_Size;
        uint8_t* f_Cursor  = fp_Buffer;

        // getrandom is capped at 256 bytes per call when using /dev/urandom
        // (GRND_NONBLOCK), chunk it to be safe across all kernel versions
        while (f_Remaining > 0)
        {
            size_t f_ChunkSize = f_Remaining < 256 ? f_Remaining : 256;
            getrandom(f_Cursor, f_ChunkSize, 0);
            f_Cursor    += f_ChunkSize;
            f_Remaining -= f_ChunkSize;
        }
    }

#elif defined(PEACH_PLATFORM_HAIKU)

    // Haiku doesn't have getrandom, /dev/urandom is the correct path
    #include <cstdio>

    inline void SecureRandomNumber(uint8_t* fp_Buffer, size_t fp_Size)
    {
        FILE* f_Urandom = fopen("/dev/urandom", "rb");
        fread(fp_Buffer, 1, fp_Size, f_Urandom);
        fclose(f_Urandom);
    }

#elif defined(PEACH_PLATFORM_WASM)

    // Emscripten provides getentropy which delegates to
    // crypto.getRandomValues in the browser
    #include <unistd.h>

    inline void SecureRandomNumber(uint8_t* fp_Buffer, size_t fp_Size)
    {
        getentropy(fp_Buffer, fp_Size);
    }

#else
    #error "SecureRandomNumber: unsupported platform - add an RNG implementation"
#endif

}//namespace PeachCore::Random