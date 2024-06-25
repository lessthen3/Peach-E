// PluginManager.cpp
#include "PluginManager.h"

namespace PeachCore {

    namespace fs = std::filesystem;

    typedef Plugin* (*CreatePluginFunc)();
    typedef void (*DestroyPluginFunc)(Plugin*);


    std::wstring StringToWString(const std::string& fp_String) {
        int f_SizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &fp_String[0], (int)fp_String.size(), NULL, 0);
        std::wstring f_WstrTo(f_SizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &fp_String[0], (int)fp_String.size(), &f_WstrTo[0], f_SizeNeeded);
        return f_WstrTo;
    }

    DYNLIB_HANDLE loadLibrary(const std::string& fp_Path)
    {
        std::wstring f_WidePath = StringToWString(fp_Path);
        return LoadLibrary(f_WidePath.c_str());
    }



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

        Plugins.emplace_back(f_CreateFunc());
        PluginHandles.push_back(f_Handle);
    }

    void PluginManager::InitializePlugins()
    {
        for (auto& plugin : Plugins) {
            plugin->Initialize();
        }
    }

    void PluginManager::UpdatePlugins(float fp_TimeSinceLastFrame)
    {
        for (auto& plugin : Plugins) {
            plugin->Update(fp_TimeSinceLastFrame);
        }
    }

    void PluginManager::ConstantUpdatePlugins(float fp_TimeSinceLastFrame)
    {
        for (auto& plugin : Plugins) {
            plugin->ConstantUpdate(fp_TimeSinceLastFrame);
        }
    }

    void PluginManager::ShutdownPlugins()
    {

        for (auto& plugin : Plugins) {
            plugin->Shutdown();
        }

        for (auto f_Handle : PluginHandles) {
            DYNLIB_UNLOAD(f_Handle);
        }

        Plugins.clear();
        PluginHandles.clear();
    }
}