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
#include "../../../include/Peach-Editor/Editor/LangUtils.h"

namespace PeachEditor::DotnetUtils
{
    bool
        InitializeRuntime()
    {

        return true;
    }

    bool
        AssertDotnetExists()
    {

        return true;
    }

    bool
        GetDotnetVersion
        (
            string* fp_DotnetVersionString,
            PeachCore::LogManager* logger
        )
    {
        //////////////////// Check for nullptr ////////////////////

        if (not logger)
        {
            PeachCore::PrintError("Tried passing a nullptr ref to logger inside GetDotnetVersion() from DotnetUtils.h");
            return false;
        }
        else if (not fp_DotnetVersionString)
        {
            logger->PEACH_LOG("Tried passing a nullptr ref to string inside GetDotnetVersion(), nothing was done.", "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        #if defined(_WIN32) || defined(_WIN64)

            FILE* pipe = _popen("dotnet --version", "r");

            if (not pipe)
            {
                logger->PEACH_LOG("Failed to run 'dotnet --version' via _popen()", "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
                return false;
            }

            char buffer[128]; //cant imagine version numbers will get bigger than this lmfao

            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                *fp_DotnetVersionString += buffer;
            }

            _pclose(pipe);

        #else

            FILE* pipe = popen("dotnet --version", "r");

            if (not pipe)
            {
                logger->PEACH_LOG("Failed to run 'dotnet --version' via popen()", "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
                return false;
            }

            char buffer[128];

            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                *fp_DotnetVersionString += buffer;
            }

            pclose(pipe);

        #endif

        // Trim whitespace / newline
            fp_DotnetVersionString->erase(remove_if(fp_DotnetVersionString->begin(), fp_DotnetVersionString->end(), ::isspace), fp_DotnetVersionString->end());

        // Validate version string (e.g., "6.9.0")
        if (fp_DotnetVersionString->empty())
        {
            logger->PEACH_LOG("Empty .NET SDK version string — is dotnet installed?", "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }
        else if (fp_DotnetVersionString->find('.') == string::npos)
        {
            logger->PEACH_LOG("Invalid .NET SDK version string — is dotnet installed?", "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        logger->PEACH_LOG("Detected .NET SDK version: " + *fp_DotnetVersionString, "DotnetUtils", PeachCore::LogManager::LogLevel::Info);

        return true;
    }

    bool
        GenerateProjectFiles
        (
            DotnetConfigs& fp_DotnetConfigs,
            const string& fp_ProjectName,
            const string& fp_ProjectPath,
            const string& fp_PeachBridgePath,
            const string& fp_RelativeBuildOutputPath,
            PeachCore::LogManager* logger
        )
    {
        //////////////////// Check for nullptr ////////////////////

        if (not logger)
        {
            PeachCore::PrintError("Tried passing a nullptr ref to logger inside GenerateProjectFiles() from DotnetUtils.h");
            return false;
        }

        //////////////////// Create Directory if it doesn't exist ////////////////////

        if (not filesystem::exists(fp_ProjectPath))
        {
            logger->PEACH_LOG(format("Failed to locate directory for C# project generation with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        //////////////////// Fetch dotnet Version from CLI ////////////////////

        string f_DotnetVersion;

        if (not GetDotnetVersion(&f_DotnetVersion, logger))
        {
            logger->PEACH_LOG(format("Failed to retrieve dotnet version, failed to generate valid C# project with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        //////////////////// Validate/Enforce .NET version ////////////////////

        try //stoi can throw so y not its 0 cost if it doesnt throw UwU, not like we're calling this more than once per project gen
        {
            int f_DotNetMajorVersion = stoi(f_DotnetVersion.substr(0, f_DotnetVersion.find('.')));

            if (f_DotNetMajorVersion < 6)
            {
                logger->PEACH_LOG(format(".NET version '{}' found when Peach-E requires .NET SDK version 6.0 or higher", f_DotNetMajorVersion), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
                return false;
            }
        }
        catch (const exception& Exception) //catch >O<
        {
            logger->PEACH_LOG(format("Failed to parse .NET version number >O<, version found: '{}', with error: '{}'", f_DotnetVersion, Exception.what()), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        //////////////////// Store Version in Context ////////////////////

        fp_DotnetConfigs.FullVersion = f_DotnetVersion;

        //////////////////// Create .csproj File ////////////////////

        fp_DotnetConfigs.ProjectPath = fp_ProjectPath + "/" + fp_ProjectName + ".csproj";

        ofstream csproj(fp_DotnetConfigs.ProjectPath);

        if (not csproj.is_open())
        {
            logger->PEACH_LOG(format("Failed to open csproj file for writing: '{}'", fp_DotnetConfigs.ProjectPath), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        //XXX: should return x.y version and all versions of dotnet do this at the very least, if not for whatever reason it will not throw an outta bounds thing and will just copy the string char for char
        fp_DotnetConfigs.MajorVersion = f_DotnetVersion.substr(0, f_DotnetVersion.find('.') + 2);
        fp_DotnetConfigs.OutpathPath = fp_ProjectPath + "/" + fp_RelativeBuildOutputPath;

        csproj
            << "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            << "\n"
            << "<Project Sdk=\"Microsoft.NET.Sdk\">"
            << "\n"
            << "  <PropertyGroup>"
            << "\n"
            << "    <TargetFramework>" << "net" << fp_DotnetConfigs.MajorVersion << "</TargetFramework>"
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
            << "    <OutputPath>" << fp_DotnetConfigs.OutpathPath << "</OutputPath>"
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

        logger->PEACH_LOG(format("C# project generated with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotnetUtils", PeachCore::LogManager::LogLevel::Info);

        //////////////////// Create C# Solution for vs ////////////////////

        fp_DotnetConfigs.SolutionPath = fp_ProjectPath + "/" + fp_ProjectName + ".sln"; //assuming the first gen command actually worked this is what it should be

        const string f_GenSlnCommand = "dotnet new sln --name " + fp_ProjectName + " --output \"" + fp_ProjectPath + "\" --force"; //sln path needs quotes 
        const string f_AddProjCommand = "dotnet sln \"" + fp_DotnetConfigs.SolutionPath + "\" add \"" + fp_DotnetConfigs.ProjectPath + "\"";

        int result = -1;

        result = system(f_GenSlnCommand.c_str());

        if (result != 0)
        {
            logger->PEACH_LOG(format("Failed to create C# solution! Solution name: '{}', Project path: '{}'", fp_ProjectName, fp_DotnetConfigs.ProjectPath), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        result = system(f_AddProjCommand.c_str());

        if (result != 0)
        {
            logger->PEACH_LOG(format("Failed to add C# project -> Solution name: '{}', Project path: '{}'", fp_ProjectName, fp_DotnetConfigs.ProjectPath), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        //////////////////// Log Info ////////////////////

        logger->PEACH_LOG(format("C# solution generated with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotnetUtils", PeachCore::LogManager::LogLevel::Info);

        return true;
    }

    bool
        GenerateDefaultScript
        (
            const string& fp_ScriptName,
            const string& fp_NodeType,
            const string& fp_ScriptFilePath,
            PeachCore::LogManager* logger
        )
    {
        const string f_FullFilePath = fp_ScriptFilePath + "/" + fp_ScriptName + ".cs";
        ofstream f_ScriptFile(f_FullFilePath);

        if (not f_ScriptFile.is_open())
        {
            logger->PEACH_LOG(format("Failed to generate default C# script file at: '{}'", f_FullFilePath), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
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
            << "    public override void OnUpdate(double delta)" << "\n"
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

    bool
        BuildDotnetProject //XXX: need to add support for if the user deleted the sln file, or if the game project is pulled off git since they shouldnt include build artifacts within the repo
        (
            const string& fp_SolutionPath,
            PeachCore::LogManager* logger
        )
    {
        //////////////////// Try to build it ////////////////////

        const string f_BuildSolutionCommand = "dotnet build \"" + fp_SolutionPath + "\"";

        int result = system(f_BuildSolutionCommand.c_str());

        if (result != 0)
        {
            logger->PEACH_LOG(format("Failed to build C# project! at path: '{}'", fp_SolutionPath), "DotnetUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        return true;
    }

    bool
        GetHostFxrLocalPath
        (
            string* fp_HostFxrString,
            PeachCore::LogManager* logger
        )
    {
        //////////////////// Check for nullptr ////////////////////

        if (not logger)
        {
            PeachCore::PrintError("Tried passing a nullptr ref to logger inside GetHostFxrLocalPath() from LangUtils.cpp");
            return false;
        }
        else if (not fp_HostFxrString)
        {
            logger->PEACH_LOG("Tried to pass nullptr ref to host fxr string inside 'GetHostFxrLocalPath()', exiting function execution immediately", "LangUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        //////////////////// Find hostfxr Path ////////////////////

        char_t buffer[MAX_PATH_LENGTH];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);

        const int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);

        if (rc != 0)
        {
            logger->PEACH_LOG("Failed to locate hostfxr", "LangUtils", PeachCore::LogManager::LogLevel::Error);
            return false;
        }

        const filesystem::path f_HostFxrPath = buffer;

        logger->PEACH_LOG(format("Successfully found hostexr at path: '{}'", f_HostFxrPath.string()), "LangUtils", PeachCore::LogManager::LogLevel::Info);

        *fp_HostFxrString = f_HostFxrPath.string();

        return true;
    }
}