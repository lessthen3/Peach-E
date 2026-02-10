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

#include <stdint.h> //idrc about supporting C standards from before i was born lmfao, C99+ only fuck you if you use C89 still ISO standard is C11 idgaf ab ur old ahh ABI

#include "StatusCodes.h"
#include "LoggerFlags.h"

//////////////////////////////////////////////////////////// C++ --> C compatibility preprocessor defs ////////////////////////////////////////////////////////////

#ifdef __cplusplus 

    //XXX: need to have C calling convention since this'll be called by external runtimes which probably just use a C calling convention esp C#
    #if (defined(_WIN32) || defined(_WIN64)) && defined(PEACH_BUILD_DYNAMIC)
            #define PEACH_API extern "C" __declspec(dllexport)
    #elif (defined(_WIN32) || defined(_WIN64)) && !defined(PEACH_API_STATIC)
            #define PEACH_API extern "C" __declspec(dllimport)
    #else
            #define PEACH_API extern "C"
    #endif

#else //being used from C

    #include <stdbool.h> 
    #define PEACH_API //leave empty when included in C file

#endif //C++ detection

//////////////////////////////////////////////////////////// Typedefs ////////////////////////////////////////////////////////////

typedef uint64_t PEACH_NODE; //all nodes are only passed by ID

//typedef int64_t PEACH_STATUS_CODE;

//////////////////////////////////////////////////////////// Core API Functions ////////////////////////////////////////////////////////////

PEACH_API const char*
    PEACH_StatusCodeToString(const PEACH_STATUS_CODE fp_StatusCode);

//////////////////////////////////////////////////////////////////////////////////////////// Logging Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE
    PEACH_LogInfo(const char* fp_LogMessage, const char* fp_Sender);

//////////////////////////////////////////////////////////////////////////////////////////// Scene Tree Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE
    PEACH_ChangeScene(const char* fp_NewSceneName);

PEACH_API PEACH_STATUS_CODE
    PEACH_QueueRemovalByID(const PEACH_NODE fp_PeachNodeID); //queues for removal from scene tree at end of frame or whenever is convenient idk

PEACH_API PEACH_STATUS_CODE
    PEACH_QueueRemovalByName(const char* fp_PeachNodeName);

PEACH_API PEACH_STATUS_CODE
    PEACH_DuplicatePeachNode(const PEACH_NODE fp_OriginalNode, PEACH_NODE*const fp_DuplicatedNodeContainer);

PEACH_API PEACH_STATUS_CODE 
    PEACH_SetNodeVisibility(const PEACH_NODE fp_NodeID, const bool fp_Visibility); //bool is fine since im only supporting C99+ and that has stdbool.h rawr >O<

PEACH_API PEACH_STATUS_CODE 
    PEACH_ChangeNodeName(const PEACH_NODE fp_NodeID, const char* fp_NewName); 

//////////////////////////////////////////////////////////////////////////////////////////// PeachNode2D Transformations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE 
    PEACH_MoveNode2D(const PEACH_NODE fp_NodeID, const float fp_Dx, const float fp_Dy);

PEACH_API PEACH_STATUS_CODE 
    PEACH_RotateNode2D(const PEACH_NODE fp_NodeID, const float fp_Rotation);

PEACH_API PEACH_STATUS_CODE 
    PEACH_ScaleNode2D(const PEACH_NODE fp_NodeID, const float fp_Scale);

//////////////////////////////////////////////////////////////////////////////////////////// Audio Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE 
    PEACH_StreamSound(const PEACH_NODE fp_NodeID); 

PEACH_API PEACH_STATUS_CODE 
    PEACH_PlaySound(const PEACH_NODE fp_NodeID); 

PEACH_API PEACH_STATUS_CODE 
    PEACH_SelectAudioOutputDevice(const PEACH_NODE fp_NodeID); 

PEACH_API PEACH_STATUS_CODE 
    PEACH_SelectAudioInputDevice(const PEACH_NODE fp_NodeID); 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //header guard