#pragma once

#include <string>
#include <vector>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

using namespace std;

namespace PeachEditor{

    struct PeachProject
    {
        string m_ProjectName;
        string m_StartupScene;
        vector<string> m_ScenePaths;
        vector<string> m_TexturePaths;
        vector<string> m_AudioPaths;
        string m_ScriptRootPath;

        template <class Archive>
        void Serialize(Archive& fp_Archive)
        {
            fp_Archive(m_ProjectName, m_StartupScene, m_ScenePaths, m_TexturePaths, m_AudioPaths, m_ScriptRootPath);
        }

    };

}