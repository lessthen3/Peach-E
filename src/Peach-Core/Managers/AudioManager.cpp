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

    void
        AudioManager::AudioLoop
        (
            const string& fp_LogOutputDirectory, 
            const float fp_InitialVolume,
            latch& fp_InitLatch
        )
    {
        if (not InitializeAudioEngine(fp_LogOutputDirectory))
        {

            return;
        }

        fp_InitLatch.count_down();

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            // Block until main thread wakes us
            pm_AudioSemaphore.acquire();

            if (not pm_IsRunning.load(std::memory_order_acquire))
            {
                break; // Double check after wake
            }

            //ProcessCommands();
        }

        ShutdownAudioEngine();
    }

    void
        AudioManager::RequestAudio()
    {
        pm_AudioSemaphore.release();        
    }

    void
        AudioManager::Stop()
    {
        pm_IsRunning.store(false, std::memory_order_release);
        pm_AudioSemaphore.release(); // Wake it up to exit        
    }

    bool 
        AudioManager::InitializeAudioEngine
        (
            const string& fp_LogOutputDirectory
        )
    {
        //////////////////// Initialize Logger ////////////////////

        audio_logger = make_unique<Logger>();
        audio_logger->Initialize(ThreadName::AudioThread, fp_LogOutputDirectory, "AudioManager", Logger::LogLevel::ALL_LOGS);
        audio_logger->Debug("AudioLogger successfully initialized", "AudioManager");

        //////////////////// Initialize Loading and Command Queues ////////////////////

        if (not InitializeLoadingQueue())
        {
            audio_logger->Fatal("Initialization failed: AudioManager was not able to obtain a valid LoadingQueue, exiting execution immediately", "AudioManager");
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
        AudioManager::InitializeLoadingQueue()
    {
        if (pm_LoadedAudioResourceQueue)
        {
            audio_logger->Warning("AudioManager already retrieved the loaded resource queue from ResourceManager >O<", "AudioManager");
            return false;
        }

        pm_LoadedAudioResourceQueue = ResourceManager::get_single().GetAudioResourceLoadingQueue(audio_logger.get());

        if (not pm_LoadedAudioResourceQueue)
        {
            audio_logger->Error("AudioManager failed to retrieve LoadingQueue from ResourceManager, nullptr ref was found >O<", "AudioManager");
            return false;
        }

        audio_logger->Info("AudioManager successfully retrieved loaded resource queue from ResourceManager", "AudioManager");

        return true;
    }

    bool
        AudioManager::InitializeAudioCommandQueue()
    {
        if (pm_AudioCommandQueue)
        {
            audio_logger->Warning("AudioManager already initialized the audio command queue >O<", "AudioManager");
            return false;
        }

        pm_AudioCommandQueue = make_shared<moodycamel::ReaderWriterQueue<AudioCommand, TESTING_CAMEL_QUEUE_SIZE>>();

        audio_logger->Info("AudioManager successfully initialized the audio command queue", "AudioManager");

        return true; //returns one and only one ptr to whoever initializes AudioManager, this is meant only for the main thread
    }

    [[nodiscard]] shared_ptr<moodycamel::ReaderWriterQueue<AudioCommand, TESTING_CAMEL_QUEUE_SIZE>>
        AudioManager::GetAudioCommandQueue(Logger* const logger)
    {
        if (not pm_IsInitialized)
        {
            logger->Error("Attempted to get a reference to AudioManager's AudioCommandQueue before AudioManager was initialized, please initialize AudioManager first UwU", "AudioManager");
            return nullptr;
        }
        else if (pm_AudioCommandQueue.use_count() >= 2)
        {
            logger->Warning("AudioManager has already issued a reference to the audio command queue, fuck off", "AudioManager");
            return nullptr;
        }

        return pm_AudioCommandQueue;
    }

    void 
        AudioManager::PlaySoundOnce(const string& fp_SoundFile)
    {
        //unique_lock<shared_mutex> lock(mutex_);
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
        //shared_lock<shared_mutex> lock(mutex_);
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