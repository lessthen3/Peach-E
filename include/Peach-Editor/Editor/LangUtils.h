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
#pragma once

#include "Utils/Serializer.h"
#include "Language-Support/DotnetRuntime.h"

namespace PeachEditor{

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

    bool
        InitializeRuntime();

    bool
        GetDotnetVersion
        (
            string* fp_DotNetVersionString,
            PeachCore::LogManager* logger
        );

    bool
        GenerateProjectFiles
        (
            DotnetConfigs& DotnetConfigs,
            const string& fp_ProjectName,
            const string& fp_ProjectPath,
            const string& fp_PeachBridgePath,
            const string& fp_RelativeBuildOutputPath,
            PeachCore::LogManager* logger
        );

    bool
        GenerateDefaultScript
        (
            const string& fp_ScriptName,
            const string& fp_NodeType,
            const string& fp_ScriptFilePath,
            PeachCore::LogManager* logger
        );

    bool
        BuildDotnetProject
        (
            const string& fp_SolutionPath,
            PeachCore::LogManager* logger
        );

}// namespace LangUtils
}//namespace PeachEditor