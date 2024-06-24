#include <iostream>
#include "PeachE-core/Managers/LogManager.h"
#include "PeachE-core/Managers/SceneTreeManager.h"
#include "PeachE-core/Managers/InputManager.h"
#include "PeachE-core/Managers/EngineManager.h"

int main()
{
    LogManager::Logger().init("D:/Game Development/Random Junk I Like to Keep/LogTestMinGE");
    std::cout << "Hello World!\n";
    LogManager::Logger().warn("NEW ENGINE ON THE BLOCK MY SLIME", "PeachE");

    LogManager::Logger().trace("Success! This Built Correctly", "PeachE");
}