// PluginManager.cpp
#include "PluginManager.h"
#include <iostream>
#include <string>

void PluginManager::LoadPlugin(const std::string& path) {
    DYNLIB_HANDLE handle = DYNLIB_LOAD(path.c_str());
    if (!handle) {
        std::cerr << "Failed to load plugin: " << path << std::endl;
        return;
    }

    using CreatePluginFunc = Plugin * (*)();
    auto createPlugin = reinterpret_cast<CreatePluginFunc>(DYNLIB_GETSYM(handle, "createPlugin"));
    if (!createPlugin) {
        std::cerr << "Failed to find createPlugin function in: " << path << std::endl;
        DYNLIB_UNLOAD(handle);
        return;
    }

    plugins.emplace_back(createPlugin());
    pluginHandles.push_back(handle);
}

void PluginManager::InitializePlugins() {
    for (auto& plugin : plugins) {
        plugin->Initialize();
    }
}

void PluginManager::UpdatePlugins(float TimeSinceLastFrame) {
    for (auto& plugin : plugins) {
        plugin->Update(TimeSinceLastFrame);
    }
}

void PluginManager::ConstantUpdatePlugins(float TimeSinceLastFrame) {
    for (auto& plugin : plugins) {
        plugin->ConstantUpdate(TimeSinceLastFrame);
    }
}

void PluginManager::ShutdownPlugins() {

    for (auto& plugin : plugins) {
        plugin->Shutdown();
    }

    for (auto handle : pluginHandles) {
        DYNLIB_UNLOAD(handle);
    }

    plugins.clear();
    pluginHandles.clear();
}
