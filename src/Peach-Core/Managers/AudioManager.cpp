#include "../../include/Peach-Core/Managers/AudioManager.h"

using namespace std;

namespace PeachCore {

    bool 
        AudioManager::Initialize()
    {
        m_Device = alcOpenDevice(nullptr); // Open default device
        
        if (!m_Device)
        {
            LogManager::AudioLogger().Error("Failed to open audio device", "AudioManager");
            return false;
        }
        
        m_Context = alcCreateContext(m_Device, nullptr);

        shared_ptr<LoadingQueue> pm_LoadedAudioResourceQueue = ResourceLoadingManager::ResourceLoader().GetAudioResourceLoadingQueue();
        
        if (!m_Context || !alcMakeContextCurrent(m_Context))
        {
            LogManager::AudioLogger().Error("Failed to create or set audio context", "AudioManager");

            if (m_Context) 
            {
                alcDestroyContext(m_Context);
            }

            alcCloseDevice(m_Device);

            return false;
        }
        return true;
    }

    void 
        AudioManager::Shutdown() 
    {
        alcMakeContextCurrent(nullptr);
        if (m_Context) {alcDestroyContext(m_Context);}
        if (m_Device) {alcCloseDevice(m_Device);}
    }

    void 
        AudioManager::PlaySoundOnce(const string& fp_SoundFile)
    {
        unique_lock<shared_mutex> lock(mutex_);
        ALuint f_Buffer, f_Source;
        alGenBuffers(1, &f_Buffer);
        alGenSources(1, &f_Source);

        // Load WAV file into buffer
        // Assuming LoadWAVFile is a function that loads a WAV file into an OpenAL buffer
        if (!LoadWAVFile(fp_SoundFile, f_Buffer)) {
            LogManager::AudioLogger().Error("Failed to load sound: " + fp_SoundFile, "AudioManager");
            return;
        }

        alSourcei(f_Source, AL_BUFFER, f_Buffer);
        alSourcePlay(f_Source);

        // Store source for cleanup
        m_Sources.push_back(f_Source);
    }

    string 
        AudioManager::GetCurrentTrack() const 
    {
        shared_lock<shared_mutex> lock(mutex_);
        return m_CurrentTrack;
    }

    void 
        AudioManager::SetCurrentTrack(const string& track)
 {
        unique_lock<shared_mutex> lock(mutex_);
        m_CurrentTrack = track;
    }

    bool 
        AudioManager::LoadWAVFile(const string& filename, ALuint buffer) 
    {
        ifstream file(filename, ios::binary);
        if (!file) 
        {
            cerr << "Failed to open WAV file: " << filename << endl;
            return false;
        }

        char f_ChunkID[4];
        file.read(f_ChunkID, 4);

        if (strncmp(f_ChunkID, "RIFF", 4) != 0)
        {
            cerr << "Invalid WAV file: " << filename << endl;
            return false;
        }

        file.seekg(4, ios::cur); // Skip Chunk Size

        char f_Type[4];
        file.read(f_Type, 4);

        if (strncmp(f_Type, "WAVE", 4) != 0) 
        {
            cerr << "Invalid WAV file format: " << filename << endl;
            return false;
        }

        char f_SubChunk1ID[4];
        file.read(f_SubChunk1ID, 4);

        if (strncmp(f_SubChunk1ID, "fmt ", 4) != 0)
        {
            cerr << "Invalid WAV file fmt subchunk: " << filename << endl;
            return false;
        }

        uint32_t f_SubChunk1Size;
        file.read(reinterpret_cast<char*>(&f_SubChunk1Size), sizeof(f_SubChunk1Size));

        uint16_t f_AudioFormat;
        file.read(reinterpret_cast<char*>(&f_AudioFormat), sizeof(f_AudioFormat));

        uint16_t f_NumChannels;
        file.read(reinterpret_cast<char*>(&f_NumChannels), sizeof(f_NumChannels));

        uint32_t f_SampleRate;
        file.read(reinterpret_cast<char*>(&f_SampleRate), sizeof(f_SampleRate));

        file.seekg(6, ios::cur); // Skip ByteRate and BlockAlign

        uint16_t f_BitsPerSample;
        file.read(reinterpret_cast<char*>(&f_BitsPerSample), sizeof(f_BitsPerSample));

        char subchunk2ID[4];
        file.read(subchunk2ID, 4);
        if (strncmp(subchunk2ID, "data", 4) != 0) {
            cerr << "Invalid WAV file data subchunk: " << filename << endl;
            return false;
        }

        uint32_t subchunk2Size;
        file.read(reinterpret_cast<char*>(&subchunk2Size), sizeof(subchunk2Size));

        vector<char> data(subchunk2Size);
        file.read(data.data(), subchunk2Size);

        ALenum format;
        if (f_NumChannels == 1) {
            format = (f_BitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
        }
        else {
            format = (f_BitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
        }

        alBufferData(buffer, format, data.data(), subchunk2Size, f_SampleRate);

        return true;
    }
    
    void 
        AudioManager::ProcessLoadedResourcePackages()
    {
        unique_ptr<LoadedResourcePackage> ResourcePackage;
        while (pm_LoadedAudioResourceQueue->PopLoadedResourceQueue(ResourcePackage)) {
            visit(overloaded
                {
                [&](AudioData& fp_RawByteData)
                {
                    // Handle creation logic here
                },
                [](auto&&)
                {
                    // Default handler for any unhandled types
                    LogManager::AudioLogger().Warn("Unhandled type in variant for ProcessLoadedResourcePackage", "AudioManager");
                }
                }, ResourcePackage.get()->ResourceData);
        }
    }
}