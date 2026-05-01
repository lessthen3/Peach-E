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
#pragma once

///PeachCore
#include <managers/ResourceManager.h>
#include <language_support/dotnet/DotnetRuntime.h>

// Lua runtime for *editor tools* (compiling scripts)
extern "C"
{
    #include <lua/lua.h>
    #include <lua/lauxlib.h>
    #include <lua/lualib.h>
}

///Ew Microsoft (windows 11 isn't that bad tbh ngl frfr ogog ifykyk)

#ifdef PEACH_PLATFORM_FREEBSD
    #include <dotnet9/nethost.h>
#else
    #include <dotnet10/nethost.h>
#endif

namespace PeachEditor {
    using namespace std;
}

namespace PeachEditor::Dotnet{

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

                //SERIALIZABLE_FIELDS(name, version)
            } framework; //needs to be named all lowercase since serializer.h will insert the var name into the json

            //SERIALIZABLE_FIELDS(tfm, rollForward, framework)
        } runtimeOptions;

        //SERIALIZABLE_FIELDS(runtimeOptions)
    };

    struct Configs
    {
        RuntimeConfig RuntimeConfigs;

        string FullVersion;
        string MajorVersion;

        string SolutionPath;
        string ProjectPath;
        string OutpathPath;
    };

    constexpr int MAX_PATH_LENGTH = 1024;

    bool
        InitializeRuntime();

    bool
        AssertDotnetExists();

    bool
        GetDotnetVersion
        (
            string* fp_DotNetVersionString,
            PeachCore::Logger* logger
        );

    bool
        GenerateProjectFiles
        (
            Configs& DotnetConfigs,
            const string& fp_ProjectName,
            const string& fp_ProjectPath,
            const string& fp_PeachBridgePath,
            const string& fp_RelativeBuildOutputPath,
            PeachCore::Logger* logger
        );

    bool
        GenerateDefaultScript
        (
            const string& fp_ScriptName,
            const string& fp_NodeType,
            const string& fp_ScriptFilePath,
            PeachCore::Logger* logger
        );

    bool
        BuildDotnetProject
        (
            const string& fp_SolutionPath,
            PeachCore::Logger* logger
        );

    bool
        GetHostFxrLocalPath(string* fp_HostFxrString, PeachCore::Logger* logger);

}//namespace PeachEditor

namespace PeachEditor::Lua {

    bool
        GenerateDefaultScript
        (
            const string& fp_ScriptName,
            const string& fp_NodeType,
            const string& fp_ScriptFilePath,
            PeachCore::Logger* logger
        );

    bool
        CompileProjectScripts //XXX: need to add support for if the user deleted the sln file, or if the game project is pulled off git since they shouldnt include build artifacts within the repo
        (
            const vector<filesystem::path>& fp_ScriptPaths,
            PeachCore::PeachBinChapter& fp_PeachBinChapter,
            PeachCore::Logger* logger
        );

}