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
#pragma once

#include <string>

namespace PeachCore{

    struct AudioStreamPlayer
    {
        void PlaySoundOnce(const std::string& soundFile); //SUSUSUSUSUSUSUSUSSSYYYY FUNCTION (is PlaySound a predefined funciton in openal?)
        std::string GetCurrentTrack() const;
        void SetCurrentTrack(const std::string& track);

        //idk get bpm of current track, probably uneccesary but could be useful for rhythm games where users can import custom audio files, and then can generate a bpm map for it
        float
            GetBPM();

        //used to sync a particular audio event to a user defined function handle to allow for easier integration of dynamic sound environments
        void
            SyncToEventCallback();
    };
}