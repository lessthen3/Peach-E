/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#pragma once

///PeachCore
#include "../Managers/LogManager.h"
#include "../Utils/Serializer.h"

///DotNet
#include <dotnet/hostfxr.h>
#include <dotnet/coreclr_delegates.h>

///STL
#include <cstdlib>
#include <stdio.h>

namespace PeachCore{

    struct ScriptInstance
    {
        void* OnEnter = nullptr;
        void* OnUpdate = nullptr;
        void* OnConstantUpdate = nullptr;
        void* OnExit = nullptr;
        void* ManagedInstance = nullptr; // ← the object created via Activator.CreateInstance

        string ScriptName;
    };

    struct RuntimeConfig //this looks kinda stupid but whatever thats how Serializer.h works uwu
    {
        struct RuntimeOptions
        {
            string tfm = "net6.0";
            string rollForward = "LatestMinor";

            struct Framework
            {
                string name = "Microsoft.NETCore.App";
                string version = "6.0.0";

                SERIALIZABLE_FIELDS(name, version)
            } framework; //needs to be named all lowercase since serializer.h will insert the var name into the json

            SERIALIZABLE_FIELDS(tfm, rollForward, framework)
        } runtimeOptions;

        SERIALIZABLE_FIELDS(runtimeOptions)
    };


    struct DotNetRuntimeContext
    {
        hostfxr_handle HostFxr = nullptr;

        hostfxr_initialize_for_runtime_config_fn RuntimeInit = nullptr;
        hostfxr_get_runtime_delegate_fn GetDelegate = nullptr;
        hostfxr_close_fn Close = nullptr;

        load_assembly_and_get_function_pointer_fn LoadAssembly = nullptr;

        load_assembly_and_get_function_pointer_fn CreateScriptInstance = nullptr;
        load_assembly_and_get_function_pointer_fn ReleaseScriptInstance = nullptr;

        load_assembly_and_get_function_pointer_fn CallOnEnter = nullptr;
        load_assembly_and_get_function_pointer_fn CallOnUpdate = nullptr;
        load_assembly_and_get_function_pointer_fn CallOnConstantUpdate = nullptr;
        load_assembly_and_get_function_pointer_fn CallOnExit = nullptr;

        string RuntimePath;
        string MainAssembyPath;
        string Version;

        vector<ScriptInstance> Scripts;

        RuntimeConfig RuntimeConfigs; //added the s here uwu me clever

        bool IsInitialized = false;
    };

    class DotNetUtils
    {
    public:
        DotNetUtils() = default;
        ~DotNetUtils() = default;

        DotNetRuntimeContext m_DotNetRuntimeContext;

        bool
            InitializeRuntime()
        {

            return true;
        }

        bool 
            GenerateProjectFiles
            (
                const string& fp_ProjectName, 
                const string& fp_ProjectPath, 
                const string& fp_PeachBridgePath,
                const string& fp_RelativeBuildOutputPath,
                LogManager* logger
            ) 
        {
            //////////////////// Check for nullptr ////////////////////

            if (not logger)
            {
                PrintError("Tried passing a nullptr ref to logger inside GenerateProjectFiles() from DotNetUtils.h");
                return false;
            }

            //////////////////// Create Directory if it doesn't exist ////////////////////

            if (not filesystem::exists(fp_ProjectPath))
            {
                logger->LogAndPrint(format("Failed to locate directory for C# project generation with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotNetUtils", LogManager::LogLevel::Error);
                return false;
            }

            //////////////////// Fetch dotnet Version from CLI ////////////////////

            string f_DotNetVersion;

            if(not GetDotNetVersion(&f_DotNetVersion, logger))
            {
                logger->LogAndPrint(format("Failed to retrieve dotnet version, failed to generate valid C# project with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotNetUtils", LogManager::LogLevel::Error);
                return false;
            }

            //////////////////// Validate/Enforce .NET version ////////////////////

            try //stoi can throw so y not its 0 cost if it doesnt throw UwU, not like we're calling this more than once per project gen
            {
                int f_DotNetMajorVersion = stoi(f_DotNetVersion.substr(0, f_DotNetVersion.find('.')));

                if (f_DotNetMajorVersion < 6)
                {
                    logger->LogAndPrint(format(".NET version '{}' found when Peach-E requires .NET SDK version 6.0 or higher", f_DotNetMajorVersion), "DotNetUtils", LogManager::LogLevel::Error);
                    return false;
                }
            }
            catch (const exception& Exception) //catch >O<
            {
                logger->LogAndPrint(format("Failed to parse .NET version number >O<, version found: '{}', with error: '{}'", f_DotNetVersion, Exception.what()), "DotNetUtils", LogManager::LogLevel::Error);
                return false;
            }

            //////////////////// Store Version in Context ////////////////////

            m_DotNetRuntimeContext.Version = f_DotNetVersion;

            //////////////////// Create .csproj File ////////////////////

            const string f_ProjectFullPath = fp_ProjectPath + "/" + fp_ProjectName + ".csproj";

            ofstream csproj(f_ProjectFullPath);

            if (not csproj.is_open())
            {
                logger->LogAndPrint(format("Failed to open csproj file for writing: '{}'", f_ProjectFullPath), "DotNetUtils", LogManager::LogLevel::Error);
                return false;
            }

            csproj 
                << "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                << "<Project Sdk=\"Microsoft.NET.Sdk\">"
                << "\n"
                << "  <PropertyGroup>"
                << "\n"
                << "    <TargetFramework>" << "net" << f_DotNetVersion << "</TargetFramework>"
                << "\n"
                << "    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>"
                << "\n"
                << "    <EnableDynamicLoading>true</EnableDynamicLoading>"
                << "\n"
                << "    <LangVersion>latest</LangVersion>"
                << "\n"
                << "    <OutputType>Library</OutputType>"
                << "\n"
                << "    <Nullable>enable</Nullable>"
                << "\n"
                << "    <OutputPath>" << fp_ProjectPath + "/" + fp_RelativeBuildOutputPath << "</OutputPath>"
                << "\n"
                << "  </PropertyGroup>"
                << "\n"
                << "  <ItemGroup>"
                << "\n"
                << "    <Reference Include=\"Peach\">"
                << "\n"
                << "      <HintPath>" << fp_PeachBridgePath << "</HintPath>"
                << "\n"
                << "    </Reference>"
                << "\n"
                << "  </ItemGroup>"
                << "\n"
                << "</Project>"
                ;

            csproj.close();

            //////////////////// Log Info ////////////////////

            logger->LogAndPrint(format("C# project generated with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotNetUtils", LogManager::LogLevel::Info);

            //////////////////// Create C# Solution for vs ////////////////////

            const string f_GenSlnCommand = "dotnet new sln -n " + fp_ProjectName;
            const string f_AddProjCommand = "dotnet sln " + fp_ProjectName + ".sln add " + fp_ProjectName + ".csproj";

            int result = -1;

            result = system(f_GenSlnCommand.c_str());

            if (result != 0)
            {
                logger->LogAndPrint("Failed to create C# solution!", "DotNetUtils", LogManager::LogLevel::Error);
                return false;
            }

            result = system(f_AddProjCommand.c_str());

            if (result != 0)
            {
                logger->LogAndPrint(format("Failed to add C# project -> solution named: '{}'", fp_ProjectName), "DotNetUtils", LogManager::LogLevel::Error);
                return false;
            }

            //////////////////// Log Info ////////////////////

            logger->LogAndPrint(format("C# solution generated with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotNetUtils", LogManager::LogLevel::Info);

            //////////////////// Try to build it ////////////////////

            const string f_SolutionPath = fp_ProjectPath + "/" + fp_ProjectName + ".sln";
            const string f_BuildSolutionCommand = "dotnet build \"" + f_SolutionPath + "\"";

            result = system(f_BuildSolutionCommand.c_str());

            if (result != 0)
            {
                logger->LogAndPrint(format("Failed to build C# project! name: '{}', at path: '{}'", fp_ProjectName, f_SolutionPath), "DotNetUtils", LogManager::LogLevel::Error);
                return false;
            }

            return true;
        }

        bool
            GenerateDefaultScript
            (
                const string& fp_ScriptName,
                const string& fp_NodeType,
                const string& fp_ScriptFilePath,
                LogManager* logger
            )
        {
            const string f_FullFilePath = fp_ScriptFilePath + "/" + fp_ScriptName + ".cs";
            ofstream f_ScriptFile(f_FullFilePath);

            if (not f_ScriptFile.is_open()) 
            {
                logger->LogAndPrint(format("Failed to generate default C# script file at: '{}'", f_FullFilePath), "DotNetUtils", LogManager::LogLevel::Error);
                return false;
            }

            f_ScriptFile
                << "using Peach;" << "\n"
                << "using System;" << "\n"
                << "\n"
                << "public partial class " << fp_ScriptName << ":" << fp_NodeType << "\n"
                << "{" << "\n"
                << "    public override void OnEnter()" << "\n"
                << "    {" << "\n"
                << "\n"
                << "    }" << "\n"
                << "    public override void OnUpdate(float delta)" << "\n"
                << "    {" << "\n"
                << "\n"
                << "    }" << "\n"
                << "    public override void OnConstantUpdate()" << "\n"
                << "    {" << "\n"
                << "\n"
                << "    }" << "\n"
                << "    public override void OnExit()" << "\n"
                << "    {" << "\n"
                << "\n"
                << "    }" << "\n"
                << "}";

            f_ScriptFile.close();

            return true;
        }

        private:
            bool
                GetDotNetVersion
                (
                    string* fp_DotNetVersionString,
                    LogManager* logger
                )
            {
                //dont need to do a nullptr check on logger since it's already done in GenerateProjectFiles() where it already performs one UwU
                if (not fp_DotNetVersionString)
                {
                    logger->LogAndPrint("Tried passing a nullptr ref to string inside GetDotNetVersion(), nothing was done.", "DotNetUtils", LogManager::LogLevel::Error);
                    return false;
                }

                #if defined(_WIN32) || defined(_WIN64)

                    FILE* pipe = _popen("dotnet --version", "r");

                    if (not pipe)
                    {
                        logger->LogAndPrint("Failed to run 'dotnet --version' via _popen()", "DotNetUtils", LogManager::LogLevel::Error);
                        return false;
                    }

                    char buffer[128];

                    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
                    {
                        *fp_DotNetVersionString += buffer;
                    }

                    _pclose(pipe);

                #else

                    FILE* pipe = popen("dotnet --version", "r");

                    if (not pipe)
                    {
                        logger->LogAndPrint("Failed to run 'dotnet --version' via popen()", "DotNetUtils", LogManager::LogLevel::Error);
                        return false;
                    }

                    char buffer[128];

                    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
                    {
                        *fp_DotNetVersionString += buffer;
                    }

                    pclose(pipe);

                #endif

                // Trim whitespace / newline
                fp_DotNetVersionString->erase(remove_if(fp_DotNetVersionString->begin(), fp_DotNetVersionString->end(), ::isspace), fp_DotNetVersionString->end());

                // Validate version string (e.g., "6.9.0")
                if (fp_DotNetVersionString->empty())
                {
                    logger->LogAndPrint("Empty .NET SDK version string — is dotnet installed?", "DotNetUtils", LogManager::LogLevel::Error);
                    return false;
                }
                else if (fp_DotNetVersionString->find('.') == string::npos)
                {
                    logger->LogAndPrint("Invalid .NET SDK version string — is dotnet installed?", "DotNetUtils", LogManager::LogLevel::Error);
                    return false;
                }

                logger->LogAndPrint("Detected .NET SDK version: " + *fp_DotNetVersionString, "DotNetUtils", LogManager::LogLevel::Info);

                return true;
            }
    };
}