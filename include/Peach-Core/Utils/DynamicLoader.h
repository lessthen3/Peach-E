/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include "../Managers/LogManager.h"

#include <memory>

#if defined(_WIN32) || defined(_WIN64)
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

namespace PeachCore {

    //////////////////////////////////////////////
    // Dynamic Library Loader Struct
    //////////////////////////////////////////////
    struct DynamicLoader
    {
    public:
        DynamicLoader() = default;
        ~DynamicLoader() = default;

        DYNLIB_HANDLE
            LoadDynamicLibrary
            (
                const string& fp_DylibPath,
                LogManager* logger
            )
        {
            if (not filesystem::exists(fp_DylibPath))
            {
                logger->LogAndPrint(format("Library path does not exist: '{}'", fp_DylibPath), "DynamicLoader", LogManager::LogLevel::Error);
                return nullptr;
            }

            DYNLIB_HANDLE f_LibraryHandle = DYNLIB_LOAD(fp_DylibPath.c_str());

            if (not f_LibraryHandle)
            {
                logger->LogAndPrint(format("Failed to load library: '{}',  Error: '{}'", fp_DylibPath, GetLastErrorAsString()), "DynamicLoader", LogManager::LogLevel::Error);
                return nullptr;
            }

            logger->LogAndPrint(("Library loaded successfully: '{}'", fp_DylibPath), "DynamicLoader", LogManager::LogLevel::Info);

            return f_LibraryHandle;
        }

        bool
            UnloadLibrary
            (
                DYNLIB_HANDLE fp_LibraryHandle,
                LogManager* logger
            )
        {
            if (not fp_LibraryHandle)
            {
                logger->LogAndPrint("Tried passing a nullptr reference to a DYNLIB_HANDLE inside GetSymbol()", "DynamicLoader", LogManager::LogLevel::Error);
                return false;
            }

            if (not DYNLIB_UNLOAD(fp_LibraryHandle))
            {
                logger->LogAndPrint(format("Failed to unload library. Error: '{}'", GetLastErrorAsString()), "DynamicLoader", LogManager::LogLevel::Error);
                return false;
            }
            
            logger->LogAndPrint("Library unloaded successfully", "DynamicLoader", LogManager::LogLevel::Info);

            return true; //Unloaded Library Successfully! >W<
        }

        // Function to retrieve symbols (functions/variables) from the library
        void* 
            GetSymbol
            (
                const string& fp_SymbolName, 
                DYNLIB_HANDLE fp_LibraryHandle,
                LogManager* logger
            )
        {
            void* symbol = nullptr;

            if (not fp_LibraryHandle)
            {
                logger->LogAndPrint("Tried passing a nullptr reference to a DYNLIB_HANDLE inside GetSymbol()", "DynamicLoader", LogManager::LogLevel::Error);
                return nullptr;
            }

            symbol = (void*)DYNLIB_GETSYM(fp_LibraryHandle, fp_SymbolName.c_str());

            if (not symbol)
            {
                logger->LogAndPrint(format("Failed to locate symbol: '{}', Error: '{}'", fp_SymbolName, GetLastErrorAsString()), "DynamicLoader", LogManager::LogLevel::Error);
                return nullptr; //its already nullptr but its nice to be explicit here
            }
            
            logger->LogAndPrint(format("Symbol located: '{}'", fp_SymbolName), "DynamicLoader", LogManager::LogLevel::Debug);

            return symbol;
        }

    private:
        // Helper function to get the error message string
        string 
            GetLastErrorAsString()
        {
            #if defined(_WIN32) or defined(_WIN64) //wtf windows are u okay
                // Windows error message
                DWORD errorMessageID = ::GetLastError();

                if (errorMessageID == 0)
                {
                    return "No error found UwU"; // No error message has been recorded
                }

                LPSTR messageBuffer = nullptr;
                size_t size = FormatMessageA
                (
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, 
                    errorMessageID, 
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                    (LPSTR)&messageBuffer, 
                    0, 
                    NULL
                );

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
