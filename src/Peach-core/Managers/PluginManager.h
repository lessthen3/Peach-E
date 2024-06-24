// PluginManager.h
#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Plugin.h"

#include <filesystem>
#include <iostream>
#include <string>
#include "LogManager.h"


#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define DYNLIB_HANDLE HINSTANCE
    #define DYNLIB_LOAD LoadLibrary
    #define DYNLIB_GETSYM GetProcAddress
    #define DYNLIB_UNLOAD FreeLibrary
#else
    #include <dlfcn.h>
    #define DYNLIB_HANDLE void*
    #define DYNLIB_LOAD(path) dlopen(path, RTLD_LAZY)
    #define DYNLIB_GETSYM dlsym
    #define DYNLIB_UNLOAD dlclose
#endif

class PluginManager {
public:
    static PluginManager& ManagePlugins() {
        static PluginManager instance;
        return instance;
    }

    void LoadPlugin(const std::string& path);
    void InitializePlugins();
    void UpdatePlugins(float TimeSinceLastFrame);
    void ConstantUpdatePlugins(float TimeSinceLastFrame);
    void ShutdownPlugins();

private:
    PluginManager() = default;
    ~PluginManager() = default;
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    std::vector<std::unique_ptr<Plugin>> Plugins;
    std::vector<DYNLIB_HANDLE> PluginHandles;
};

