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

#ifdef PEACH_PLATFORM_DESKTOP

namespace PeachCore {

    [[nodiscard]] int 
        LauncherMain(int argc, const char** argv);
    
    [[nodiscard]] bool 
        IsDebuggerAttached()
        noexcept;

}

#endif /*PEACH_PLATFORM_DESKTOP*/


// // peach_launcher.cpp
// #include <SDL3/SDL.h>
// #include <SDL3/SDL_messagebox.h>
// #include <cstdio>
// #include <cstdlib>
// #include <filesystem>
// #include <fstream>

// int main(int argc, char* argv[])
// {
//     namespace fs = std::filesystem;
    
//     fs::path f_GameExe = /* path to actual engine exe */;
//     fs::path f_CrashFlag = fs::temp_directory_path() / "peach_crash.flag";
//     fs::path f_LogDir = /* path to engine log directory */;
    
//     // Clear any stale crash flag from previous run
//     std::error_code f_EC;
//     fs::remove(f_CrashFlag, f_EC);
    
//     // Pass crash flag path to engine via env var or command line
//     std::string f_Cmd = f_GameExe.string() + " --crash-flag " + f_CrashFlag.string();
    
//     int f_ExitCode = std::system(f_Cmd.c_str());
    
//     // Engine exited. Was it a crash?
//     if (fs::exists(f_CrashFlag))
//     {
//         // Read the signal number from the flag
//         std::ifstream f_Flag(f_CrashFlag);
//         int f_Signal = 0;
//         f_Flag >> f_Signal;
        
//         // Now we can safely use SDL — we're in a fresh process
//         SDL_Init(SDL_INIT_VIDEO);
        
//         char f_Msg[2048];
//         std::snprintf(f_Msg, sizeof(f_Msg),
//             "Peach-E crashed with signal %d.\n\n"
//             "The engine has been shut down to protect your work.\n\n"
//             "A log has been saved to:\n%s\n\n"
//             "Please send the log to the developer.",
//             f_Signal,
//             f_LogDir.string().c_str());
        
//         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
//                                   "Peach-E Crashed",
//                                   f_Msg, nullptr);
        
//         SDL_Quit();
//         fs::remove(f_CrashFlag, f_EC);
//         return f_ExitCode;
//     }
    
//     return f_ExitCode;
// }