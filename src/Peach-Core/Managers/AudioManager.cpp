/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#include "../../include/Peach-Core/Managers/AudioManager.h"

namespace PeachCore {

    bool 
        AudioManager::Initialize
        (
            const string& fp_LogOutputDirectory,
            shared_ptr<Console> fp_Console
        )
    {
        //////////////////// Nullptr check for PeachConsole ref ////////////////////

        if (not fp_Console)
        {
            PrintError("Tried to initialize AudioManager with a nullptr reference to the Console");
            return false;
        }

        //////////////////// Initialize Logger ////////////////////

        audio_logger = make_unique<LogManager>();
        audio_logger->Initialize(ThreadName::AudioThread, fp_LogOutputDirectory, "AudioManager", fp_Console, LogManager::LogLevel::All);
        audio_logger->LogAndPrint("AudioLogger successfully initialized", "AudioManager", PeachCore::LogManager::LogLevel::Debug);

        //////////////////// Initialize Loading and Command Queues ////////////////////

        if (not InitializeLoadingQueue())
        {
            audio_logger->LogAndPrint("Initialization failed: AudioManager was not able to obtain a valid LoadingQueue, exiting execution immediately", "AudioManager", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }

        InitializeAudioCommandQueue();

        //////////////////// Initialized Successfully! ////////////////////

        pm_IsInitialized = true;

        return true;
    }

    void 
        AudioManager::ShutdownAudioEngine()
    {
        //alcMakeContextCurrent(nullptr);
        //if (pm_Context) {alcDestroyContext(pm_Context);}
        //if (pm_Device) {alcCloseDevice(pm_Device);}
    }


    bool
        AudioManager::InitializeAudioEngine()
    {

        return true;
    }

    bool
        AudioManager::InitializeLoadingQueue()
    {
        if (pm_LoadedAudioResourceQueue)
        {
            audio_logger->LogAndPrint("AudioManager already retrieved the loaded resource queue from ResourceManager >O<", "AudioManager", LogManager::LogLevel::Warning);
            return false;
        }

        pm_LoadedAudioResourceQueue = ResourceManager::get_single().GetAudioResourceLoadingQueue();

        if (not pm_LoadedAudioResourceQueue)
        {
            audio_logger->LogAndPrint("AudioManager failed to retrieve LoadingQueue from ResourceManager, nullptr ref was found >O<", "AudioManager", LogManager::LogLevel::Error);
            return false;
        }

        audio_logger->LogAndPrint("AudioManager successfully retrieved loaded resource queue from ResourceManager", "AudioManager", LogManager::LogLevel::Info);

        return true;
    }

    bool
        AudioManager::InitializeAudioCommandQueue()
    {
        if (pm_AudioCommandQueue)
        {
            audio_logger->LogAndPrint("AudioManager already initialized the audio command queue >O<", "AudioManager", LogManager::LogLevel::Warning);
            return false;
        }

        pm_AudioCommandQueue = make_shared<CommandQueue>();

        audio_logger->LogAndPrint("AudioManager successfully initialized the audio command queue", "AudioManager", LogManager::LogLevel::Info);

        return true; //returns one and only one ptr to whoever initializes AudioManager, this is meant only for the main thread
    }

    [[nodiscard]] shared_ptr<CommandQueue>
        AudioManager::GetAudioCommandQueue()
    {
        if (not pm_IsInitialized)
        {
            audio_logger->LogAndPrint("Attempted to get a reference to AudioManager's AudioCommandQueue before AudioManager was initialized, please initialize AudioManager first UwU", "AudioManager", LogManager::LogLevel::Error);
            return nullptr;
        }
        else if (pm_AudioCommandQueue.use_count() >= 2)
        {
            audio_logger->LogAndPrint("AudioManager has already issued a reference to the audio command queue, fuck off", "AudioManager", LogManager::LogLevel::Warning);
            return nullptr;
        }

        return pm_AudioCommandQueue;
    }

    void 
        AudioManager::PlaySoundOnce(const string& fp_SoundFile)
    {
        unique_lock<shared_mutex> lock(mutex_);
        //ALuint f_Buffer, f_Source;
        //alGenBuffers(1, &f_Buffer);
        //alGenSources(1, &f_Source);

        //// Load WAV file into buffer
        //// Assuming LoadWAVFile is a function that loads a WAV file into an OpenAL buffer
        //if (!LoadWAVFile(fp_SoundFile, f_Buffer)) {
        //    audio_logger->LogAndPrint("Failed to load sound: " + fp_SoundFile, "AudioManager", PeachCore::LogManager::LogLevel::Error);
        //    return;
        //}

        //alSourcei(f_Source, AL_BUFFER, f_Buffer);
        //alSourcePlay(f_Source);

        //// Store source for cleanup
        //pm_Sources.push_back(f_Source);
    }

    string 
        AudioManager::GetCurrentTrack() 
        const 
    {
        shared_lock<shared_mutex> lock(mutex_);
        return pm_CurrentTrack;
    }

    void 
        AudioManager::SetCurrentTrack(const string& track) //this doesnt need a lock since the command queue tells audiomanager to do this
 {
        pm_CurrentTrack = track;
    }
    
    void 
        AudioManager::ProcessLoadedResourcePackages()
    {
        //unique_ptr<LoadedResourcePackage> ResourcePackage;
        //while (pm_LoadedAudioResourceQueue->PopLoadedResourceQueue(ResourcePackage)) {
        //    visit(overloaded
        //        {
        //        [&](AudioData& fp_RawByteData)
        //        {
        //            // Handle creation logic here
        //        },
        //        [](auto&&)
        //        {
        //            // Default handler for any unhandled types
        //            //audio_logger->LogAndPrint("Unhandled type in variant for ProcessLoadedResourcePackage", "AudioManager", "warn");
        //        }
        //        }, ResourcePackage.get()->ResourceData);
        //}
    }
}