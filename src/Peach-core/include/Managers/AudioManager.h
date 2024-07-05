#pragma once

#include <iostream>
#include <thread>
#include <shared_mutex>
#include <chrono>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>

namespace PeachCore {

    class AudioManager {
    public:
        static AudioManager& Audio() {
            static AudioManager instance;
            return instance;
        }

        bool Initialize();
        void Shutdown();
        void PlaySound(const std::string& soundFile);
        std::string GetCurrentTrack() const;
        void SetCurrentTrack(const std::string& track);

    private:
        AudioManager() : device(nullptr), context(nullptr) {}
        ~AudioManager() {
            Shutdown();
        }

        // Prevent copying
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        bool LoadWAVFile(const std::string& filename, ALuint buffer);

        ALCdevice* device;
        ALCcontext* context;
        mutable std::shared_mutex mutex_;
        std::string currentTrack_;
        std::vector<ALuint> sources;
	};



}
