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
#include "LangUtils.h"

namespace PeachEditor::Dotnet
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
            PeachCore::Logger* logger
        )
    {
        //////////////////// Check for nullptr ////////////////////

        if (not logger)
        {
            PRINT_ERROR("Tried passing a nullptr ref to logger inside GetDotnetVersion() from DotnetUtils.h");
            return false;
        }
        else if (not fp_DotnetVersionString)
        {
            logger->Error("Tried passing a nullptr ref to string inside GetDotnetVersion(), nothing was done.", "DotnetUtils");
            return false;
        }

        #if defined(_WIN32) || defined(_WIN64) //ohhhh _popen SO DIFFERENT SO UNIQUE THANK U BILL FUCK U IDGAF THAT U CAN JUMP OVER A FUCKING CHAIR

            FILE* pipe = _popen("dotnet --version", "r");

            if (not pipe)
            {
                logger->Error("Failed to run 'dotnet --version' via _popen()", "DotnetUtils");
                return false;
            }

            char buffer[128]; //cant imagine version numbers will get bigger than this lmfao

            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                *fp_DotnetVersionString += buffer;
            }

            _pclose(pipe);

        #else //just assuming posix otherwise idk

            FILE* pipe = popen("dotnet --version", "r");

            if (not pipe)
            {
                logger->Error("Failed to run 'dotnet --version' via popen()", "DotnetUtils");
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
            logger->Error("Empty .NET SDK version string — is dotnet installed?", "DotnetUtils");
            return false;
        }
        else if (fp_DotnetVersionString->find('.') == string::npos)
        {
            logger->Error("Invalid .NET SDK version string — is dotnet installed?", "DotnetUtils");
            return false;
        }

        logger->Info("Detected .NET SDK version: " + *fp_DotnetVersionString, "DotnetUtils");

        return true;
    }

    bool
        GenerateProjectFiles
        (
            Configs& fp_DotnetConfigs,
            const string& fp_ProjectName,
            const string& fp_ProjectPath,
            const string& fp_PeachBridgePath,
            const string& fp_RelativeBuildOutputPath,
            PeachCore::Logger* logger
        )
    {
        //////////////////// Check for nullptr ////////////////////

        if (not logger)
        {
            PRINT_ERROR("Tried passing a nullptr ref to logger inside GenerateProjectFiles() from DotnetUtils.h");
            return false;
        }

        //////////////////// Create Directory if it doesn't exist ////////////////////

        if (not filesystem::exists(fp_ProjectPath))
        {
            logger->Error(fmt::format("Failed to locate directory for C# project generation with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotnetUtils");
            return false;
        }

        //////////////////// Fetch dotnet Version from CLI ////////////////////

        string f_DotnetVersion;

        if (not GetDotnetVersion(&f_DotnetVersion, logger))
        {
            logger->Error(fmt::format("Failed to retrieve dotnet version, failed to generate valid C# project with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotnetUtils");
            return false;
        }

        //////////////////// Validate/Enforce .NET version ////////////////////

        try //stoi can throw so y not its 0 cost if it doesnt throw UwU, not like we're calling this more than once per project gen
        {
            int f_DotNetMajorVersion = stoi(f_DotnetVersion.substr(0, f_DotnetVersion.find('.')));

            if (f_DotNetMajorVersion < 6)
            {
                logger->Error(fmt::format(".NET version '{}' found when Peach-E requires .NET SDK version 6.0 or higher", f_DotNetMajorVersion), "DotnetUtils");
                return false;
            }
        }
        catch (const exception& Exception) //catch >O<
        {
            logger->Error(fmt::format("Failed to parse .NET version number >O<, version found: '{}', with error: '{}'", f_DotnetVersion, Exception.what()), "DotnetUtils");
            return false;
        }

        //////////////////// Store Version in Context ////////////////////

        fp_DotnetConfigs.FullVersion = f_DotnetVersion;

        //////////////////// Create .csproj File ////////////////////

        fp_DotnetConfigs.ProjectPath = fp_ProjectPath + "/" + fp_ProjectName + ".csproj";

        ofstream csproj(fp_DotnetConfigs.ProjectPath);

        if (not csproj.is_open())
        {
            logger->Error(fmt::format("Failed to open csproj file for writing: '{}'", fp_DotnetConfigs.ProjectPath), "DotnetUtils");
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

        logger->Info(fmt::format("C# project generated with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotnetUtils");

        //////////////////// Create C# Solution for vs ////////////////////

        fp_DotnetConfigs.SolutionPath = fp_ProjectPath + "/" + fp_ProjectName + ".sln"; //assuming the first gen command actually worked this is what it should be

        const string f_GenSlnCommand = "dotnet new sln --name " + fp_ProjectName + " --output \"" + fp_ProjectPath + "\" --force"; //sln path needs quotes 
        const string f_AddProjCommand = "dotnet sln \"" + fp_DotnetConfigs.SolutionPath + "\" add \"" + fp_DotnetConfigs.ProjectPath + "\"";

        int result = -1;

        result = system(f_GenSlnCommand.c_str());

        if (result != 0)
        {
            logger->Error(fmt::format("Failed to create C# solution! Solution name: '{}', Project path: '{}'", fp_ProjectName, fp_DotnetConfigs.ProjectPath), "DotnetUtils");
            return false;
        }

        result = system(f_AddProjCommand.c_str());

        if (result != 0)
        {
            logger->Error(fmt::format("Failed to add C# project -> Solution name: '{}', Project path: '{}'", fp_ProjectName, fp_DotnetConfigs.ProjectPath), "DotnetUtils");
            return false;
        }

        //////////////////// Log Info ////////////////////

        logger->Info(fmt::format("C# solution generated with name: '{}', and at path: '{}'", fp_ProjectName, fp_ProjectPath), "DotnetUtils");

        //////////////////// Dotnet Testing not Real Production Code ////////////////////

        //Serializer f_Serializer;

        //f_Serializer.ToJSON(pm_DotnetContext.RuntimeConfigs, "PeachGame.runtimeconfig", fp_RootPath + "/local_tests", main_logger.get());

        return true;
    }

    bool
        GenerateDefaultScript
        (
            const string& fp_ScriptName,
            const string& fp_NodeType,
            const string& fp_ScriptFilePath,
            PeachCore::Logger* logger
        )
    {
        const string f_FullFilePath = fp_ScriptFilePath + "/" + fp_ScriptName + ".cs";
        ofstream f_ScriptFile(f_FullFilePath);

        if (not f_ScriptFile.is_open())
        {
            logger->Error(fmt::format("Failed to generate default C# script file at: '{}'", f_FullFilePath), "DotnetUtils");
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
            PeachCore::Logger* logger
        )
    {
        //////////////////// Try to build it ////////////////////

        const string f_BuildSolutionCommand = "dotnet build \"" + fp_SolutionPath + "\"";

        int result = system(f_BuildSolutionCommand.c_str());

        if (result != 0)
        {
            logger->Error(fmt::format("Failed to build C# project! at path: '{}'", fp_SolutionPath), "DotnetUtils");
            return false;
        }

        return true;
    }

    bool
        GetHostFxrLocalPath
        (
            string* fp_HostFxrString,
            PeachCore::Logger* logger
        )
    {
        //////////////////// Check for nullptr ////////////////////

        if (not logger)
        {
            PRINT_ERROR("Tried passing a nullptr ref to logger inside GetHostFxrLocalPath() from LangUtils.cpp");
            return false;
        }
        else if (not fp_HostFxrString)
        {
            logger->Error("Tried to pass nullptr ref to host fxr string inside 'GetHostFxrLocalPath()', exiting function execution immediately", "LangUtils");
            return false;
        }

        //////////////////// Find hostfxr Path ////////////////////

        char_t buffer[MAX_PATH_LENGTH];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);

        const int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);

        if (rc != 0)
        {
            logger->Error("Failed to locate hostfxr", "LangUtils");
            return false;
        }

        const filesystem::path f_HostFxrPath = buffer;

        logger->Info(fmt::format("Successfully found hostexr at path: '{}'", f_HostFxrPath.string()), "LangUtils");

        *fp_HostFxrString = f_HostFxrPath.string();

        return true;
    }
}

namespace PeachEditor::Lua
{
    bool
        GenerateDefaultScript
        (
            const string& fp_ScriptName,
            const string& fp_NodeType,
            const string& fp_ScriptFilePath,
            PeachCore::Logger* logger
        )
    {

        return true;
    }

    ////////////////////////////////////////////// Local helper for lua_dump //////////////////////////////////////////////

    static int
        LuaChunkWriter
        (
            lua_State* /*fp_LuaState*/, 
            const void* fp_Data, 
            size_t fp_Size, 
            void* fp_UserData
        )
    {
        auto& f_DataOut = *static_cast<vector<uint8_t>*>(fp_UserData);
        const auto* f_DataIn = static_cast<const uint8_t*>(fp_Data);

        f_DataOut.insert(f_DataOut.end(), f_DataIn, f_DataIn + fp_Size);

        return 0; // success
    }

    ////////////////////////////////////////////// Lua -> Bytecode //////////////////////////////////////////////

    bool
        CompileProjectScripts 
        (
            const vector<filesystem::path>& fp_ScriptPaths,
            PeachCore::PeachBinChapter& fp_PeachBinChapter,
            PeachCore::Logger* logger
        )
    {
        ////////////////////////////////////////////// Safety checks //////////////////////////////////////////////

        if (not logger)
        {
            PRINT_ERROR("Tried passing a nullptr ref to logger inside Lua::CompileProjectScripts()");
            return false;
        }

        if (fp_ScriptPaths.empty())
        {
            logger->Warning("CompileProjectScripts called with empty script list", "Lua::CompileProjectScripts");
            return false; 
        }

        if (not fp_PeachBinChapter.Contents.empty())
        {
            logger->Warning("Attempted to pass a non-empty PeachBinChapter -> CompileProjectScripts() please only feed an empty default initialized PeachBinChapter i'm lookin at you Ryan >///<", "Lua::CompileProjectScripts");
            return false; 
        }

        ////////////////////////////////////////////// Create Lua state //////////////////////////////////////////////

        lua_State* f_LuaState = luaL_newstate();

        if (not f_LuaState)
        {
            logger->Error("Failed to create Lua state for compilation", "Lua::CompileProjectScripts");
            return false;
        }

        ////////////////////////////////////////////// Open every lib except for OS and IO //////////////////////////////////////////////

        luaL_requiref(f_LuaState, "_G", luaopen_base, 1); lua_pop(f_LuaState, 1);
        luaL_requiref(f_LuaState, "package", luaopen_package, 1); lua_pop(f_LuaState, 1);
        luaL_requiref(f_LuaState, "string", luaopen_string, 1); lua_pop(f_LuaState, 1);
        luaL_requiref(f_LuaState, "table", luaopen_table, 1); lua_pop(f_LuaState, 1);
        luaL_requiref(f_LuaState, "math", luaopen_math, 1); lua_pop(f_LuaState, 1);
        luaL_requiref(f_LuaState, "utf8", luaopen_utf8, 1); lua_pop(f_LuaState, 1);
        luaL_requiref(f_LuaState, "debug", luaopen_debug, 1); lua_pop(f_LuaState, 1);

        ////////////////////////////////////////////// Compile each script //////////////////////////////////////////////

        for (const auto& lv_ScriptPath : fp_ScriptPaths)
        {
            vector<uint8_t> f_Bytecode;

            string f_ScriptName = lv_ScriptPath.filename().string();

            ////////////////////////////////////////////// Safety Check for Name Length //////////////////////////////////////////////

            if (f_ScriptName.size() > numeric_limits<uint16_t>::max())
            {
                logger->Error(fmt::format("Tried to pass a script named: '{}' that is bigger than 65536 characters, full compilation of lua project was unable to proceed!", f_ScriptName), "Lua::CompileProjectScripts");
                return false;
            }

            logger->Info(fmt::format("Compiling Lua script: '{}' -> '{}'", lv_ScriptPath.string(), f_ScriptName), "Lua::CompileProjectScripts");

            ////////////////////////////////////////////// Compile: pushes function (chunk) onto the stack on success //////////////////////////////////////////////

            int f_CompilationStatus = luaL_loadfile(f_LuaState, lv_ScriptPath.string().c_str());

            if (f_CompilationStatus != LUA_OK)
            {
                const char* f_LuaError = lua_tostring(f_LuaState, -1);
                logger->Error(fmt::format("Lua compile error in '{}': {}", lv_ScriptPath.string(), f_LuaError ? f_LuaError : "<unknown>"), "Lua::CompileProjectScripts");

                lua_pop(f_LuaState, 1); // pop error
                lua_close(f_LuaState);

                return false;
            }

            ////////////////////////////////////////////// Dump Lua Bytecode Into f_Bytecode //////////////////////////////////////////////

            int f_DumpStatus = lua_dump
            (
                f_LuaState,
                LuaChunkWriter,
                &f_Bytecode,
                0   // strip debug info? 0 = keep, 1 = strip
            );

            if (f_DumpStatus != 0)
            {
                logger->Error(fmt::format("lua_dump failed for script '{}'", lv_ScriptPath.string()), "Lua::CompileProjectScripts");
                lua_pop(f_LuaState, 1); // pop compiled function
                lua_close(f_LuaState);

                return false;
            }

            ////////////////////////////////////////////// Store Section Inside Chapter //////////////////////////////////////////////

            //fp_PeachBinChapter.Contents.emplace(f_ScriptName, lv_ScriptPath.string(), move(f_Bytecode));

            ////////////////////////////////////////////// Pop compiled chunk from stack //////////////////////////////////////////////

            lua_pop(f_LuaState, 1);
        }

        ////////////////////////////////////////////// Close Lua State and Log //////////////////////////////////////////////

        lua_close(f_LuaState);

        //logger->Info(fmt::format("Successfully compiled {} Lua scripts into {} bytes of bytecode", fp_ScriptPaths.size(), fp_OutputBytecode.size()), "Lua::CompileProjectScripts");

        return true;
    }
}