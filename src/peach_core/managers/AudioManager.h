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

    struct GameManager; //fwd decl for friend owo, they friends >///< just friends ^W^ >w<

    //////////////////////////////////////////////
    // AudioManager UwU!
    //////////////////////////////////////////////

    class AudioManager 
    {
    //////////////////////////////////////////////
    // Public Destructor
    //////////////////////////////////////////////
    public:
        ~AudioManager() = default;

    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        AudioManager() = default; //: pm_Device(nullptr), pm_Context(nullptr) {}

        // Prevent copy and move >//<
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;
        AudioManager(AudioManager&&) = delete;
        AudioManager& operator=(AudioManager&&) = delete;

        friend GameManager; // <3

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        std::thread pm_AudioThread;
        
        shared_ptr<AudioResourcePipe> pm_LoadedAudioResourceQueue = nullptr;
        AudioCommandPipe pm_AudioCommandQueue;

        atomic<bool> pm_IsRunning{ true }; //this doesn't need to be atomic but whatevs, or even needed tbh but probs helpful for the while loop maybes
        atomic<bool> pm_IsInitialized{ false };

        vector<AudioPCM> pm_Sounds;         // static SFX
        vector<AudioStreamed> pm_StreamedSounds; // music/ambient

        // WARNING: these are very big and need to be heap allocd possibly, we wanna take up < 10KB on the stack but 20KB is probably fine ngl idk for later ||| future ryan:
        // ma_engine pm_Engine;
        // ma_sound_group pm_MasterGroup;
        // ma_sound_group pm_MusicGroup;
        // ma_sound_group pm_SFXGroup;

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
        [[nodiscard]] bool
            InitializeAudioEngine
            (
                const float fp_InitialVolume,
                shared_ptr<AudioResourcePipe> fp_AudioResourcePipe,
                const string& fp_LogOutputDirectory
            );

        void
            RequestAudio();

        void
            ShutdownSubsystem(Logger*const logger);

        PEACH_FORCEINLINE void
            PushCommand(AudioCommand fp_AudioCommand)
        {
            pm_AudioCommandQueue.enqueue(fp_AudioCommand);
        }

        void 
            ShutdownAudioEngine();

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        void
            AudioLoop();

        //this should probably be in resource loading manager along w the plugin stuff lmfao
        //bool LoadWAVFile(const string& filename, ALuint buffer);

        void
            ProcessCommand(AudioCommand fp_AudioCommand);

        // void
        //     OnResourceTransfer(const ResourceTransfer& rt);
    };
}
