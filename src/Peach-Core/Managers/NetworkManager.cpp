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
#include "../../../include/Peach-Core/Managers/NetworkManager.h"

namespace PeachCore {

    bool
        NetworkManager::InitializeNetworking
        (
            const string& fp_LogOutputDirectory
        )
    {
        network_logger = make_unique<Logger>();
        network_logger->Initialize(ThreadName::NetworkThread, fp_LogOutputDirectory, "NetworkLogger", Logger::LogLevel::ALL_LOGS);
        network_logger->Debug("NetworkLogger successfully initialized", "NetworkManager");

        return true;
    }
}