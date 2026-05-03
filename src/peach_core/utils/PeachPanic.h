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
#include "managers/LogManager.h"

///SDL
#include <SDL3/SDL_messagebox.h>

namespace PeachCore {

    [[nodiscard]] PEACH_FORCEINLINE static constexpr const char*
        PeachExtractFilename(const char* fp_Path)
    {
        const char* f_LastSlash = fp_Path;

        for (const char* lv_Cur = fp_Path; *lv_Cur != '\0'; ++lv_Cur)
        {
            if (*lv_Cur == '/' || *lv_Cur == '\\')
            {
                f_LastSlash = lv_Cur + 1;
            }
        }

        return f_LastSlash;
    }

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
            PeachExtractFilename(fp_Loc.file_name()),
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
        LogManager::get_single().ForceFlushAllLogggers();

        std::exit(-69420); 
    }
}

#define PEACH_PANIC_IF(fp_Condition, fp_Message) (__builtin_expect(!!(fp_Condition), 1) ? (void)0 : ::PeachCore::PanicExit(#fp_Condition, fp_Message))