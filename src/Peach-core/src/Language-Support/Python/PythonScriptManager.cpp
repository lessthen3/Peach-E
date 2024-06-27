#include "../../../include/Language-Support/Python/PythonScriptManager.h"


//namespace py = pybind11;

namespace PeachCore {

    void PythonScriptManager::RunPythonScript(const std::string& script) {
        /*py:: guard{};
        py::object scope = py::module_::import("__main__").attr("__dict__");
        py::exec_file(script, scope);

        if (scope.contains("initialize")) {
            scope["initialize"]();
        }

        if (scope.contains("update")) {
            scope["update"]();
        }*/
    }
}