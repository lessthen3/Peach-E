// PluginManager.cpp
#include "PluginManager.h"

namespace PeachCore {

    namespace fs = std::filesystem;

    void PluginManager::LoadPlugin(const std::string& fp_Path)
    {
        DYNLIB_HANDLE f_Handle;

        if (fs::exists(fp_Path) && fs::is_regular_file(fp_Path))
        {
            f_Handle = DYNLIB_LOAD(fp_Path.c_str());
            LogManager::Logger().Debug("Successfully located DLL at: " + fp_Path, "PluginManager");
        }

        else
        {
            LogManager::Logger().Error("Failed to locate DLL at: " + fp_Path, "PluginManager");
            return;
        }

        if (!f_Handle)
        {
            LogManager::Logger().Error("Failed to load plugin at path: " + fp_Path, "PluginManager");
            return;
        }
        else
        {
            LogManager::Logger().Debug("Successfully loaded plugin at: " + fp_Path, "PluginManager");
        }

        auto f_CreateFunc = (CreatePluginFunc)DYNLIB_GETSYM(f_Handle, "createPlugin");
        auto f_DestroyFunc = (DestroyPluginFunc)DYNLIB_GETSYM(f_Handle, "destroyPlugin");

        if (!f_CreateFunc || !f_DestroyFunc)
        {
            LogManager::Logger().Error("Failed to find CreatePlugin() or DestroyPlugin() functions in: " + fp_Path, "PluginManager");
            DYNLIB_UNLOAD(f_Handle);
            return;
        }
        else
        {
            LogManager::Logger().Debug("Successfully located CreatePlugin() or DestroyPlugin() functions in: " + fp_Path, "PluginManager");
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

        for (auto& plugin : pm_PluginInstances) {
            plugin->Shutdown();
        }

        pm_PluginInstances.clear();
        pm_PluginHandles.clear();

        for (auto f_Handle : pm_PluginHandles) {
            if (f_Handle != nullptr) {
                DYNLIB_UNLOAD(f_Handle);
            }
        }
    }
}