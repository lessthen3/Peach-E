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

#include <fmt/format.h>

// ANSI colour codes
#define PEACH_COL_RESET            "\033[0m"

#define PEACH_COL_RED              "\x1B[31m"
#define PEACH_COL_GREEN            "\x1B[32m"
#define PEACH_COL_YELLOW           "\x1B[33m"
#define PEACH_COL_BLUE             "\x1B[34m"
#define PEACH_COL_MAGENTA          "\x1B[35m"
#define PEACH_COL_CYAN             "\x1B[36m"
#define PEACH_COL_WHITE            "\x1B[37m"

#define PEACH_COL_BRIGHT_RED       "\x1B[91m"
#define PEACH_COL_BRIGHT_GREEN     "\x1B[92m"
#define PEACH_COL_BRIGHT_YELLOW    "\x1B[93m"
#define PEACH_COL_BRIGHT_BLUE      "\x1B[94m"
#define PEACH_COL_BRIGHT_MAGENTA   "\x1B[95m"
#define PEACH_COL_BRIGHT_CYAN      "\x1B[96m"
#define PEACH_COL_BRIGHT_WHITE     "\x1B[97m"

// wrap a string literal in a colour, zero runtime cost, pure compile-time concat
#define PEACH_COLOURED(fp_Colour, fp_String) fp_Colour fp_String PEACH_COL_RESET

#ifdef PEACH_DEBUG
# define PEACH_USING_OS_TERMINAL
#endif /*PEACH_DEBUG*/

// print macros, get thrown out for release builds owo

// stdout and stderr are routed to /dev/null for Android app processes. The ONLY way to get output is through the Android log system. No ANSI codes ;w;
// fmt formats the message to a std::string first since __android_log_print is printf-style (%s/%d/etc), not fmt-style ({}/{:d}/etc).
//
// View output with: adb logcat -s peach_core:V
//
// fmt::format().c_str() is intentional, its lifetime extends to the end of the full-expression containing the __android_log_print call, so the pointer is valid when printf-style %s reads it.
#if defined(PEACH_PLATFORM_ANDROID) && defined(PEACH_DEBUG)
#   include <android/log.h>
#   define PEACH_ANDROID_LOG_TAG "peach_core"
#   define PEACH_PRINT(fp_String, fp_Colour) __android_log_print(ANDROID_LOG_INFO, PEACH_ANDROID_LOG_TAG, "%s", (fp_String))
#   define PEACH_PRINT_ERROR(fp_String) __android_log_print(ANDROID_LOG_ERROR, PEACH_ANDROID_LOG_TAG, "%s", (fp_String))
#   define PEACH_PRINT_FMT(fp_Colour, fp_Format, ...) __android_log_print(ANDROID_LOG_INFO, PEACH_ANDROID_LOG_TAG, "%s", fmt::format(fp_Format __VA_OPT__(,) __VA_ARGS__).c_str())
#   define PEACH_PRINT_ERROR_FMT(fp_Format, ...) __android_log_print(ANDROID_LOG_ERROR, PEACH_ANDROID_LOG_TAG, "%s", fmt::format(fp_Format __VA_OPT__(,) __VA_ARGS__).c_str())
#elif (defined(PEACH_PLATFORM_IOS) || defined(PEACH_PLATFORM_TVOS)) && defined(PEACH_DEBUG)
// Xcode's debugger console captures stderr from running apps, os_log has a completely different format string ABI incompatible with fmt so we use fprintf(stderr) instead. No ANSI codes .w.
#   include <cstdio>
#   define PEACH_PRINT(fp_String, fp_Colour) \
        do { ::std::fputs(fp_String, stderr); ::std::fputc('\n', stderr); } while(0)
#   define PEACH_PRINT_ERROR(fp_String) \
        do { ::std::fputs("[ERROR] ", stderr); ::std::fputs(fp_String, stderr); ::std::fputc('\n', stderr); } while(0)
#   define PEACH_PRINT_FMT(fp_Colour, fp_Format, ...) \
        do { \
            auto fv_Msg = fmt::format(fp_Format __VA_OPT__(,) __VA_ARGS__); \
            ::std::fputs(fv_Msg.c_str(), stderr); \
            ::std::fputc('\n', stderr); \
        } while(0)
#   define PEACH_PRINT_ERROR_FMT(fp_Format, ...) \
        do { \
            auto fv_Msg = fmt::format(fp_Format __VA_OPT__(,) __VA_ARGS__); \
            ::std::fputs("[ERROR] ", stderr); \
            ::std::fputs(fv_Msg.c_str(), stderr); \
            ::std::fputc('\n', stderr); \
        } while(0)
#elif defined(PEACH_PLATFORM_WASM) && defined(PEACH_DEBUG) //Emscripten maps stdout->console.log and stderr->console.error, ANSI codes appear literally as "\x1B[32m" so we ignore em (scripten)
#   define PEACH_PRINT(fp_String, fp_Colour) fmt::print(stdout, "{}\n", fp_String)
#   define PEACH_PRINT_ERROR(fp_String) fmt::print(stderr, "[ERROR] {}\n", fp_String)
#   define PEACH_PRINT_FMT(fp_Colour, fp_Format, ...) fmt::print(stdout, fp_Format "\n" __VA_OPT__(,) __VA_ARGS__)
#   define PEACH_PRINT_ERROR_FMT(fp_Format, ...) fmt::print(stderr, "[ERROR] " fp_Format "\n" __VA_OPT__(,) __VA_ARGS__)
#elif defined(PEACH_USING_OS_TERMINAL) && defined(PEACH_PLATFORM_DESKTOP) //Desktop platforms only owo
#   define PEACH_PRINT(fp_String, fp_Colour) fmt::print(stdout, "{}{}{}\n", fp_Colour, fp_String, PEACH_COL_RESET)
#   define PEACH_PRINT_ERROR(fp_String) fmt::print(stderr, "{}{}{}\n", PEACH_COL_BRIGHT_RED, fp_String ,PEACH_COL_RESET)
#   define PEACH_PRINT_FMT(fp_Colour, fp_Format, ...) fmt::print(stdout, "{}" fp_Format "{}\n", fp_Colour, __VA_OPT__(__VA_ARGS__,) PEACH_COL_RESET)
#   define PEACH_PRINT_ERROR_FMT(fp_Format, ...) fmt::print(stdout, "{}" fp_Format "{}\n", PEACH_COL_BRIGHT_RED, __VA_OPT__(__VA_ARGS__,) PEACH_COL_RESET)
#else
#   define PEACH_PRINT(fp_String, fp_Colour)                   ((void)0)
#   define PEACH_PRINT_ERROR(fp_String)                        ((void)0)
#   define PEACH_PRINT_FMT(fp_Colour, fp_Format, ...)          ((void)0)
#   define PEACH_PRINT_ERROR_FMT(fp_Colour, fp_Format, ...)    ((void)0)
#endif /*PEACH_USING_OS_TERMINAL*/


//XXX: meant to be grepable owo, avoids the -Wunused spam from clang/gcc
#define PEACH_TO_DO_UNUSED(fp_Action) (void)(fp_Action)


// #elif defined(PEACH_PLATFORM_WASM) && defined(PEACH_DEBUG)

//     #include <emscripten/emscripten.h>

//     #define PEACH_PRINT(fp_String, fp_Colour) \
//         emscripten_log(EM_LOG_CONSOLE, "%s", (fp_String))

//     #define PEACH_PRINT_ERROR(fp_String) \
//         emscripten_log(EM_LOG_ERROR, "%s", (fp_String))

//     #define PEACH_PRINT_FMT(fp_Colour, fp_Format, ...) \
//         emscripten_log(EM_LOG_CONSOLE, "%s", \
//             fmt::format(fp_Format __VA_OPT__(,) __VA_ARGS__).c_str())

//     #define PEACH_PRINT_ERROR_FMT(fp_Format, ...) \
//         emscripten_log(EM_LOG_ERROR, "%s", \
//             fmt::format(fp_Format __VA_OPT__(,) __VA_ARGS__).c_str())