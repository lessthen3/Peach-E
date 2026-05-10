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

///PeachCore
#include "ResourceManager.h"

///STL

namespace PeachCore {

    struct AudioPCM
    {
        AudioID ID;
        ma_sound Sound;
    };

    struct AudioStreamed
    {
        AudioID ID;
        ma_sound Stream;
    };

    //////////////////////////////////////////////
    // AudioManager word size
    //////////////////////////////////////////////

    struct AudioCommand
    {
        uint32_t node_id = 0;       // 4 bytes
        uint16_t opcode = 0;        // 2 bytes
        uint16_t reserved = 0;      // 2 bytes (alignment or flags)
        uint64_t operand = 0;       // 8 bytes
    };

    //////////////////////////////////////////////
    // Audio Pipe Mask
    //////////////////////////////////////////////

    using AudioCommandPipe = moodycamel::ReaderWriterQueue<AudioCommand, MOODY_CAMEL_QUEUE_SIZE>;

    //////////////////////////////////////////////
    // AudioManager UwU!
    //////////////////////////////////////////////

    class AudioManager 
    {
    //////////////////////////////////////////////
    // Private Destructor
    //////////////////////////////////////////////
    private:
        ~AudioManager() = default;

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static AudioManager& get_single()
        {
            static AudioManager audio_manager;
            return audio_manager;
        }

    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        AudioManager() = default; //: pm_Device(nullptr), pm_Context(nullptr) {}

        // Prevent copying
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        AudioManager(AudioManager&&) = delete;
        AudioManager& operator=(AudioManager&&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        shared_ptr<ResourcePipe> pm_LoadedAudioResourceQueue = nullptr;
        shared_ptr<AudioCommandPipe> pm_AudioCommandQueue = nullptr;

        atomic<bool> pm_IsRunning{ true }; //this doesn't need to be atomic but whatevs, or even needed tbh but probs helpful for the while loop maybes
        atomic<bool> pm_IsInitialized{ false };

        vector<AudioPCM> pm_Sounds;         // static SFX
        vector<AudioStreamed> pm_StreamedSounds; // music/ambient
        ma_engine pm_Engine;

        ma_sound_group pm_MasterGroup;
        ma_sound_group pm_MusicGroup;
        ma_sound_group pm_SFXGroup;

        unique_ptr<Logger> audio_logger = nullptr;

        MaxCountingSemaphore pm_AudioSemaphore{ 0 }; // starts locked (zero tickets)

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:

        void
            AudioLoop
            (
                const string& fp_LogOutputDirectory, 
                const float fp_InitialVolume,
                latch& fp_InitLatch
            );

        void
            RequestAudio();

        void
            Stop();

        bool
            InitializeLoadingQueue();

        bool
            InitializeAudioCommandQueue();

        [[nodiscard]] shared_ptr<AudioCommandPipe>
            GetAudioCommandQueue(Logger*const logger);

        void 
            ShutdownAudioEngine();

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        bool
            InitializeAudioEngine
            (
                const string& fp_LogOutputDirectory
            );

        //this should probably be in resource loading manager along w the plugin stuff lmfao
        //bool LoadWAVFile(const string& filename, ALuint buffer);

        void
            ProcessCommand(const AudioCommand& fp_AudioCommand);

        void
            OnResourceTransfer(const ResourceTransfer& rt);

    };
}
