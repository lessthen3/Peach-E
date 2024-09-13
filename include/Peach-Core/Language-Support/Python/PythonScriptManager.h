//#pragma once
//
//#include <pybind11/pybind11.h>
//#include "../Managers.h"
//
//namespace PeachCore {
//
//    struct PythonScriptManager
//    {
//    public:
//        static PythonScriptManager& Python() {
//            static PythonScriptManager instance;
//            return instance;
//        }
//
//    public:
//        void InitializePythonBindingsForPeachCore(pybind11::module& m);
//
//
//    private:
//        PythonScriptManager() = default;
//        ~PythonScriptManager() = default;
//
//        PythonScriptManager(const PythonScriptManager&) = delete;
//        PythonScriptManager& operator=(const PythonScriptManager&) = delete;
//
//    private:
//        void BindEntityManagerToPythonModule(pybind11::module& fp_Module);
//        void BindRegistryManagerToPythonModule(pybind11::module& fp_Module);
//        void BindScheduleManagerToPythonModule(pybind11::module& fp_Module);
//
//        void BindEventQueueManagerToPythonModule(pybind11::module& fp_Module);
//
//    };
//}
//
