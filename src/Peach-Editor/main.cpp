/*******************************************************************
 *                                        Peach Editor v0.0.7
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *              Peach Editor is a free open source editor for Peach-E
********************************************************************/

#include "../../include/Peach-Editor/Managers/PeachEditorManager.h"

#include <csignal>

#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>

#include <zlib.h>

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

#include "clipper2/clipper.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <sodium.h>


using namespace Clipper2Lib;

static void 
    SegFaultHandler(int fp_Signal) //primitive segfault handler
{
    PeachCore::PrintError(format("[!] Crash signal received: {}, FATAL_SEGMENTATION_FAULT", fp_Signal));
    // possibly notify watchdog or dump stack trace
    exit(FATAL_SEGMENTATION_FAULT); //clean exit so everything calls their destructors
}

static inline constexpr void 
    ReplaceChar(string* fp_String, char fp_OldChar, char fp_NewChar)
{
    for (size_t i = 0; i < fp_String->length(); ++i)
    {
        if ((*fp_String)[i] == fp_OldChar)
        {
            (*fp_String)[i] = fp_NewChar;
        }
    }
}

inline Path64 MakeStar(const Point64& center, int radius, int points)
{
    if (!(points % 2)) --points;
    if (points < 5) points = 5;
    Path64 tmp = Ellipse<int64_t>(center, radius, radius, points);
    Path64 result;
    result.reserve(points);
    result.push_back(tmp[0]);
    for (int i = points - 1, j = i / 2; j;)
    {
        result.push_back(tmp[j--]);
        result.push_back(tmp[i--]);
    }
    return result;
}

void DoSimpleTest(bool show_solution_coords)
{
    Paths64 tmp, solution;
    FillRule fr = FillRule::NonZero;

    Paths64 subject, clip;
    subject.push_back(MakeStar(Point64(225, 225), 220, 9));
    clip.push_back(Ellipse<int64_t>(Point64(225, 225), 150, 150));

    //Intersect both shapes and then 'inflate' result -10 (ie deflate)
    solution = Intersect(subject, clip, fr);
    solution = InflatePaths(solution, -10, JoinType::Round, EndType::Polygon);
}

//////////////////////////////////////////////
// MAIN FUNCTION BABY
//////////////////////////////////////////////

int 
    main(int fp_ArgCount, const char* fp_ArgVector[])
{
    signal(SIGSEGV, SegFaultHandler); //XXX: used for trying to close and flush logs on seg fault

    cout << fp_ArgVector[0] << "\n"; //COOL AF

    //WARNING: WE ONLY USE THIS FOR DEVELOPMENT, FOR DEPLOYMENT WE NEED THIS DIRECTORY TO BE THE BASE DIR OF THE EXECUTABLE
    // Get the full path of the executable
    filesystem::path mf_ExePath = filesystem::absolute(fp_ArgVector[0]);
    filesystem::path mf_TopLevelDir = mf_ExePath.parent_path();  // Start from the executable directory

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

    string mf_PeachERootPath = mf_TopLevelDir.string();

    //itll just leave the string unaffected for good OS' like linux or linux im not gonna say mac beacuse that shit fucking sucks
    ReplaceChar(&mf_PeachERootPath, '\\', '/'); //XXX: used to relace stupid windows shit

    //////////////// TESTING

    printf("************* Testing libsodium ***************\n");

    msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
    if (ft) {
        std::cout << "Test" << std::endl; // This should be printed
    }

    printf("ZLIB VERSION: %s\n", zlibVersion());

    JPH::RegisterDefaultAllocator();
    auto factory = JPH::Factory();
    JPH::UnregisterTypes();
       

    DoSimpleTest(false);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(mf_PeachERootPath + "/res/models/cube.obj",
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    if (!scene) {
        return 1;
    }

    if (sodium_init() == -1) {
        printf("\tFAIL\n");
        return 1;
    }
    printf("\tOK\n");

    printf("***********************************************\n");

    ////////////////////////////////////////////////
    // Setup Environment
    ////////////////////////////////////////////////
    try
    {
        auto peach_editor = &PeachEditor::PeachEditorManager::get_single();

        peach_editor->InitializePeachEditor
        (
            mf_PeachERootPath
        );

        peach_editor->StartPeachEditorMainLoop();

        return EXIT_SUCCESS;
    }
    catch (const exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PeachCore::PrintError(format("Unhandled exception: {}", Exception.what()));

        return EXIT_FAILURE;
    }
}
