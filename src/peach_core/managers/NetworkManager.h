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
#pragma once

///STL
#include <semaphore>

///PeachCore
#include "utils/Logger.h" 

///moodycamel
#include <moody_camel/readerwriterqueue.h>

namespace PeachCore {

    //////////////////////////////////////////////
    // ResourceManager word size
    //////////////////////////////////////////////

    struct NetworkCommand
    {
        uint32_t node_id;       // 4 bytes
        uint16_t opcode;        // 2 bytes
        uint16_t reserved;      // 2 bytes (alignment or flags)
        uint64_t operand;       // 8 bytes
    };

    using NetworkCommandPipe = moodycamel::ReaderWriterQueue<NetworkCommand, MOODY_CAMEL_QUEUE_SIZE>;

    struct GameManager;

    class NetworkManager
    {
    //////////////////////////////////////////////
    // Public Destructor
    //////////////////////////////////////////////
    public:
        ~NetworkManager() = default;

    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        explicit NetworkManager() = default; //explicitly nothing UwU >O<

        NetworkManager(const NetworkManager&) = delete;
        NetworkManager& operator=(const NetworkManager&) = delete;
        NetworkManager(NetworkManager&&) = delete;
        NetworkManager& operator=(NetworkManager&&) = delete;

        friend GameManager;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        unique_ptr<Logger> network_logger = nullptr;

        binary_semaphore pm_NetworkSemaphore{ 0 }; // starts locked (zero tickets)

        atomic<bool> pm_IsRunning{ true }; //this doesn't need to be atomic but whatevs, or even needed tbh but probs helpful for the while loop maybes
        atomic<bool> pm_IsInitialized{ false };

        std::thread pm_NetworkThread;

        NetworkCommandPipe pm_NetworkCommandQueue;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool
            InitializeNetworking
            (
                const string& fp_LogOutputDirectory
            );

        void
            NetworkLoop();

        void
            RequestNetworkStats();

        void
            ShutdownSubsystem(Logger*const logger);

        PEACH_FORCEINLINE void
            PushCommand(NetworkCommand fp_NetworkCommand)
        {
            pm_NetworkCommandQueue.enqueue(fp_NetworkCommand);
        }
    };
}