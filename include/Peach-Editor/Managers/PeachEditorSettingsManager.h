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


	struct SettingsManager
	{

    public:
        static SettingsManager& Settings() {
            static SettingsManager instance;
            return instance;
        }

    public:
        std::map<std::string, std::string> EditorSettings;
        std::map<std::string, std::string> ProjectSettings;

        void Initialize(const std::string& logDirectory);

    private:
        SettingsManager() = default;
        ~SettingsManager() = default;

        SettingsManager(const SettingsManager&) = delete;
        SettingsManager& operator=(const SettingsManager&) = delete;


	private:


	};

}
