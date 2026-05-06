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
#include "PeachLauncher.h"

// for showing crash dialog from the parent process owo
#include <SDL3/SDL.h>
#include <SDL3/SDL_messagebox.h>

#include <cstdio>

#ifdef PEACH_PLATFORM_WINDOWS
#   include <windows.h>
#   include <debugapi.h>
#elif defined(PEACH_PLATFORM_LINUX)
#   include <fstream>
#   include <string>
#   include <unistd.h>
#   include <sys/wait.h>
#elif defined(PEACH_PLATFORM_APPLE)
#   include <sys/sysctl.h>
#   include <unistd.h>
#endif

namespace PeachCore {

    #ifdef PEACH_PLATFORM_WINDOWS    
        bool 
            IsDebuggerAttached() 
            noexcept 
        {
            return ::IsDebuggerPresent() != 0;
        }
    #elif defined(PEACH_PLATFORM_LINUX)
        bool 
            IsDebuggerAttached()
            noexcept 
        {
            std::ifstream f_Status("/proc/self/status");
            std::string f_Line;

            while (std::getline(f_Status, f_Line)) 
            {
                if (f_Line.compare(0, 11, "TracerPid:\t") == 0) 
                {
                    return std::stoi(f_Line.substr(11)) != 0;
                }
            }

            return false;
        }
    #elif defined(PEACH_PLATFORM_APPLE)
        bool 
            IsDebuggerAttached()
            noexcept 
        {
            int f_MIB[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, ::getpid() };
            struct kinfo_proc f_Info{};
            size_t f_Size = sizeof(f_Info);

            if (sysctl(f_MIB, 4, &f_Info, &f_Size, nullptr, 0) == 0) 
            {
                return (f_Info.kp_proc.p_flag & P_TRACED) != 0;
            }

            return false;
        }
    #else
        bool 
            IsDebuggerAttached() 
            noexcept 
        { 
            return false; 
        }
    #endif
}

namespace PeachCore{

    static void
        ShowCrashDialogue(int fp_ExitCode)
    {
        char f_Msg[2048]; //we don't call SDL_Init here in the case that a gpu driver crashes owo

        std::snprintf
        (
            f_Msg, 
            sizeof(f_Msg),
            "Peach-E crashed with signal %d.\n\n"
            "The engine has been shut down to protect your work.\n\n"
            "A log has been saved to:\n%s\n\n"
            "Please send the log to the developer.",
            fp_ExitCode,
            "owo"
        );

        SDL_ShowSimpleMessageBox
        (
            SDL_MESSAGEBOX_ERROR,
            "Peach-E Crashed",
            f_Msg, 
            nullptr //no window created this is called from the parent process
        );
    }

#ifdef PEACH_PLATFORM_DESKTOP

    int 
        LauncherMain(int, const char** argv)
    {
        #if defined(PEACH_PLATFORM_WINDOWS)

            char f_CmdLine[MAX_PATH * 3];
            std::snprintf(f_CmdLine, sizeof(f_CmdLine), "\"%s\" --engine-child", argv[0]);
            
            STARTUPINFOA f_StartupInformation = { sizeof(f_StartupInformation) }; //looks weird buts fine
            PROCESS_INFORMATION f_ProcessInformation = {};
            
            if (not CreateProcessA(nullptr, f_CmdLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &f_StartupInformation, &f_ProcessInformation))
            {
                return 1;
            }
            
            WaitForSingleObject(f_ProcessInformation.hProcess, INFINITE);
            
            DWORD f_ExitCode = 0;
            GetExitCodeProcess(f_ProcessInformation.hProcess, &f_ExitCode);
            
            CloseHandle(f_ProcessInformation.hProcess);
            CloseHandle(f_ProcessInformation.hThread);
            
            // Windows reports crash exit codes in the high range (0xC0000005 = STATUS_ACCESS_VIOLATION etc.)
            if (f_ExitCode >= 0xC0000000) 
            {
                ShowCrashDialogue(static_cast<int>(f_ExitCode));
                return 1;
            }
            
            return f_ExitCode;

        #elif defined(PEACH_PLATFORM_LINUX) || defined(PEACH_PLATFORM_APPLE) || defined(PEACH_PLATFORM_FREEBSD)

            pid_t f_Pid = fork();
        
            if (f_Pid == 0) // Child process — exec the engine mode, Build new argv with --engine-child appended
            {
                const char* f_ChildArgs[] = { argv[0], "--engine-child", nullptr };
                execv(argv[0], const_cast<char**>(f_ChildArgs));
                _exit(127);  // exec failed
            }
            
            // as parent process wait for child, report on crash
            int f_Status = 0;
            waitpid(f_Pid, &f_Status, 0);
            
            if (WIFSIGNALED(f_Status)) // Child died by signal, show crash dialog
            {
                int f_Signal = WTERMSIG(f_Status);
                ShowCrashDialogue(f_Signal);

                return 128 + f_Signal; //idk y they do this but i like standard unlike some ppl *coughs in tim apple
            }
            
            return WEXITSTATUS(f_Status);
        
        #else 
        #   error "Unsupported platform for Peach Launcher ;w;"
        #endif
    }

#endif /*PEACH_PLATFORM_DESKTOP*/

} //namespace PeachCore
