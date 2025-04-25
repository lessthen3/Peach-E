/*******************************************************************
 *                                        Peach Editor v0.0.7
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                 Peach Editor is an open source editor for Peach-E
********************************************************************/
#pragma once

#include <string>
#include <vector>
#include <map>

namespace PeachEditor {

	struct ProjectManager
	{

    public:
        

    public:
        std::map<std::string, std::string> EditorSettings;
        std::map<std::string, std::string> ProjectSettings;

        void Initialize(const std::string& logDirectory);

    private:
        ProjectManager() = default;
        ~ProjectManager() = default;

        //prevent accidental copying
        ProjectManager(const ProjectManager&) = delete;
        ProjectManager& operator=(const ProjectManager&) = delete;


	private:


	};

}
