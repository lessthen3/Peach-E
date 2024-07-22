#pragma once

#include <string>
#include <map>

using namespace std;

namespace PeachEngine {

	struct ExportConfigSetting
	{
		ExportConfigSetting() = default;

	};

	class ProjectExportManager
	{
	public:
		ProjectExportManager() = default;
		~ProjectExportManager();


	public:

		//utility function for plugins to adjust export configs if additional files are needed to. returns true if change was accepted by the editor, returns false if the change was unsuccessful
		bool ChangeExportConfig()
		{


			return true;
		}

		map<string, ExportConfigSetting>* GetAllCurrentExportConfigSettings()
		{
			//return a nullptr if config settings weren't successfully retrieved
			if (false)
			{
				//Log something here on the mainlogger, since exporting will be done on the main thread
				return nullptr;
			}

			return &pm_CurrentExportConfigSettings;
		}
		


	private:
		map<string, ExportConfigSetting> pm_CurrentExportConfigSettings;


	}


}