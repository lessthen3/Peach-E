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
#include "PeachAPI.h"
#include "../Managers/GameManager.h"

#define PEACH_ASSERT_NODE_EXISTS(x) PeachCore::GameManager::get_single().GetCurrentScene()->

/*
* This file is a bastard mix of C++ and C99 since all entry points need to use ABI compatible types because the header will be included in C code and anything else will be a superset of that like C++ or Rust
* 
* so all function body code is C++ since this .cpp file will only be compiled using a C++ compiler
*/

//////////////////////////////////////////////////////////////////////////////////////////// Logging Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE
    PEACH_LogInfo(const char* fp_LogMessage, const char* fp_Sender)
{
    PeachCore::GameManager::get_single().m_UserLogger->Info(fp_LogMessage, fp_Sender);

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////// Scene Tree Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE
    PEACH_ChangeScene(const char* fp_NewSceneName)
{
    return PeachCore::GameManager::get_single().ChangeScene(fp_NewSceneName);
}

PEACH_API PEACH_STATUS_CODE
    PEACH_QueueRemovalByName(const char* fp_PeachNodeName)
{
    return PeachCore::GameManager::get_single().GetCurrentScene()->QueueNodeForRemoval(fp_PeachNodeName);
}

PEACH_API PEACH_STATUS_CODE
    PEACH_QueueRemovalByID(const PEACH_NODE fp_PeachNodeID)
{
    return PeachCore::GameManager::get_single().GetCurrentScene()->QueueNodeForRemoval(fp_PeachNodeID);
}

PEACH_API PEACH_STATUS_CODE
    PEACH_DuplicatePeachNode(const PEACH_NODE fp_OriginalNode, PEACH_NODE*const  fp_DuplicatedNodeContainer)
{
    if (not fp_DuplicatedNodeContainer)
    {
        //PeachCore::Print("[ERROR]: Tried to pass a nullptr reference to Peach_DuplicatePeachNode(), for desired duplicated peach node container", PeachCore::Colours::Red);
        return PeachCore::PEACH_STATUS_CODE::PEACH_ERROR_NULLPTR_REF_PASSED;
    }

    *fp_DuplicatedNodeContainer = 69; //OwO

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

PEACH_API PEACH_STATUS_CODE 
    PEACH_ChangeNodeName(const PEACH_NODE fp_NodeID, const char* fp_NewName)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////// PeachNode2D Transformations ////////////////////////////////////////////////////////////////////////////////////////////


PEACH_API PEACH_STATUS_CODE 
    PEACH_MoveNode2D(const PEACH_NODE fp_NodeID, const float fp_Dx, const float fp_Dy)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

PEACH_API PEACH_STATUS_CODE 
    PEACH_RotateNode2D(const PEACH_NODE fp_NodeID, const float fp_Rotation)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

PEACH_API PEACH_STATUS_CODE 
    PEACH_ScaleNode2D(const PEACH_NODE fp_NodeID, const float fp_Scale)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

PEACH_API PEACH_STATUS_CODE 
    PEACH_SetNodeVisibility(const PEACH_NODE fp_NodeID, const bool fp_Visibility)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////// Audio Operations ////////////////////////////////////////////////////////////////////////////////////////////

PEACH_API PEACH_STATUS_CODE 
    PEACH_StreamSound(const PEACH_NODE fp_NodeID)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

PEACH_API PEACH_STATUS_CODE 
    PEACH_PlaySound(const PEACH_NODE fp_NodeID)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

PEACH_API PEACH_STATUS_CODE 
    PEACH_SelectAudioOutputDevice(const PEACH_NODE fp_NodeID)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}

PEACH_API PEACH_STATUS_CODE 
    PEACH_SelectAudioInputDevice(const PEACH_NODE fp_NodeID)
{

    return PeachCore::PEACH_STATUS_CODE::PEACH_OK;
}
