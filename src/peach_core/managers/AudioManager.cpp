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
        AudioManager::AudioLoop()
    {
        audio_logger->UpdateThreadOwner();

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
                
                if (not pm_AudioCommandQueue.try_dequeue(f_AudioCommand))
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
        AudioManager::ShutdownSubsystem(Logger*const logger)
    {
        if(not logger) [[unlikely]]
        {
            PEACH_PRINT_ERROR("Tried to pass nullptr reference to logger inside AudioManager::ShutdownSubsystem()");
            return;
        }

        if(not pm_IsInitialized.load(std::memory_order_acquire))
        {
            logger->Error("Tried to call ShutdownSubsystem() on AudioManager when audio thread was never started owo wtf mang ;w;", "AudioManager"); 
            return;
        }

        pm_IsRunning.store(false, std::memory_order_release);
        pm_AudioSemaphore.release(); // wake up to exit     
        
        if (pm_AudioThread.joinable()) 
        { 
            pm_AudioThread.join(); 
            logger->Info("Successfully joined audio thread", "AudioManager");
        }
    }

    bool 
        AudioManager::InitializeAudioEngine
        (
            const float fp_InitialVolume,
            shared_ptr<AudioResourcePipe> fp_AudioResourcePipe,
            const string& fp_LogOutputDirectory
        )
    {
        PEACH_TO_DO_UNUSED(fp_InitialVolume);

        //////////////////// Guard against multiple initializations owo ////////////////////

        if(pm_IsInitialized.load(std::memory_order_acquire)) //XXX: can log here since initialization was successful before owo
        {
            audio_logger->Warning("Tried to initialize audio manager again, what are you doing? this request was ignored >w<", "AudioManager");
            return false;
        }

        //////////////////// Initialize Logger ////////////////////

        audio_logger = LogManager::get_single().CreateUniqueLogger("AudioManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_LogOutputDirectory);

        if (not audio_logger)
        {
            PEACH_PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: AudioManager failed to initialize the audio_thread logger >w<");
            return false;
        }

        audio_logger->Info("AudioThreadLogger successfully initialized", "AudioManager");

        //////////////////// Initialize Loading and Command Queues ////////////////////

        if (not fp_AudioResourcePipe)
        {
            audio_logger->Fatal("Initialization failed! AudioManager was not able to obtain a valid LoadingQueue, exiting execution immediately, did you pass a nullptr reference to the audio resource transfer pipe?", "AudioManager");
            return false;
        }

        pm_LoadedAudioResourceQueue = fp_AudioResourcePipe;

        audio_logger->Info("AudioManager successfully retrieved loaded resource queue from ResourceManager", "AudioManager");

        //////////////////// Start Thread >w< ////////////////////

        pm_AudioThread = thread
        (
            &AudioManager::AudioLoop,
            this
        );

        //////////////////// Initialized Successfully! ////////////////////

        audio_logger->Info("AudioLogger successfully initialized", "AudioManager");
        pm_IsInitialized.store(true);
        return true;
    }

    void 
        AudioManager::ShutdownAudioEngine()
    {
        //alcMakeContextCurrent(nullptr);
        //if (pm_Context) {alcDestroyContext(pm_Context);}
        //if (pm_Device) {alcCloseDevice(pm_Device);}
    }
    
    void 
        AudioManager::ProcessCommand(AudioCommand fp_AudioCommand)
    {
        //ye
        PEACH_TO_DO_UNUSED(fp_AudioCommand);
    }

    // void 
    //     AudioManager::OnResourceTransfer(const ResourceTransfer& fp_ResourceTransfer)
    // {
        // PEACH_TO_DO_UNUSED(fp_ResourceTransfer);

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
    // }
}