/*******************************************************************
 *                     Peach Editor v0.0.7
 *             Created by Ranyodh Mandur - 🍑 2024
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#include "Managers/PeachEditorManager.h"

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
    main(int fp_ArgCount, const char* fp_ArgVector[])
{
    // // freetype smoke test
    // {
    //     FT_Library f_FtLibrary;
    //     FT_Error f_FtError = FT_Init_FreeType(&f_FtLibrary);

    //     if (f_FtError)
    //     {
    //         PeachCore::PRINT_ERROR("FreeType init FAILED");
    //     }
    //     else
    //     {
    //         PeachCore::Print("FreeType init OK", PeachCore::Colours::BrightGreen);
    //         FT_Done_FreeType(f_FtLibrary);
    //     }
    // }

    // // msdfgen-core smoke test
    // {
    //     msdfgen::Shape f_Shape;
    //     msdfgen::Contour& f_Contour = f_Shape.addContour();
    //     f_Contour.addEdge(msdfgen::EdgeHolder(msdfgen::Point2(0, 0), msdfgen::Point2(1, 0)));
    //     PeachCore::Print("msdfgen-core OK", PeachCore::Colours::BrightGreen);
    // }

    // // msdfgen-ext smoke test
    // {
    //     msdfgen::FreetypeHandle* f_Ft = msdfgen::initializeFreetype();

    //     if (not f_Ft)
    //     {
    //         PeachCore::PRINT_ERROR("msdfgen-ext FreeType init FAILED");
    //     }
    //     else
    //     {
    //         PeachCore::Print("msdfgen-ext init OK", PeachCore::Colours::BrightGreen);
    //         msdfgen::deinitializeFreetype(f_Ft);
    //     }
    // }
    //// jolt smoke test — if this links and runs ur golden nyaa~
    //{
    //    JPH::RegisterDefaultAllocator();
    //    JPH::Factory::sInstance = new JPH::Factory();
    //    JPH::RegisterTypes();
    //    PeachCore::Print("Jolt init OK", PeachCore::Colours::BrightGreen);
    //    JPH::UnregisterTypes();
    //    delete JPH::Factory::sInstance;
    //    JPH::Factory::sInstance = nullptr;
    //}

    //// box2d smoke test
    //{
    //    b2WorldDef f_WorldDef = b2DefaultWorldDef();
    //    b2WorldId f_WorldId = b2CreateWorld(&f_WorldDef);
    //    PeachCore::Print("box2d init OK", PeachCore::Colours::BrightGreen);
    //    b2DestroyWorld(f_WorldId);
    //}

    PRINT(fp_ArgVector[0], PeachCore::Colours::BrightMagenta); //COOL AF

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
        PRINT("Failed to find the top-level directory 'Peach-E'!", PeachCore::Colours::Magenta);
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

        auto peach_editor = &PeachEditor::PeachEditorManager::get_single();

        if (not peach_editor->InitializePeachEditor(mf_PeachERootPath))
        {
            PRINT_ERROR("Failed to initialize Peach Editor properly uwu");
            return EXIT_FAILURE;
        }

        peach_editor->StartPeachEditorMainLoop();

        return EXIT_SUCCESS;
    }
    catch (const std::exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PRINT_ERROR(fmt::format("Unhandled exception: {}", Exception.what()));
        return EXIT_FAILURE;
    }
}
