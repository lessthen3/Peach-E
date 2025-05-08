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
            const string& fp_LogOutputDirectory,
            shared_ptr<Console> fp_Console
        )
    {
        if (not fp_Console)
        {
            PrintError("Tried to initialize NetworkManager with a nullptr reference to the Console");
            return false;
        }

        network_logger = make_unique<LogManager>();
        network_logger->Initialize(ThreadName::NetworkThread, fp_LogOutputDirectory, "NetworkLogger", fp_Console, LogManager::LogLevel::All);
        network_logger->LogAndPrint("NetworkLogger successfully initialized", "NetworkManager", LogManager::LogLevel::Debug);

        return true;
    }
}