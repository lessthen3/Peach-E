#include <iostream>
#include "../Peach-core/Managers/Managers.h"



int main()
{
    LogManager::Logger().Init("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE");
    std::cout << "Hello World!\n";
    LogManager::Logger().Warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

    LogManager::Logger().Trace("Success! This Built Correctly", "Peach-E");

    //////////////////////////////////////////////
    // Plugin Loader Step
    //////////////////////////////////////////////

    #if defined(_WIN32) || defined(_WIN64)
        PluginManager::ManagePlugins().LoadPlugin("D:/Game Development/MinGE/src/Peach-E-Core/SimplePlugin.dll"); // Windows
    #else
        PluginManager::ManagePlugins().LoadPlugin("./libSimplePlugin.so"); // Linux/Unix
    #endif

    PluginManager::ManagePlugins().InitializePlugins();
    PluginManager::ManagePlugins().UpdatePlugins(0.1f);
    PluginManager::ManagePlugins().ShutdownPlugins();

    return EXIT_SUCCESS;
}