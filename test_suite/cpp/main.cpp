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
#define PEACH_USING_OS_TERMINAL //needa do this since logger is header only atm

#include <managers/GameManager.h>


#define BUILD_PEACH_SERIALIZER_TEST ////////////////////////////////////////////// Here just for a bit for testing the testing suite UwU

#ifdef BUILD_PEACH_SERIALIZER_TEST
#   include "tests/serialization/Cereal.h"
#endif

#ifdef BUILD_PEACH_LOGGER_TEST
#   include "tests/logging/LoggerTest.h"
#endif 

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

int 
    main(int, const char* fp_ArgVector[]) //This method kinda clean ngl lmfao
{
    // std::cout << "Hello World!\n"; //Used this to test the projects first start up uwu

    ////////////////////////////////////////////// Lazily Look for Tests Directory from Executable Build Aritfact Directory //////////////////////////////////////////////

    std::filesystem::path mf_ExePath = std::filesystem::absolute(fp_ArgVector[0]);
    std::filesystem::path mf_TopLevelDir = mf_ExePath.parent_path();  // Start from the executable directory

    ////////////////////////////////////////////// Traverse Up to Peach-E Directory //////////////////////////////////////////////

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

    ////////////////////////////////////////////// itll just leave the string unaffected for good OS' like linux or linux im not gonna say mac beacuse that shit fucking sucks //////////////////////////////////////////////

    ReplaceChar(&mf_PeachERootPath, '\\', '/'); //XXX: used to relace stupid windows shit

    ////////////////////////////////////////////// Setup the Testing Logger //////////////////////////////////////////////

    std::unique_ptr<PeachCore::Logger> testing_logger = nullptr;
    
    const std::string f_TestsRootDir = mf_PeachERootPath + "/test_suite/tests";

    testing_logger = PeachCore::Logger::CreateUnique("PeachTests", PEACH_LOGGER_DEFAULT_FLAGS, f_TestsRootDir + "/logs");

    if (not testing_logger)
    {
        PEACH_PRINT_ERROR("Initialization error: Was not able to initialize PeachTests' main logger");
        return EXIT_FAILURE;
    }

    testing_logger->Debug("Main editor logger successfully initialized", "PeachTests");

    ////////////////////////////////////////////// Try to Initialize Peach-E and Run Desired Tests //////////////////////////////////////////////

    try
    {
        // auto engine_manager = &PeachCore::GameManager::get_single();

        // engine_manager->InitializePeachEngineCustom
        // (
        //     f_TestsRootDir,
        //     PeachCore::ThreadName::NO_THREAD,
        //     PeachCore::RendererType::Vulkan
        // );

        ////////////////////////////////////////////// Serializer Tests //////////////////////////////////////////////

#ifdef BUILD_PEACH_SERIALIZER_TEST
        PEACH_PRINT("\n====================================================== Starting Serializer Test ======================================================", PEACH_COL_BRIGHT_MAGENTA);

        //PeachTests::RunSerializerPODTests(f_TestsRootDir + "/serialization", testing_logger.get());
        PeachTests::RunSerializerPODBinaryTests(testing_logger.get());

        PEACH_PRINT("====================================================== Ending Serializer Test ======================================================", PEACH_COL_BRIGHT_MAGENTA);
#endif /*BUILD_PEACH_SERIALIZER_TEST*/

        //uint32_t I = static_cast<uint32_t>( - 1);

        ////uint32_t UI = 1U << 31 | 1U << 30;

        //PeachCore::Print("As Uint: " + std::to_string(I));

        //int32_t New = static_cast<int32_t>(I);

        //PeachCore::Print("As int: " + std::to_string(New));

        ////////////////////////////////////////////// Vulkan Texture Tests //////////////////////////////////////////////

#ifdef BUILD_PEACH_VULKAN_TEXTURE_TEST

#endif /*BUILD_PEACH_VULKAN_TEXTURE_TEST*/

#ifdef BUILD_PEACH_LOGGER_TEST

        //PeachTests::RunLoggerTests(f_TestsRootDir + "/logging", testing_logger.get());

        // engine_manager->ShutdownPeachEngine();
#endif /*BUILD_PEACH_LOGGER_TEST*/

        return EXIT_SUCCESS;
    }

    catch (const std::exception& Exception) ///Try to ensure all destructors are called especially close() on LogManager
    {
        PEACH_PRINT_ERROR_FMT("Unhandled exception: {}", Exception.what());
        return EXIT_FAILURE;
    }
}
