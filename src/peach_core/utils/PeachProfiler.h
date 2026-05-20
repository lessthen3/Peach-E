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

namespace PeachCore {

    // #ifdef PEACH_PROFILER_ENABLED

    // #define PEACH_PROFILE_ZONE(name)        ProfilerZone fv_Zone(__FILE__, __LINE__, name)
    // #define PEACH_PROFILE_FRAME(name)       ProfilerFrame fv_Frame(name)
    // #define PEACH_PROFILE_GPU_ZONE(name)    GpuProfilerZone fv_GpuZone(name)
    // #define PEACH_PROFILE_MUTEX(mutex)      ProfilerMutex fv_MutexProf(mutex, __func__)
    // #define PEACH_PROFILE_ALLOC(ptr, size)  ProfilerTrackAlloc(ptr, size, __FILE__, __LINE__)
    // #define PEACH_PROFILE_FREE(ptr)         ProfilerTrackFree(ptr)
    // #define PEACH_PROFILE_VALUE(name, val)  ProfilerPlot(name, val)

    // #else

    //     #define PEACH_PROFILE_ZONE(name)        ((void)0)
    //     #define PEACH_PROFILE_FRAME(name)       ((void)0)
    //     // etc
    // #endif

    struct PeachFrameStats
    {
        float    FrameTimeMs;
        float    RenderTimeMs;
        float    PhysicsTimeMs;
        float    AudioTimeMs;
        float    GameLogicTimeMs;
        
        uint32_t DrawCalls;
        uint32_t TriangleCount;
        uint32_t ActivePhysicsBodies;
        
        uint64_t VramUsedBytes;
        uint64_t RamUsedBytes;       // /proc/self/status on Linux, QueryProcessMemory on Windows
        
        uint32_t LiveNodeCount;
        uint32_t ActiveAudioSources;
    };
}