#include <iostream>
#include "../Peach-core/Managers/Managers.h"



int main()
{
    LogManager::Logger().init("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE");
    std::cout << "Hello World!\n";
    LogManager::Logger().warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

    LogManager::Logger().trace("Success! This Built Correctly", "Peach-E");

    //////////////////////////////////////////////
    // Plugin Loader Step
    //////////////////////////////////////////////

    //#if defined(_WIN32) || defined(_WIN64)
    //    PluginManager::ManagePlugins().LoadPlugin("SimplePlugin.dll"); // Windows
    //#else
    //    PluginManager::ManagePlugins().LoadPlugin("./libSimplePlugin.so"); // Linux/Unix
    //#endif


}