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
#ifdef PEACH_PLATFORM_WINDOWS
#   include <windows.h>
#endif

#include "CrashSignalHandler.h"

#include <csignal>
#include <cstring>      // for strlen — async-signal-safe
#include <fcntl.h>

#ifdef PEACH_PLATFORM_WINDOWS
#   include <io.h>
#   include <process.h>
#   include <sys/stat.h>
#
#   define STDERR_FILENO 2
#   define PEACH_CRASH_WRITE ::_write
#   define PEACH_CRASH_EXIT ::_exit
#elif defined(PEACH_PLATFORM_LINUX) || defined(PEACH_PLATFORM_APPLE) || defined(PEACH_PLATFORM_FREEBSD)
#   include <unistd.h>     // for write, _exit on POSIX
#
#   define PEACH_CRASH_WRITE ::write
#   define PEACH_CRASH_EXIT ::_exit
#else
#   error "Unsupported platform for crash signal handling owo"
#endif

namespace PeachCore::Debug {

    /* 
        Static path written at startup, read at crash time.
        Must be a stable buffer — no std::string, no allocation. 
    */
    static const char* s_CrashFlagPath = "./";
    // static char s_LogDirectoryPath[1024] = { 0 };

    // static void
    //     SetCrashFlagPath(const char* fp_Path)
    //     noexcept
    // {
    //     std::strncpy(s_CrashFlagPath, fp_Path, sizeof(s_CrashFlagPath) - 1);
    // }

    /*
        Async-signal-safe number-to-string. Returns length written.
        Avoids snprintf which is NOT async-signal-safe.
    */
    static int
        WriteIntToBuffer(char* fp_Buffer, int fp_Value)
        noexcept
    {
        if (fp_Value == 0)
        {
            fp_Buffer[0] = '0';
            return 1;
        }

        char f_Temp[16];
        int f_Len = 0;

        bool f_Negative = fp_Value < 0;

        if (f_Negative)
        {
            fp_Value = -fp_Value;
        }

        while (fp_Value > 0)
        {
            f_Temp[f_Len++] = '0' + (fp_Value % 10);
            fp_Value /= 10;
        }

        int f_Out = 0;

        if (f_Negative)
        {
            fp_Buffer[f_Out++] = '-';
        }

        for (int i = f_Len - 1; i >= 0; --i)
        {
            fp_Buffer[f_Out++] = f_Temp[i];
        }

        return f_Out;
    }

    extern "C" void
        CrashSignalHandler(int fp_Signal)
        noexcept
    {
        // Reset to default handler so a re-entrant crash kills the process
        std::signal(fp_Signal, SIG_DFL);

        // Write a fixed message to stderr — async-signal-safe
        const char* f_Msg = "\n[PEACH FATAL] crash signal received: ";
        PEACH_CRASH_WRITE(STDERR_FILENO, f_Msg, std::strlen(f_Msg));

        char f_NumBuf[16];
        int f_NumLen = WriteIntToBuffer(f_NumBuf, fp_Signal);
        PEACH_CRASH_WRITE(STDERR_FILENO, f_NumBuf, f_NumLen);
        PEACH_CRASH_WRITE(STDERR_FILENO, "\n", 1);

        /*
            Write a crash flag file so the launcher knows we crashed
            Use raw open/write/close — async-signal-safe on POSIX
        */ 
        if (s_CrashFlagPath[0] != '\0')
        {
            #ifdef PEACH_PLATFORM_WINDOWS
                // Windows: _open is technically not in the strict POSIX safe list
                // but on Windows you don't have async-signal semantics anyway.
                int f_Fd = ::_open(s_CrashFlagPath, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
            #else
                int f_Fd = ::open(s_CrashFlagPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            #endif

            if (f_Fd >= 0)
            {
                PEACH_CRASH_WRITE(f_Fd, f_NumBuf, f_NumLen);
                PEACH_CRASH_WRITE(f_Fd, "\n", 1);
                
                #ifdef PEACH_PLATFORM_WINDOWS
                    ::_close(f_Fd);
                #else
                    ::close(f_Fd);
                #endif
            }
        }

        PEACH_CRASH_EXIT(128 + fp_Signal);
    }

    void
        InstallCrashHandler()
        noexcept
    {
        #if defined(PEACH_PLATFORM_DESKTOP)

            std::signal(SIGSEGV, CrashSignalHandler);
            std::signal(SIGABRT, CrashSignalHandler);
            std::signal(SIGFPE, CrashSignalHandler);
            std::signal(SIGILL, CrashSignalHandler);

            #ifdef SIGBUS
                std::signal(SIGBUS, CrashSignalHandler);
            #endif

        #elif defined(PEACH_PLATFORM_IOS) || defined(PEACH_PLATFORM_TVOS)
        // No-op — let Apple's CrashReporter handle it

        #elif defined(PEACH_PLATFORM_ANDROID)
        // No-op — let Android debuggerd generate tombstones

        #elif defined(PEACH_PLATFORM_WASM)
            // Install JS-side error handler via EM_ASM
            EM_ASM
            (
                {
                    window.addEventListener
                    (
                        'error',
                        function(e)
                        {
                            console.error('Peach-E:', e.message);
                        }
                    );
                }
            );

        #elif defined(PEACH_PLATFORM_VITA)
            // Vita has limited signal support — install what's available
            std::signal(SIGSEGV, CrashSignalHandler);
            std::signal(SIGABRT, CrashSignalHandler);
        #else
            // Unknown platform, install standard signals defensively
            std::signal(SIGSEGV, CrashSignalHandler);
        #endif
    }

    #ifdef PEACH_PLATFORM_WINDOWS
        static LONG WINAPI
            PeachVectoredHandler(PEXCEPTION_POINTERS ExceptionInfo)
        {
            // Check if it's a fatal crash exception
            DWORD code = ExceptionInfo->ExceptionRecord->ExceptionCode;

            if (code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_ILLEGAL_INSTRUCTION)
            {
                // You can convert Windows codes to your internal signal format
                // and call your existing CrashSignalHandler logic here.
                CrashSignalHandler(SIGSEGV);
            }

            // Return CONTINUE_SEARCH to let other handlers (or the OS) handle it next
            return EXCEPTION_CONTINUE_SEARCH;
        }

        static void
            InstallVEH()
        {
            // 1 = Call this handler first
            AddVectoredExceptionHandler(1, PeachVectoredHandler);
        }
    #endif  
}
