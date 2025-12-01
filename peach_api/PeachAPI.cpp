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
#include "Managers/GameManager.h"





PEACH_API PEACH_ERROR_CODE
    Peach_Log(const char* fp_LogMessage)
{
    //GameManager::get_single().m_UserLogger->Info(msg, "PeachAPI");

    return PEACH_OK;
}

PEACH_API PEACH_ERROR_CODE
    Peach_ChangeScene(const char* fp_NewSceneName)
{

    return PEACH_OK;
}

PEACH_API PEACH_ERROR_CODE
    Peach_QueueRemoval(size_t fp_PeachNodeID)
{
        
    return PEACH_OK;
}

PEACH_API PEACH_ERROR_CODE
    Peach_DuplicatePeachNode(PeachNode fp_OriginalNode, PeachNode*  fp_DuplicatedNodeContainer)
{
    if (not fp_DuplicatedNodeContainer)
    {
        PeachCore::Print("[ERROR]: Tried to pass a nullptr reference to Peach_DuplicatePeachNode(), for desired duplicated peach node container", PeachCore::Colours::Red);
        return PEACH_ERROR_NULLPTR_REF_PASSED;
    }

    return PEACH_OK;
}

