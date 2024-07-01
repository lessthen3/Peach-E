//#include "../../include/Managers/PythonScriptManager.h"
//
//
//
//namespace PeachCore {
//
//	void PythonScriptManager::InitializePythonBindingsForPeachCore(pybind11::module& fp_Module)
//	{
//        //BindEntityManagerToPythonModule(fp_Module);
//        ////BindRegistryManagerToPythonModule(fp_Module);
//        //BindScheduleManagerToPythonModule(fp_Module);
//	}
//
//    void PythonScriptManager::BindEntityManagerToPythonModule(pybind11::module& fp_Module)
//    {
//        //pybind11::class_<EntityManager>(fp_Module, "EntityManager")
//        //    .def_static("instance", EntityManager::Entity, pybind11::return_value_policy::reference)
//        //    .def("create_entity", EntityManager::CreateEntity)
//        //    .def("destroy_entity", EntityManager::DestroyEntity)
//        //    .def("add_component", EntityManager::AddComponent<YourComponentType>)
//        //    .def("get_component", EntityManager::GetComponent<YourComponentType>);
//    }
//
//    void PythonScriptManager::BindRegistryManagerToPythonModule(pybind11::module& fp_Module)
//    {
//        pybind11::class_<RegistryManager>(fp_Module, "RegistryManager")
//            .def_static("registry", &RegistryManager::Registry)
//            .def("get_registry", &RegistryManager::GetRegistry);
//
//    }
//
//    void PythonScriptManager::BindScheduleManagerToPythonModule(pybind11::module& fp_Module)
//    {
//        //pybind11::class_<ScheduleManager>(fp_Module, "ScheduleManager")
//        //    .def_static("instance", &ScheduleManager::Schedule)
//        //    .def("add_system", &ScheduleManager::AddSystem)
//        //    .def("update_systems", &ScheduleManager::UpdateSystems);
//    }
//
//    //void PythonScriptManager::BindEventQueueManagerToPythonModule(pybind11::module& fp_Module)
//    //{
//    //    pybind11::class_<EventQueueManager>(fp_Module, "EventQueueManager")
//    //        .def_static("EventQueue", &EventQueueManager::EventQueue, py::return_value_policy::reference)
//    //        .def("PostEvent", &EventQueueManager::postEvent<ExplosionEvent>)  // Template instantiation for specific events
//    //        .def("Subscribe", &EventQueueManager::subscribe<ExplosionEvent>)
//    //        .def("ProcessEvents", &EventQueueManager::processEvents);
//    //}
//}