#include "../../include/Parsers/PythonScriptParser.h"

/*
    This class is used to
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace Princess {
    
    namespace py = pybind11;

    void PythonScriptParser::ExtractFunctionInformationFromPythonModule(const std::string& fp_DesiredModuleImport) 
{
        py::scoped_interpreter guard{}; // Start the Python interpreter

        auto sys = py::module_::import("sys");
        sys.attr("path").attr("append")("D:/Game Development/Random Junk I Like to Keep/"); // Add your script directory to sys.path

       // sys.attr("path").attr("append")("D:/Game Development/Peach-E/src/Language-Support/Visual-Peach/");
        //auto test_module = py::module_::import("Test-Function-Read");

        auto inspector_module = py::module_::import("FunctionInspector");
        auto inspector = inspector_module.attr("FunctionInspector")(py::module_::import(fp_DesiredModuleImport.c_str()));
        auto functions_info = inspector.attr("get_functions")();

        // Convert Python dictionary to C++ map or simply iterate in Python context
        for (auto item : functions_info.cast<py::dict>())
        {
            std::string f_TempFirst = std::string(py::str(item.first));
            std::string f_TempSecond = std::string(py::str(item.second));
            std::cout << "Function Name: " << f_TempFirst << "\n";
            std::cout << "Details: " << f_TempSecond << "\n\n";
            m_ListOfPythonFunctionsToAutoPopulate.push_back({ f_TempFirst ,f_TempSecond, fp_DesiredModuleImport });
        }
    }
}
