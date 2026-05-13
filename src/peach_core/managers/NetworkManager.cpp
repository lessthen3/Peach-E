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
#include "NetworkManager.h"
#include "LogManager.h"

namespace PeachCore {

    bool
        NetworkManager::InitializeNetworking
        (
            const string& fp_LogOutputDirectory
        )
    {
        network_logger = LogManager::get_single().CreateUniqueLogger("NetworkLogger", PEACH_LOGGER_DEFAULT_FLAGS, fp_LogOutputDirectory);

        if (not network_logger)
        {
            PEACH_PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: NetworkManager failed to initialize the network_thread logger >w<");
            return false;
        }

        pm_NetworkThread = thread
        (
            &NetworkManager::NetworkLoop,
            this
        );        

        network_logger->Info("NetworkLogger successfully initialized", "NetworkManager");

        pm_IsInitialized.store(true);
        
        return true;
    }

    void
         NetworkManager::NetworkLoop()
    {
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
        NetworkManager::ShutdownSubsystem(Logger*const logger)
    {
        if(not logger) [[unlikely]]
        {
            PEACH_PRINT_ERROR("Tried to pass nullptr reference to logger inside NetworkManager::ShutdownSubsystem()");
            return;
        }

        if(not pm_IsInitialized.load(std::memory_order_acquire))
        {
            logger->Error("Tried to call ShutdownSubsystem() on Network Manager when network thread was never started owo wtf mang ;w;", "NetworkManager");
            return;
        }

        pm_IsRunning.store(false, std::memory_order_release);
        pm_NetworkSemaphore.release(); // Wake it up to exit 
        
        if (pm_NetworkThread.joinable()) 
        { 
            pm_NetworkThread.join(); 
            logger->Info("Successfully joined network thread", "NetworkManager");
        }
    }
}