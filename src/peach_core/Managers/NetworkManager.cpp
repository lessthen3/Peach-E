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
#include "NetworkManager.h"

namespace PeachCore {

    bool
        NetworkManager::InitializeNetworking
        (
            const string& fp_LogOutputDirectory
        )
    {
        network_logger = Logger::CreateUnique("NetworkLogger", Logger::Flags::ALL_LOGS | Logger::Flags::FLUSH_ERROR | Logger::Flags::FLUSH_FATAL, fp_LogOutputDirectory);

        if (not network_logger)
        {
            PrintError("[CRITICAL_LOGGING_ERROR]: NetworkManager failed to initialize the network_thread logger >w<");
            return false;
        }

        network_logger->Debug("NetworkLogger successfully initialized", "NetworkManager");

        return true;
    }
}