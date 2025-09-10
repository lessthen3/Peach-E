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

#include <Utils/Serializer.h>

namespace PeachEditor{

    using namespace std;

    enum ScriptRuntimeType : uint8_t //flag since a project can have multiple runtimes in it for whatever reason
    {
        Dotnet  = 1 << 0,
        Python = 1 << 1,
        Lua = 1 << 2,
        None = 0
    };

    struct PeachProjectSettings //per project settings/configs
    {
        map<string, string> ProjectSettings; 

        uint32_t FontSize;
        string CurrentFont;

        //theme stuff

        SERIALIZABLE_FIELDS(FontSize, CurrentFont)
    };

    struct PeachProject
    {
        friend class Serializer;

        string ProjectName;
        uint8_t RuntimeType = None;

        map<string, string> ScriptPaths;
        map<string, string> ScenePaths; // Key : ObjectID, Val : path -> resource
        map<string, string> TexturePaths;
        map<string, string> AudioPaths;
        map<string, string> ModelPaths;
        map<string, string> ShaderPaths;

        PeachProjectSettings ProjectSettings;

    private:
        SERIALIZABLE_FIELDS(ProjectName, RuntimeType, ScriptPaths, ScenePaths, TexturePaths, AudioPaths, ProjectSettings)

    };

}