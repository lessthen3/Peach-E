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

///STL
#include <thread>
#include <shared_mutex>

namespace PeachCore {

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
            static AudioManager audio_player;
            return audio_player;
        }

    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        AudioManager() = default; //: pm_Device(nullptr), pm_Context(nullptr) {}

        // Prevent copying
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        shared_ptr<LoadingQueue> pm_LoadedAudioResourceQueue = nullptr;
        shared_ptr<CommandQueue> pm_AudioCommandQueue = nullptr;

        bool pm_IsInitialized = false;

        mutable shared_mutex mutex_;
        string pm_CurrentTrack;
        //vector<ALuint> pm_Sources;

        unique_ptr<LogManager> audio_logger = nullptr;

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool 
            Initialize
        (
            const string& fp_LogOutputDirectory,
            shared_ptr<Console> fp_Console
        );

        bool
            InitializeAudioEngine();

        bool
            InitializeLoadingQueue();

        bool
            InitializeAudioCommandQueue();

        [[nodiscard]] shared_ptr<CommandQueue>
            GetAudioCommandQueue();

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
        //this should probably be in resource loading manager along w the plugin stuff lmfao
        //bool LoadWAVFile(const string& filename, ALuint buffer);

        void ProcessLoadedResourcePackages();

    };
}
