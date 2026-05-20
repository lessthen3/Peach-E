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
#include "AudioStreamPlayer.h"

#include "debug/PeachPrint.h"

namespace PeachCore
{
    void
        AudioStreamPlayer::PlaySoundOnce(const std::string& fp_SoundFile)
    {
        PEACH_TO_DO_UNUSED(fp_SoundFile);
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

    std::string
        AudioStreamPlayer::GetCurrentTrack()
        const
    {
        //shared_lock<shared_mutex> lock(mutex_);
        //return pm_CurrentTrack;
        return "";
    }

    void
        AudioStreamPlayer::SetCurrentTrack(const std::string& fp_TrackName) //this doesnt need a lock since the command queue tells audiomanager to do this
    {
        PEACH_TO_DO_UNUSED(fp_TrackName);

        //pm_CurrentTrack = track;
    }

}
