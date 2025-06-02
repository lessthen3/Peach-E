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

namespace PeachCore{

    PEACH_API void
        Peach_Log(const char* msg)
    {
        GameManager::get_single().m_UserLogger->PEACH_LOG(msg, "PeachAPI", LogManager::LogLevel::Info);
    }

    PEACH_API void 
        Peach_ChangeScene(const char* fp_NewSceneName)
    {

    }

    PEACH_API void
        Peach_QueueRemoval(size_t fp_PeachNodeID)
    {
        
    }

}