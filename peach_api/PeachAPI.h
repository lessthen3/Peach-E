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
#ifndef PEACH_ENGINE_API_C_H_
#define PEACH_ENGINE_API_C_H_

#include <stdint.h>

#ifdef __cplusplus

    //XXX: need to have C calling convention since this'll be called by external runtimes which probably just use a C calling convention esp C#
#if (defined(_WIN32) || defined(_WIN64)) && defined(PEACH_API_BUILD_DYNAMIC)
        #define PEACH_API extern "C" __declspec(dllexport)
    #else
        #define PEACH_API extern "C"
    #endif

#else //being used from C

    #define PEACH_API //leave empty when included in C file

#endif //C++ detection


PEACH_API void 
    Peach_Log(const char* fp_Message);

PEACH_API int64_t
    Peach_ChangeScene(const char* fp_NewSceneName);

PEACH_API void 
    Peach_QueueRemoval(size_t fp_PeachNodeID); //queues for removal from scene tree at end of frame or whenever is convenient idk


#endif //header guard