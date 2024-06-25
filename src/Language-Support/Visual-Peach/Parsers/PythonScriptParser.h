#pragma once

#include "../../PythonScriptComponent.h"
#include <map>

#include <pybind11/pybind11.h>
#include <pybind11/embed.h> // Use pybind11 to embed Python
#include <iostream>

#pragma once

#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace VisualPeach {

	class PythonScriptParser//PeachCore::PythonScriptComponent fp_Script
	{
	public:
		static PythonScriptParser& Parser() {
			static PythonScriptParser instance;
			return instance;

		};

	public:
		void ExtractFunctionInformationFromPythonModule();

	private:
		PythonScriptParser() = default;
		~PythonScriptParser() = default;

		PythonScriptParser(const PythonScriptParser&) = delete;
		PythonScriptParser& operator=(const PythonScriptParser&) = delete;

	};
}
