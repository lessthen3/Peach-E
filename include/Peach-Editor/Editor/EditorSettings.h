#pragma once

#include <string>
#include <vector>
#include <map>

namespace PeachEditor {


	struct EditorSettings
	{

        std::map<std::string, std::string> EditorSettings;
        std::map<std::string, std::string> ProjectSettings;

	};

}