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
#pragma once

#include "../Utils/Logger.h"

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

    class NetworkManager
    {
    //////////////////////////////////////////////
    // Private Destructor
    //////////////////////////////////////////////
    private:
        ~NetworkManager() = default;

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static NetworkManager& get_single()
        {
            static NetworkManager network_manager;
            return network_manager;
        }
    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        explicit NetworkManager() = default; //explicitly nothing UwU >O<
        NetworkManager(const NetworkManager&) = delete;
        NetworkManager& operator=(const NetworkManager&) = delete;

        NetworkManager(NetworkManager&&) = delete;
        NetworkManager& operator=(NetworkManager&&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        unique_ptr<Logger> network_logger = nullptr;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool
            InitializeNetworking
            (
                const string& fp_LogOutputDirectory
            );
        //NOT SURE AT ALL HOW NETWORK MANAGER INTEGRATES INTO THE ENGINE SYSTEM UWU OWOWOWOWWO
        //bool
        //    InitializeNetworkCommandQueue();

        //[[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<RenderCommand, 10>>
        //    GetNetworkCommandQueue();

    };
}