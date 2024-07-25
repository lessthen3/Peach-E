#pragma once

#include <string>
#include <vector>
#include <map>

//THIS IS WHERE WE HANDLE (de)SERIALIZATION OF JSON DATA

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
