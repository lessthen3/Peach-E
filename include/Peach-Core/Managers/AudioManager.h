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

///PeachCore
#include "ResourceManager.h"
#include <limits>

///STL

namespace PeachCore {

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

    class AudioManager 
    {
    //////////////////////////////////////////////
    // Private Destructor
    //////////////////////////////////////////////
    private:
        ~AudioManager() {}

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
        shared_ptr<moodycamel::ReaderWriterQueue<ResourceTransfer, MOODY_CAMEL_QUEUE_SIZE>> pm_LoadedAudioResourceQueue = nullptr;
        shared_ptr<moodycamel::ReaderWriterQueue<AudioCommand, MOODY_CAMEL_QUEUE_SIZE>> pm_AudioCommandQueue = nullptr;

        atomic<bool> pm_IsRunning = true; //this doesn't need to be atomic but whatevs, or even needed tbh but probs helpful for the while loop maybes
        atomic<bool> pm_IsInitialized = false;

        //mutable shared_mutex mutex_;
        string pm_CurrentTrack;
        //vector<ALuint> pm_Sources;

        unique_ptr<Logger> audio_logger = nullptr;

        counting_semaphore<PEACH_MAX_PTR_DIFF> pm_AudioSemaphore{ 0 }; // starts locked (zero tickets)

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

        [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<AudioCommand, MOODY_CAMEL_QUEUE_SIZE>>
            GetAudioCommandQueue(Logger*const logger);

        void PlaySoundOnce(const string& soundFile); //SUSUSUSUSUSUSUSUSSSYYYY FUNCTION (is PlaySound a predefined funciton in openal?)
        string GetCurrentTrack() const;
        void SetCurrentTrack(const string& track);

        //idk get bpm of current track, probably uneccesary but could be useful for rhythm games where users can import custom audio files, and then can generate a bpm map for it
        float 
            GetBPM(); 

        //used to sync a particular audio event to a user defined function handle to allow for easier integration of dynamic sound environments
        void 
            SyncToEventCallback(); 
 

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

    };
}
