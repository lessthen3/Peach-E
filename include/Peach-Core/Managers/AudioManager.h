/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#pragma once

///PeachCore
#include "LogManager.h"
#include "ResourceManager.h"

///STL
#include <thread>
#include <shared_mutex>

///External
#include <miniaudio/miniaudio.h>

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
        static AudioManager& AudioPlayer()
        {
            static AudioManager audioplayer;
            return audioplayer;
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
        shared_ptr<LoadingQueue> pm_LoadedAudioResourceQueue;

        //ALCdevice* pm_Device;
        //ALCcontext* pm_Context;
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

        void PlaySoundOnce(const string& soundFile); //SUSUSUSUSUSUSUSUSSSYYYY FUNCTION (is PlaySound a predefined funciton in openal?)
        string GetCurrentTrack() const;
        void SetCurrentTrack(const string& track);

        float GetBPM(); //idk get bpm of current track, probably uneccesary but could be useful for rhythm games where users can import custom audio files, and then can generate a bpm map for it

        void SyncToEventCallback(); //used to sync a particular audio event to a user defined function handle to allow for easier integration of dynamic sound environments
 

        void Shutdown();

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        //this should probably be in resource loading manager along w the plugin stuff lmfao
        //bool LoadWAVFile(const string& filename, ALuint buffer);

        void ProcessLoadedResourcePackages();

	};
}
