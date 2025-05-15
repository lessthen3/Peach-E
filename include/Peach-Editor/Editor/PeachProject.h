#pragma once

#include <Utils/Serializer.h>

namespace PeachEditor{

    enum ScriptRuntimeType : uint8_t
    {
        Dotnet,
        Python,
        Lua,
        None
    };

    struct PeachProject
    {
        string ProjectName;
        uint8_t RuntimeType = None;

        map<string, string> ScriptPaths;
        map<string, string> ScenePaths; // Key : ObjectID, Val : path -> resource
        map<string, string> TexturePaths;
        map<string, string> AudioPaths;
        map<string, string> ModelPaths;
        map<string, string> ShaderPaths;

        SERIALIZABLE_FIELDS(ProjectName, ScriptPaths, ScenePaths, TexturePaths, AudioPaths)
    };

}