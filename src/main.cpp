#include <iostream>
#include "MinGE-core/Managers/LogManager.h"

int main()
{
    LogManager::Logger().init("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE");
    std::cout << "Hello World!\n";
    LogManager::Logger().warn("NEW ENGINE ON THE BLOCK MY SLIME", "MinGE");

    ;
}