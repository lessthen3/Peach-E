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
#pragma once

///PeachCore
#include <Utils/Serializer.h>
#include <Language-Support/DotnetRuntime.h>

///Ew Microsoft
#include <dotnet/nethost.h>

namespace PeachEditor{

    using namespace std;

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

    struct DotnetConfigs
    {
        RuntimeConfig RuntimeConfigs;

        string FullVersion;
        string MajorVersion;

        string SolutionPath;
        string ProjectPath;
        string OutpathPath;
    };

namespace DotnetUtils{

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
            DotnetConfigs& DotnetConfigs,
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

}// namespace LangUtils
}//namespace PeachEditor