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
#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../General/Plugin.h"

#include <filesystem>
#include <iostream>
#include <string>
#include "LogManager.h"


#if defined(_WIN32) || defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #define DYNLIB_HANDLE HINSTANCE
    #define DYNLIB_LOAD LoadLibraryA
    #define DYNLIB_GETSYM GetProcAddress
    #define DYNLIB_UNLOAD FreeLibrary
#else
    #include <dlfcn.h>
    #define DYNLIB_HANDLE void*
    #define DYNLIB_LOAD(path) dlopen(path, RTLD_LAZY)
    #define DYNLIB_GETSYM dlsym
    #define DYNLIB_UNLOAD dlclose
#endif

using namespace std;

namespace PeachCore {

    typedef Plugin* (*CreatePluginFunc)();
    typedef void (*DestroyPluginFunc)(Plugin*);

    class PluginManager
    {
    public:
        static PluginManager& ManagePlugins() {
            static PluginManager instance;
            return instance;
        }

        void LoadPlugin(const std::string& path);
        void InitializePlugins();
        void UpdatePlugins(float fp_TimeSinceLastFrame);
        void ConstantUpdatePlugins(float fp_TimeSinceLastFrame);
        void ShutdownPlugins();

    private:
        PluginManager() = default;
        ~PluginManager() = default;
        PluginManager(const PluginManager&) = delete;
        PluginManager& operator=(const PluginManager&) = delete;

        std::vector<std::unique_ptr<Plugin, DestroyPluginFunc>> pm_PluginInstances;
        std::vector<DYNLIB_HANDLE> pm_PluginHandles;
    };

    struct DynamicLoader
    {
    public:
        DynamicLoader(const string& dllPath)
        {
            if (filesystem::exists(dllPath))
            {
                pm_LibraryHandle = DYNLIB_LOAD(dllPath.c_str());

                if (not pm_LibraryHandle)
                {
                    cerr << "Failed to load library: " << dllPath << " Error: " << GetLastErrorAsString() << endl;
                }
                else
                {
                    cout << "Library loaded successfully: " << dllPath << endl;
                }
            }
            else
            {
                cerr << "Library path does not exist: " << dllPath << endl;
            }
        }

        ~DynamicLoader()
        {
            if (pm_LibraryHandle)
            {
                if (not DYNLIB_UNLOAD(pm_LibraryHandle))
                {
                    cerr << "Failed to unload library. Error: " << GetLastErrorAsString() << endl;
                }
                else
                {
                    cout << "Library unloaded successfully." << endl;
                }
            }
        }

        // Function to retrieve symbols (functions/variables) from the library
        void* GetSymbol(const string& symbolName)
        {
            void* symbol = nullptr;

            if (pm_LibraryHandle)
            {
                symbol = (void*)DYNLIB_GETSYM(pm_LibraryHandle, symbolName.c_str());

                if (not symbol)
                {
                    cerr << "Failed to locate symbol: " << symbolName << " Error: " << GetLastErrorAsString() << endl;
                }
                else
                {
                    cout << "Symbol located: " << symbolName << endl;
                }
            }
            return symbol;
        }

    private:
        DYNLIB_HANDLE pm_LibraryHandle = nullptr;

        // Helper function to get the error message string
        string GetLastErrorAsString()
        {
#if defined(_WIN32) || defined(_WIN64)
            // Windows error message
            DWORD errorMessageID = ::GetLastError();
            if (errorMessageID == 0)
                return string(); // No error message has been recorded

            LPSTR messageBuffer = nullptr;
            size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

            string message(messageBuffer, size);
            LocalFree(messageBuffer);
            return message;
#else
            // POSIX error message
            return string(dlerror());
#endif
        }
    };
}
