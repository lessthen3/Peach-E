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

///font stuff
#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>

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
    PeachCore::Print(std::to_string(PeachCore::NullResources::GetDefaultFontSize()));

    std::cout << fp_ArgVector[0] << "\n"; //COOL AF

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
        PeachCore::PrintError("Failed to find the top-level directory 'Peach-E'!", PeachCore::Colours::Magenta);
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
            PeachCore::PrintError("Failed to initialize Peach Editor properly uwu");
            return EXIT_FAILURE;
        }

        peach_editor->StartPeachEditorMainLoop();

        return EXIT_SUCCESS;
    }
    catch (const std::exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PeachCore::PrintError(std::format("Unhandled exception: {}", Exception.what()));

        return EXIT_FAILURE;
    }
}
