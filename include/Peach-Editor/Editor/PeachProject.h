#pragma once

#include <General/Serializer.h>

using namespace std;

namespace PeachEditor{

    struct PeachProject
    {
        string m_ProjectName;
        string m_ScriptRootPath;

        map<string, string> m_ScenePaths; // Key : ObjectID, Val : path -> resource
        map<string, string> m_TexturePaths;
        map<string, string> m_AudioPaths;

        SERIALIZABLE_FIELDS(m_ProjectName, m_ScriptRootPath, m_ScenePaths, m_TexturePaths, m_AudioPaths)
    };

}