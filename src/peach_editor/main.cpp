/*******************************************************************
 *                     Peach Editor v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#include "managers/PeachEditorManager.h"
#include "managers/ProjectManager.h"

#include "utils/PeachPanic.h"
#include "utils/PeachPrint.h"

//#include <Jolt/Jolt.h>
//#include <Jolt/RegisterTypes.h>
//#include <Jolt/Core/Factory.h>
//#include <box2d/box2d.h>

// #include <ft2build.h>
// #include FT_FREETYPE_H
// #include <msdfgen.h>
// #include <msdfgen-ext.h>

static inline constexpr void 
    ReplaceChar(std::string* fp_String, char fp_OldChar, char fp_NewChar)
{
    for (size_t i = 0; i < fp_String->length(); ++i)
    {
        if ((*fp_String)[i] == fp_OldChar)
        {
            (*fp_String)[i] = fp_NewChar;
        }
    }
}

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////
int 
    main(int, const char* fp_ArgVector[])
{
    PeachCore::LogManager::get_single();
    // PEACH_PANIC_IF(false, "This is a test for panic owo");

    // // freetype smoke test
    // {
    //     FT_Library f_FtLibrary;
    //     FT_Error f_FtError = FT_Init_FreeType(&f_FtLibrary);

    //     if (f_FtError)
    //     {
    //        PEACH_PRINT_ERROR("FreeType init FAILED");
    //     }
    //     else
    //     {
    //         PEACH_PRINT("FreeType init OK", PEACH_COL_BRIGHT_GREEN);
    //         FT_Done_FreeType(f_FtLibrary);
    //     }
    // }

    // // msdfgen-core smoke test
    // {
    //     msdfgen::Shape f_Shape;
    //     msdfgen::Contour& f_Contour = f_Shape.addContour();
    //     f_Contour.addEdge(msdfgen::EdgeHolder(msdfgen::Point2(0, 0), msdfgen::Point2(1, 0)));
    //     PEACH_PRINT("msdfgen-core OK", PEACH_COL_BRIGHT_GREEN);
    // }

    // // msdfgen-ext smoke test
    // {
    //     msdfgen::FreetypeHandle* f_Ft = msdfgen::initializeFreetype();

    //     if (not f_Ft)
    //     {
    //         PEACH_PRINT_ERROR("msdfgen-ext FreeType init FAILED");
    //     }
    //     else
    //     {
    //         PEACH_PRINT("msdfgen-ext init OK", PEACH_COL_BRIGHT_GREEN);
    //         msdfgen::deinitializeFreetype(f_Ft);
    //     }
    // }
    //// jolt smoke test — if this links and runs ur golden nyaa~
    //{
    //    JPH::RegisterDefaultAllocator();
    //    JPH::Factory::sInstance = new JPH::Factory();
    //    JPH::RegisterTypes();
    //    PEACH_PRINT("Jolt init OK", PEACH_COL_BRIGHT_GREEN);
    //    JPH::UnregisterTypes();
    //    delete JPH::Factory::sInstance;
    //    JPH::Factory::sInstance = nullptr;
    //}

    //// box2d smoke test
    //{
    //    b2WorldDef f_WorldDef = b2DefaultWorldDef();
    //    b2WorldId f_WorldId = b2CreateWorld(&f_WorldDef);
    //    PEACH_PRINT("box2d init OK", PEACH_COL_BRIGHT_GREEN);
    //    b2DestroyWorld(f_WorldId);
    //}

    PEACH_PRINT(fp_ArgVector[0], PEACH_COL_BRIGHT_MAGENTA); //COOL AF

    //WARNING: WE ONLY USE THIS FOR DEVELOPMENT, FOR DEPLOYMENT WE NEED THIS DIRECTORY TO BE THE BASE DIR OF THE EXECUTABLE
    // Get the full path of the executable
    std::filesystem::path mf_ExePath = std::filesystem::absolute(fp_ArgVector[0]);
    std::filesystem::path mf_TopLevelDir = mf_ExePath.parent_path();  // Start from the executable directory

    // Traverse upwards until we find the "Peach-E" directory
    while (not mf_TopLevelDir.empty() and mf_TopLevelDir.filename() != "Peach-E")
    {
        mf_TopLevelDir = mf_TopLevelDir.parent_path();
    }

    if (mf_TopLevelDir.empty())
    {
        PEACH_PRINT("Failed to find the top-level directory 'Peach-E'!", PEACH_COL_MAGENTA);
        return EXIT_FAILURE;
    }

    std::string mf_PeachERootPath = mf_TopLevelDir.string();

    //itll just leave the string unaffected for good OS' like linux or linux im not gonna say mac beacuse that shit fucking sucks
    ReplaceChar(&mf_PeachERootPath, '\\', '/'); //XXX: used to relace stupid windows shit

    ////////////////////////////////////////////////
    // Setup Environment
    ////////////////////////////////////////////////
    try
    {
        PeachEditor::ProjectManager f_ProjectManager;

        f_ProjectManager.StartProjectManager();

        PeachEditor::PeachEditorManager peach_editor;

        if (not peach_editor.InitializePeachEditor(mf_PeachERootPath))
        {
            PEACH_PRINT_ERROR("Failed to initialize Peach Editor properly uwu");
            return EXIT_FAILURE;
        }

        peach_editor.StartPeachEditorMainLoop(); //ye

        PEACH_PRINT(PEACH_COL_BRIGHT_MAGENTA, "exited all loops and reached EXIT_SUCCESS, any errors after here are static destructors fuqqq");

        return EXIT_SUCCESS;
    }
    catch (const std::exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PEACH_PRINT_ERROR_FMT("Unhandled exception: {}", Exception.what());
        return EXIT_FAILURE;
    }
}
