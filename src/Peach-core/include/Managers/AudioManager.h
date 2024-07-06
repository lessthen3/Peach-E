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
        AudioManager() : m_Device(nullptr), m_Context(nullptr) {}
        ~AudioManager() {
            Shutdown();
        }

        // Prevent copying
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        bool LoadWAVFile(const std::string& filename, ALuint buffer);

        ALCdevice* m_Device;
        ALCcontext* m_Context;
        mutable std::shared_mutex mutex_;
        std::string m_CurrentTrack;
        std::vector<ALuint> m_Sources;
	};



}
