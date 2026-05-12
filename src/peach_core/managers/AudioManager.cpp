/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#include "AudioManager.h"

namespace PeachCore {

    void
        AudioManager::AudioLoop
        (
            const string& fp_LogOutputDirectory, 
            const float fp_InitialVolume,
            latch& fp_InitLatch
        )
    {
        PEACH_TO_DO_UNUSED(fp_InitialVolume);

        if (not InitializeAudioEngine(fp_LogOutputDirectory))
        {

            return;
        }

        fp_InitLatch.count_down();

        AudioCommand f_AudioCommand;

        while (pm_IsRunning.load(std::memory_order_acquire)) //WARNING: im not sure how queue timing could affect the wakeup, im pretty sure when i call queue() itll queue first reading thru the machine code THEN send a ticket uwu
        {
            // Block until main thread wakes us
            pm_AudioSemaphore.acquire();

            if (not pm_IsRunning.load(std::memory_order_acquire)) //Stop() will wake the thread and cause it to exit but idk if this is necessasary
            {
                break; // Double check after wake
            }
            
            do { //needa do this since i already acquired a ticket so needa process a command if i wanna keep the N tickets N jobs system otherwise it'll do N tickets N-1 jobs since the loop acquires a ticket as well
                
                if (not pm_AudioCommandQueue->try_dequeue(f_AudioCommand))
                {
                    break;
                }

                ProcessCommand(f_AudioCommand);

            } while (pm_AudioSemaphore.try_acquire());

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
        pm_AudioSemaphore.release(); // wake up to exit        
    }

    bool 
        AudioManager::InitializeAudioEngine
        (
            const string& fp_LogOutputDirectory
        )
    {
        //////////////////// Initialize Logger ////////////////////

        audio_logger = LogManager::get_single().CreateUniqueLogger("AudioManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_LogOutputDirectory);

        if (not audio_logger)
        {
            PEACH_PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: AudioManager failed to initialize the audio_thread logger >w<");
            return false;
        }

        audio_logger->Info("AudioThreadLogger successfully initialized", "AudioManager");

        //////////////////// Initialize Loading and Command Queues ////////////////////

        if (not InitializeLoadingQueue())
        {
            audio_logger->Fatal("Initialization failed: AudioManager was not able to obtain a valid LoadingQueue, exiting execution immediately", "AudioManager");
            return false;
        }

        InitializeAudioCommandQueue();

        //////////////////// Initialized Successfully! ////////////////////

        audio_logger->Info("AudioLogger successfully initialized", "AudioManager");

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

        pm_AudioCommandQueue = make_shared<AudioCommandPipe>();

        audio_logger->Info("AudioManager successfully initialized the audio command queue", "AudioManager");

        return true; //returns one and only one ptr to whoever initializes AudioManager, this is meant only for the main thread
    }

    [[nodiscard]] shared_ptr<AudioCommandPipe>
        AudioManager::GetAudioCommandQueue(Logger*const logger)
    {
        if (not logger)
        {
            PEACH_PRINT_ERROR("TRIED TO PASS NULL_PTR REF TO LOGGER INSIDE GetAudioCommandQueue()");
            return nullptr;
        }
        else if (not pm_IsInitialized)
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
        AudioManager::ProcessCommand(const AudioCommand& fp_AudioCommand)
    {
        //ye
        PEACH_TO_DO_UNUSED(fp_AudioCommand);
    }

    void 
        AudioManager::OnResourceTransfer(const ResourceTransfer& fp_ResourceTransfer)
    {
        PEACH_TO_DO_UNUSED(fp_ResourceTransfer);

        //if (not holds_alternative<unique_ptr<AudioData>>(rt.Payload))
        //{
        //    return;
        //}

        //auto audio = get<unique_ptr<AudioData>>(rt.Payload).get();

        //ma_sound sound{};
        //ma_uint32 flags = 0;

        //if (audio->streamed)
        //{
        //    flags |= MA_SOUND_FLAG_STREAM;
        //}

        //// Path here is virtual / PhysFS – you can use custom IO callbacks if needed.
        //if 
        //(
        //    ma_sound_init_from_file
        //    (
        //        &m_Engine,
        //        audio->virtualPath.c_str(),
        //        flags,
        //        &m_SFXGroup, // or music group, or chosen group
        //        nullptr,
        //        &sound
        //    ) 
        //    == MA_SUCCESS
        //)
        //{
        //    if (audio->streamed)
        //    {
        //        m_StreamedSounds.emplace(audio->id, move(sound));
        //    }
        //    else
        //    {
        //        m_Sounds.emplace(audio->id, move(sound));
        //    }
        //}
        //else
        //{
        //    // log error
        //}
    }
}