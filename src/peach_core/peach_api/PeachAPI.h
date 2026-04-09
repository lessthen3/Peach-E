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
#ifndef PEACH_ENGINE_API_C_H_
#define PEACH_ENGINE_API_C_H_

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdint.h> //idrc about supporting C standards from before i was born lmfao, C99+ only fuck you if you use C89 still ISO standard is C11 idgaf ab ur old ahh ABI

#include "StatusCodes.h"
#include "LoggerFlags.h"
#include "NodeDef.h"

//////////////////////////////////////////////////////////// C++ --> C compatibility preprocessor defs ////////////////////////////////////////////////////////////

#ifdef __cplusplus 

    //XXX: need to have C calling convention since this'll be called by external runtimes which probably just use a C calling convention esp C#
    #if defined(PEACH_PLATFORM_WINDOWS) && defined(PEACH_BUILD_DYNAMIC)
            #define PEACH_API extern "C" __declspec(dllexport)
    #elif defined(PEACH_PLATFORM_WINDOWS) && !defined(PEACH_API_STATIC)
            #define PEACH_API extern "C" __declspec(dllimport)
    #else
            #define PEACH_API extern "C"
    #endif

#else //being used from C

    #include <stdbool.h> 
    #define PEACH_API //leave empty when included in C file

#endif //C++ detection

//////////////////////////////////////////////////////////// Starting Engine ////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE
    PEACH_InitializePeachEngineCustom();

//////////////////////////////////////////////////////////// Core API Functions ////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE
    PEACH_StatusCodeToString(const PEACH_STATUS_CODE fp_StatusCodesize_t, size_t fp_BufferSize, char* fp_CharBuffer);

PEACH_API PEACH_STATUS_CODE
    PEACH_GetLastErrorAsString(size_t fp_BufferSize, char* fp_CharBuffer);

//////////////////////////////////////////////////////////////////////////////////////////// Logging Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE
    PEACH_LogInfo(const char* fp_LogMessage, const char* fp_Sender);

//////////////////////////////////////////////////////////////////////////////////////////// Scene Tree Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE
    PEACH_ChangeScene(const char* fp_NewSceneName);

PEACH_API PEACH_STATUS_CODE
    PEACH_QueueRemovalByID(const PEACH_NodeID fp_PeachNodeID); //queues for removal from scene tree at end of frame or whenever is convenient idk

PEACH_API PEACH_STATUS_CODE
    PEACH_QueueRemovalByName(const char* fp_PeachNodeName);

PEACH_API PEACH_STATUS_CODE
    PEACH_DuplicatePeachNode(const PEACH_NodeID fp_OriginalNode, PEACH_NodeID*const fp_DuplicatedNodeContainer);

PEACH_API PEACH_STATUS_CODE 
    PEACH_SetNodeVisibility(const PEACH_NodeID fp_NodeID, const bool fp_Visibility); //bool is fine since im only supporting C99+ and that has stdbool.h rawr >O<

PEACH_API PEACH_STATUS_CODE 
    PEACH_ChangeNodeName(const PEACH_NodeID fp_NodeID, const char* fp_NewName); 

//////////////////////////////////////////////////////////////////////////////////////////// PeachNode2D Transformations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE 
    PEACH_MoveNode2D(const PEACH_NodeID fp_NodeID, const float fp_Dx, const float fp_Dy);

PEACH_API PEACH_STATUS_CODE 
    PEACH_RotateNode2D(const PEACH_NodeID fp_NodeID, const float fp_Rotation);

PEACH_API PEACH_STATUS_CODE 
    PEACH_ScaleNode2D(const PEACH_NodeID fp_NodeID, const float fp_Scale);

//////////////////////////////////////////////////////////////////////////////////////////// Audio Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE 
    PEACH_StreamSound(const PEACH_NodeID fp_NodeID); 

PEACH_API PEACH_STATUS_CODE 
    PEACH_PlaySound(const PEACH_NodeID fp_NodeID); 

PEACH_API PEACH_STATUS_CODE 
    PEACH_SelectAudioOutputDevice(const PEACH_NodeID fp_NodeID); 

PEACH_API PEACH_STATUS_CODE 
    PEACH_SelectAudioInputDevice(const PEACH_NodeID fp_NodeID); 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* PEACH_ENGINE_API_C_H_ */