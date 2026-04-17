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

#include <Utils/SerializerMacros.h>

#include <string>
#include <unordered_map>
#include <stdint.h> //using the c version across the entire project owo don't need the std wrap tbh

namespace PeachEditor{

    using namespace std;

    struct PeachProjectSettings //per project settings/configs
    {
        unordered_map<string, string> ProjectSettings;

        uint32_t FontSize;
        string CurrentFont;

        //theme stuff

        //SERIALIZABLE_FIELDS(FontSize, CurrentFont)
    };

    struct ProjectLocalEditorSettings
    {

        //SERIALIZABLE_FIELDS()
    };

    struct ExportConfigSettings
    {
        string GameName;
        string ExportDirectory;

        bool IsDebug = false;

        //SERIALIZABLE_FIELDS()
    };

    struct PeachProject
    {
        string ProjectName;
        uint8_t RuntimeType = 0;

        unordered_map<string, string> ScriptPaths;
        unordered_map<string, string> ScenePaths; // Key : ObjectID, Val : path -> resource
        unordered_map<string, string> TexturePaths;
        unordered_map<string, string> AudioPaths;
        unordered_map<string, string> ModelPaths;
        unordered_map<string, string> ShaderPaths;

        PeachProjectSettings ProjectSettings;
        ExportConfigSettings ExportSettings;

        bool
            Export
            (

            )
        {

            return true;
        }

        PEACH_SERIALIZABLE
        (
            PEACH_FIELD(ProjectName) //, RuntimeType, ScriptPaths, ScenePaths, TexturePaths, AudioPaths, ProjectSettings
        )
    };

}