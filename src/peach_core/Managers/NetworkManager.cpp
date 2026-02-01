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

        pm_NetworkCommandQueue = make_shared<NetworkCommandPipe>();

        pm_IsInitialized = true;

        return true;
    }

    void
         NetworkManager::NetworkLoop
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch
        )
    {
        if (not InitializeNetworking(fp_LogOutputDirectory))
        {
            PrintError("Failed to Initialize Network Thread!");
            return;
        }

        fp_InitLatch.count_down();

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            // Block until main thread wakes us
            pm_NetworkSemaphore.acquire();

            if (not pm_IsRunning.load(std::memory_order_acquire))
            {
                break; // Double check after wake
            }

            //ProcessCommands();
        }
    }

    void
        NetworkManager::RequestNetworkStats()
    {
        pm_NetworkSemaphore.release();
    }

    void
        NetworkManager::Stop()
    {
        pm_IsRunning.store(false, std::memory_order_release);
        pm_NetworkSemaphore.release(); // Wake it up to exit        
    }

    [[nodiscard]] shared_ptr<NetworkCommandPipe>
        NetworkManager::GetNetworkCommandQueue(Logger* const logger)
    {
        if (not logger)
        {
            PrintError("TRIED TO PASS NULL_PTR REF TO LOGGER INSIDE GetAudioCommandQueue()");
            return nullptr;
        }
        else if (not pm_IsInitialized)
        {
            logger->Error("Attempted to get a reference to AudioManager's AudioCommandQueue before AudioManager was initialized, please initialize AudioManager first UwU", "AudioManager");
            return nullptr;
        }
        else if (pm_NetworkCommandQueue.use_count() >= 2)
        {
            logger->Warning("AudioManager has already issued a reference to the audio command queue, fuck off", "AudioManager");
            return nullptr;
        }

        return pm_NetworkCommandQueue;
    }
}