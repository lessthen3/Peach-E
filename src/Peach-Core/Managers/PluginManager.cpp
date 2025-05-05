/*******************************************************************
 *                                             Peach-E v0.1
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#include "../../include/Peach-Core/Managers/PluginManager.h"

namespace PeachCore {

    namespace fs = std::filesystem;

    bool
        PluginManager::Initialize //WARNING: i need to re implement plugin manager into a separate DLL/dylib/so loader, and also rework the logger since i want to enforce one logger per thread design
        (
            const string& fp_LogOutputDirectory,
            shared_ptr<Console> fp_Console
        )
    {
        plugin_logger = make_unique<LogManager>();
        plugin_logger->Initialize(ThreadName::MainThread, fp_LogOutputDirectory, "PluginManager", fp_Console, LogManager::LogLevel::All);
        plugin_logger->LogAndPrint("PluginLogger successfully initialized", "PluginManager", LogManager::LogLevel::Debug);

        return true;
    }

    void 
        PluginManager::LoadPlugin(const string& fp_Path)
    {
        DYNLIB_HANDLE f_Handle;

        if (fs::exists(fp_Path) && fs::is_regular_file(fp_Path))
        {
            f_Handle = DYNLIB_LOAD(fp_Path.c_str());
            plugin_logger->LogAndPrint("Successfully located DLL at: " + fp_Path, "PluginManager", LogManager::LogLevel::Debug);
        }

        else
        {
            plugin_logger->LogAndPrint("Failed to locate DLL at: " + fp_Path, "PluginManager", LogManager::LogLevel::Error);
            return;
        }

        if (not f_Handle)
        {
            plugin_logger->LogAndPrint("Failed to load plugin at path: " + fp_Path, "PluginManager", LogManager::LogLevel::Error);
            return;
        }
        else
        {
            plugin_logger->LogAndPrint("Successfully loaded plugin at: " + fp_Path, "PluginManager", LogManager::LogLevel::Debug);
        }

        auto f_CreateFunc = (CreatePluginFunc)DYNLIB_GETSYM(f_Handle, "createPlugin");
        auto f_DestroyFunc = (DestroyPluginFunc)DYNLIB_GETSYM(f_Handle, "destroyPlugin");

        if (!f_CreateFunc || !f_DestroyFunc)
        {
            plugin_logger->LogAndPrint("Failed to find CreatePlugin() or DestroyPlugin() functions in: " + fp_Path, "PluginManager", LogManager::LogLevel::Error);
            DYNLIB_UNLOAD(f_Handle);
            return;
        }
        else
        {
            plugin_logger->LogAndPrint("Successfully located CreatePlugin() or DestroyPlugin() functions in: " + fp_Path, "PluginManager", LogManager::LogLevel::Debug);
        }

        std::unique_ptr<Plugin, DestroyPluginFunc> plugin(f_CreateFunc(), f_DestroyFunc); //creates smrt poiner with destructor tied to it;
        pm_PluginInstances.emplace_back(std::move(plugin));

        pm_PluginHandles.push_back(f_Handle);
    }

    void PluginManager::InitializePlugins()
    {
        for (auto& plugin : pm_PluginInstances) {
            plugin->Initialize();
        }
    }

    void PluginManager::UpdatePlugins(float fp_TimeSinceLastFrame)
    {
        for (auto& plugin : pm_PluginInstances) {
            plugin->Update(fp_TimeSinceLastFrame);
        }
    }

    void PluginManager::ConstantUpdatePlugins(float fp_TimeSinceLastFrame)
    {
        for (auto& plugin : pm_PluginInstances) {
            plugin->ConstantUpdate(fp_TimeSinceLastFrame);
        }
    }

    void PluginManager::ShutdownPlugins()
    {

        for (auto& plugin : pm_PluginInstances) //call shutdown methods defined in external plugins
        {
            plugin->Shutdown(); //plugin devs better cleanup after themselves, nothing I can do to ensure safety here uwu
        }

        pm_PluginInstances.clear();

        for (auto f_Handle : pm_PluginHandles)
        {
            if (f_Handle != nullptr)
            {
                DYNLIB_UNLOAD(f_Handle);
            }
        }

        pm_PluginHandles.clear(); //wait why am i clearing plugin handles before unloading them LMFAO, XXX: fixed it uwu ><
    }
}

















//old license uwu

//Copyright(c) 2024-present Ranyodh Singh Mandur.
/*
This class is used to manage plugins that the user would like to load objects safely

Note: could expose the functionality of this class to python so that users could repurpose it into a mod loader for games made on Peach-E

*/