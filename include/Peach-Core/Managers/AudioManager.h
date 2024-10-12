/*******************************************************************
 *                                             Peach-E v0.1
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#pragma once

#include <fstream>
#include <iostream>
#include <thread>
#include <shared_mutex>
#include <chrono>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>

#include "LogManager.h"
#include "ResourceLoadingManager.h"

using namespace std;

namespace PeachCore {

    class AudioManager {
    public:
        static AudioManager& AudioPlayer() {
            static AudioManager audioplayer;
            return audioplayer;
        }

        bool Initialize();

        void Shutdown();
        void PlaySoundOnce(const string& soundFile); //SUSUSUSUSUSUSUSUSSSYYYY FUNCTION (is PlaySound a predefined funciton in openal?)
        string GetCurrentTrack() const;
        void SetCurrentTrack(const string& track);

    private:
        AudioManager() : m_Device(nullptr), m_Context(nullptr) {}
        ~AudioManager() {
            Shutdown();
        }

        // Prevent copying
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        bool LoadWAVFile(const string& filename, ALuint buffer);

        void ProcessLoadedResourcePackages();

        float GetBPM() //idk get bpm of current track, probably uneccesary but could be useful for rhythm games where users can import custom audio files, and then can generate a bpm map for it
        {

        }

        void SyncToEventCallback() //used to sync a particular audio event to a user defined function handle to allow for easier integration of dynamic sound environments
        {

        }


    private:
        shared_ptr<LoadingQueue> pm_LoadedAudioResourceQueue;

        ALCdevice* m_Device;
        ALCcontext* m_Context;
        mutable shared_mutex mutex_;
        string m_CurrentTrack;
        vector<ALuint> m_Sources;
	};
}
