#include "PythonScriptParser.h"

namespace Princess {
    
    namespace py = pybind11;

    void PythonScriptParser::ExtractFunctionInformationFromPythonModule() {
        py::scoped_interpreter guard{}; // Start the Python interpreter
        auto sys = py::module_::import("sys");
        sys.attr("path").attr("append")("D:/Game Development/Random Junk I Like to Keep/"); // Add your script directory to sys.path
       // sys.attr("path").attr("append")("D:/Game Development/Peach-E/src/Language-Support/Visual-Peach/");

        auto inspector_module = py::module_::import("FunctionInspector");
        //auto test_module = py::module_::import("Test-Function-Read");
        auto inspector = inspector_module.attr("FunctionInspector")(py::module_::import("Test-Function-Read"));
        auto functions_info = inspector.attr("get_functions")();

        // Convert Python dictionary to C++ map or simply iterate in Python context
        for (auto item : functions_info.cast<py::dict>()) {
            std::cout << "Function Name: " << std::string(py::str(item.first)) << "\n";
            std::cout << "Details: " << std::string(py::str(item.second)) << "\n\n";
        }
    }
}
