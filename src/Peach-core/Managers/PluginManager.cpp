// PluginManager.cpp
#include "PluginManager.h"

namespace fs = std::filesystem;

typedef Plugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(Plugin*);

std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

DYNLIB_HANDLE loadLibrary(const std::string& path) 
{
    std::wstring widePath = stringToWString(path);
    return LoadLibrary(widePath.c_str());
}



void PluginManager::LoadPlugin(const std::string& fp_Path) 
{
    DYNLIB_HANDLE handle;

    if (fs::exists(fp_Path) && fs::is_regular_file(fp_Path))
    {
        handle = loadLibrary(fp_Path);
        LogManager::Logger().Debug("Successfully located DLL at: " + fp_Path, "PluginManager");
    }
    
    else
    {
        LogManager::Logger().Error("Failed to locate DLL at: " + fp_Path, "PluginManager");
        return;

    }

    if (!handle)
    {
        LogManager::Logger().Error("Failed to load plugin at path: " + fp_Path, "PluginManager");
        return;
    }
    else
    {
        LogManager::Logger().Debug("Successfully loaded plugin at: " + fp_Path, "PluginManager");
    }

    auto CreateFunc = (CreatePluginFunc)DYNLIB_GETSYM(handle, "createPlugin");
    auto DestroyFunc = (DestroyPluginFunc)DYNLIB_GETSYM(handle, "destroyPlugin");

    if (!CreateFunc || !DestroyFunc)
    {
        LogManager::Logger().Error( "Failed to find CreatePlugin() or DestroyPlugin() functions in: " + fp_Path, "PluginManager");
        DYNLIB_UNLOAD(handle);
        return;
    }
    else
    {
        LogManager::Logger().Debug("Successfully located CreatePlugin() or DestroyPlugin() functions in: " + fp_Path, "PluginManager");
    }

    Plugins.emplace_back(CreateFunc());
    PluginHandles.push_back(handle);
}

void PluginManager::InitializePlugins() 
{
    for (auto& plugin : Plugins) {
        plugin->Initialize();
    }
}

void PluginManager::UpdatePlugins(float TimeSinceLastFrame) 
{
    for (auto& plugin : Plugins) {
        plugin->Update(TimeSinceLastFrame);
    }
}

void PluginManager::ConstantUpdatePlugins(float TimeSinceLastFrame) 
{
    for (auto& plugin : Plugins) {
        plugin->ConstantUpdate(TimeSinceLastFrame);
    }
}

void PluginManager::ShutdownPlugins()
{

    for (auto& plugin : Plugins) {
        plugin->Shutdown();
    }

    for (auto handle : PluginHandles) {
        DYNLIB_UNLOAD(handle);
    }

    Plugins.clear();
    PluginHandles.clear();
}
