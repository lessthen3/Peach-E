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
#include <cstring>      // for strlen — async signal safe aka ass
#include <fcntl.h>

#ifdef PEACH_PLATFORM_WINDOWS
#   include <io.h>
#   include <process.h>
#   include <sys/stat.h>
#   define STDERR_FILENO 2
#   define PEACH_CRASH_WRITE ::_write
#   define PEACH_CRASH_EXIT ::_exit
#elif defined(PEACH_PLATFORM_LINUX) || defined(PEACH_PLATFORM_APPLE) || defined(PEACH_PLATFORM_FREEBSD) || defined(PEACH_PLATFORM_ANDROID)
#   include <unistd.h>     // for write, _exit on POSIX
#   define PEACH_CRASH_WRITE ::write
#   define PEACH_CRASH_EXIT ::_exit
#elif defined(PEACH_PLATFORM_WASM)
#   include <emscripten/emscripten.h>
#   include <cstdio>
#   include <unistd.h>     // for write, _exit on POSIX
#   define PEACH_CRASH_WRITE ::write
#   define PEACH_CRASH_EXIT ::_exit
#elif defined(PEACH_PLATFORM_IOS) || defined(PEACH_PLATFORM_TVOS)
// iOS/tvOS delegate crash reporting to the platform (Xcode/Instruments), CrashSignalHandler compiles but InstallCrashHandler's iOS branch is a no-op
// so this is dead code. Define stubs to satisfy the compiler.
#   include <unistd.h>
#   define PEACH_CRASH_WRITE(fd, buf, len) ((void)0)
#   define PEACH_CRASH_EXIT(code)          ::_exit(code)
#   define STDERR_FILENO 2
#else
#   error "Unsupported platform for crash signal handling owo"
#endif

namespace PeachCore::Debug {

    /* 
        Static path written at startup, read at crash time.
        Must be a stable buffer — no std::string, no allocation. 
    */
    static char* s_CrashFlagPath;
    static char s_LogDirectoryPath[1024] = { 0 };

    void
        SetCrashFlagPath(const char* fp_Path) //TODO ACTUALLY FIX THIS
        noexcept
    {
        if(not fp_Path)
        {
            return; //idk this is broken
        }

        // std::strncpy_s(s_CrashFlagPath, fp_Path, sizeof(s_CrashFlagPath) - 1); //might need to use strncpy_s
        // // snprintf is on everything, handles null-termination, and is "safe"
        // // Use the array buffer s_LogDirectoryPath, not the pointer s_CrashFlagPath!
        // snprintf(s_LogDirectoryPath, sizeof(s_LogDirectoryPath), "%s", fp_Path);
    }

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
            Write a crash flag file so the launcher knows we crashed, using raw open/write/close since it's async signal safe on POSIX
        */ 
        if (s_CrashFlagPath[0] != '\0')
        {
            #ifdef PEACH_PLATFORM_WINDOWS // Windows doesn't have async-signal semantics ;w; but the kernel guys always have smth just as good or better than POSIX
                int f_Fd = ::_sopen_s(s_CrashFlagPath, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
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

    #if defined(PEACH_PLATFORM_DESKTOP)

        void
            InstallCrashHandler()
            noexcept
        {
            std::signal(SIGSEGV,CrashSignalHandler);
            std::signal(SIGABRT,CrashSignalHandler);
            std::signal(SIGFPE, CrashSignalHandler);
            std::signal(SIGILL, CrashSignalHandler);

            #ifdef SIGBUS
                std::signal(SIGBUS, CrashSignalHandler);
            #endif
        }

    #elif defined(PEACH_PLATFORM_VITA)

        void
            InstallCrashHandler()
            noexcept
        {
            // Vita has limited signal support — install what's available
            std::signal(SIGSEGV, CrashSignalHandler);
            std::signal(SIGABRT, CrashSignalHandler);
        }

    #elif defined(PEACH_PLATFORM_ANDROID)

        void
            InstallCrashHandler()
            noexcept
        {
            // No-op — let Android debuggerd generate tombstones

        }

    #elif defined(PEACH_PLATFORM_IOS) || defined(PEACH_PLATFORM_TVOS)

        void
            InstallCrashHandler()
            noexcept
        {
            // No-op — let Apple's CrashReporter handle it
        }

    #elif defined(PEACH_PLATFORM_WASM)
        //register a JS window.onerror handler to surface WASM traps in the console.
        //this runs on the JS side and fires when any unhandled error/trap occurs.
        //EM_ASM embeds the JS literal directly — C++ code resumes after the closing brace.
        void
            InstallCrashHandler()
            noexcept
        {
            EM_ASM
            (
                window.addEventListener('unhandledrejection', function(fp_Event)
                {
                    console.error('[peach_core] unhandled promise rejection:', fp_Event.reason);
                });

                window.onerror = function(fp_Message, fp_Source, fp_Line, fp_Col, fp_Error)
                {
                    console.error('[peach_core] window.onerror:', fp_Message, 
                        'at', fp_Source + ':' + fp_Line + ':' + fp_Col,
                        fp_Error ? fp_Error.stack : "");
                    return false; //return false to NOT suppress the default browser error reporting
                };
            );

            std::fputs("[peach_core] WASM crash handler: JS window.onerror registered\n", stderr);
        }

    #endif

        // #ifdef
    //     void
    //         InstallCrashHandler() //default?
    //         noexcept
    //     {
            
    //     }
    // #endif /**/

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
