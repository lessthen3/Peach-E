#pragma once

/*
	This class is used to
	Copyright(c) 2024-present Ranyodh Singh Mandur.
*/

//#include "../../Python/PythonScriptComponent.h"
#include <map>

#include <pybind11/pybind11.h>
#include <pybind11/embed.h> // Use pybind11 to embed Python
#include <iostream>
#include <vector>

namespace Princess {

	struct PythonFunctionInfo {
		std::string Name;
		std::string Info;
		std::string Module;
	};

	class PythonScriptParser//PeachCore::PythonScriptComponent fp_Script
	{
	public:
		static PythonScriptParser& Parser() {
			static PythonScriptParser instance;
			return instance;

		};

	public:
		void ExtractFunctionInformationFromPythonModule(const std::string& fp_DesiredModuleImport);

		void ExecutePythonScript(std::string fp_PythonScript);

		std::vector<PythonFunctionInfo> m_ListOfPythonFunctionsToAutoPopulate = {};

	private:
		PythonScriptParser() = default;
		~PythonScriptParser() = default;

		PythonScriptParser(const PythonScriptParser&) = delete;
		PythonScriptParser& operator=(const PythonScriptParser&) = delete;

	};
}
