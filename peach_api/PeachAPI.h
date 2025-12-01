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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

//////////////////////////////////////////////////////////// C++ --> C compatibility preprocessor defs ////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////// Error Codes ////////////////////////////////////////////////////////////

#define PEACH_OK 1

#define PEACH_ERROR_NULLPTR_REF_PASSED -1001

//////////////////////////////////////////////////////////// Typedefs ////////////////////////////////////////////////////////////

typedef struct
{
    uint64_t NodeID;
    uint8_t NodeType;
    void* Handle;
} PeachNode;

typedef int64_t PEACH_ERROR_CODE;


//////////////////////////////////////////////////////////// Core API Functions ////////////////////////////////////////////////////////////

PEACH_API PEACH_ERROR_CODE
    Peach_Log(const char* fp_LogMessage);

PEACH_API PEACH_ERROR_CODE
    Peach_ChangeScene(const char* fp_NewSceneName);

PEACH_API PEACH_ERROR_CODE
    Peach_QueueRemoval(size_t fp_PeachNodeID); //queues for removal from scene tree at end of frame or whenever is convenient idk

PEACH_API PEACH_ERROR_CODE
    Peach_DuplicatePeachNode(PeachNode fp_OriginalNode, PeachNode* fp_DuplicatedNodeContainer);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //header guard