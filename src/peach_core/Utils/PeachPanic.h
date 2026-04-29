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

///STL
#include <source_location>

///PeachCore
#include "PeachForceInline.h"
#include "PeachPrint.h"

///SDL
#include <SDL3/SDL_messagebox.h>

namespace PeachCore {
    [[noreturn]] PEACH_FORCEINLINE void 
        PanicExit
        (
            const char* fp_Condition,
            const char* fp_Message, //XXX: the message should always be a const char* string literal owo
            const std::source_location& fp_Loc = std::source_location::current()
        )
        noexcept
    {

        char f_UserMsg[2048];
        std::snprintf
        (
            f_UserMsg, sizeof(f_UserMsg),
            "Peach-E encountered a fatal error and must close.\n\n"
            "Condition: %s\n"
            "Message: %s\n"
            "Location: %s:%u\n"
            "Function: %s\n\n"
            "Please report this to the developer.",
            fp_Condition,
            fp_Message,
            fp_Loc.file_name(),
            fp_Loc.line(),
            fp_Loc.function_name()
        );

        PEACH_PRINT_ERROR_FMT
        (
            "\n[PEACH PANIC]\n  condition : {}\n  file      : {}\n  line      : {}\n  func      : {}\n", 
            fp_Condition, 
            fp_Loc.file_name(), 
            fp_Loc.line(), 
            fp_Loc.function_name()
        );

        // Show platform native dialog via SDL3
        SDL_ShowSimpleMessageBox
        (
            SDL_MESSAGEBOX_ERROR,
            "Peach-E Fatal Error",
            f_UserMsg,
            nullptr  // no parent window — works even if SDL_Init failed partway
        );

        // Flush all log files, close handles
        // Logger::FlushAllLoggers();

        std::exit(-69420); 
    }
}

#define PEACH_PANIC_IF(fp_Condition, fp_Message) ((fp_Condition) ? (void)0 : ::PeachCore::PanicExit(#fp_Condition, fp_Message))