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

#ifdef PEACH_DEBUG

    #include "PeachForceInline.h"
    #include "PeachPrint.h"

    #include <source_location>

//================================================================================ Assert ================================================================================//

    namespace PeachCore{
        [[noreturn]] PEACH_FORCEINLINE void
            AssertFail
            (
                const char* fp_Condition,
                const std::source_location& fp_Loc = std::source_location::current()
            ) 
            noexcept
        {
            fmt::print
            (
                stderr,
                PEACH_COL_BRIGHT_RED
                "\n[PEACH ASSERT FAILED]\n"
                "  condition : {}\n"
                "  file      : {}\n"
                "  line      : {}\n"
                "  func      : {}\n"
                PEACH_COL_RESET "\n",
                fp_Condition, fp_Loc.file_name(), fp_Loc.line(), fp_Loc.function_name()
            );

            std::exit(-69420);
        }
    }

    #define PEACH_DEBUG_ASSERT(fp_Condition) (__builtin_expect(!!(fp_Condition), 1) ? (void)0 : ::PeachCore::AssertFail(#fp_Condition))

#else

    #define PEACH_DEBUG_ASSERT(fp_Condition) ((void)0)

#endif /*PEACH_DEBUG*/
