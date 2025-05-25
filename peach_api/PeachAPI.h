/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include "Managers/GameManager.h"

//XXX: need to have C calling convention since this'll be called by external runtimes which probably just use a C calling convention esp C#
#if defined(_WIN32) || defined(_WIN64)
    #define PEACH_API extern "C" __declspec(dllexport)
#else
    #define PEACH_API extern "C"
#endif

namespace PeachCore{

    PEACH_API void Peach_Log(const char* msg);

    PEACH_API void Peach_ChangeScene(const char* fp_NewSceneName);

}