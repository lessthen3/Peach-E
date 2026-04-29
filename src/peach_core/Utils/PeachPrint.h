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

// print macros, gets thrown out for release builds owo
#ifdef PEACH_USING_OS_TERMINAL
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


//XXX: meant to be grepable owo
#define PEACH_TO_DO_UNUSED(fp_Action) (void)(fp_Action)
