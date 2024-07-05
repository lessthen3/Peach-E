#include "../../include/Managers/AudioManager.h"


namespace PeachCore {

    bool AudioManager::Initialize() {
        device = alcOpenDevice(nullptr); // Open default device
        if (!device) {
            std::cerr << "Failed to open audio device" << std::endl;
            return false;
        }
        context = alcCreateContext(device, nullptr);
        if (!context || !alcMakeContextCurrent(context)) {
            std::cerr << "Failed to create or set audio context" << std::endl;
            if (context) alcDestroyContext(context);
            alcCloseDevice(device);
            return false;
        }
        return true;
    }

    void AudioManager::Shutdown() {
        alcMakeContextCurrent(nullptr);
        if (context) alcDestroyContext(context);
        if (device) alcCloseDevice(device);
    }

    void AudioManager::PlaySound(const std::string& soundFile) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        ALuint buffer, source;
        alGenBuffers(1, &buffer);
        alGenSources(1, &source);

        // Load WAV file into buffer
        // Assuming LoadWAVFile is a function that loads a WAV file into an OpenAL buffer
        if (!LoadWAVFile(soundFile, buffer)) {
            std::cerr << "Failed to load sound: " << soundFile << std::endl;
            return;
        }

        alSourcei(source, AL_BUFFER, buffer);
        alSourcePlay(source);

        // Store source for cleanup
        sources.push_back(source);
    }

    std::string AudioManager::GetCurrentTrack() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return currentTrack_;
    }

    void AudioManager::SetCurrentTrack(const std::string& track) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        currentTrack_ = track;
    }

    bool AudioManager::LoadWAVFile(const std::string& filename, ALuint buffer) {
        // Placeholder for loading WAV files
        // You should implement actual WAV file loading here
        // This is a simplified version for demonstration purposes
        return true;
    }
    
}