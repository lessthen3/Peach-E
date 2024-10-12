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
#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>;

#include "../../include/Peach-Engine/PeachEngineManager.h"

#include <iostream>
#include <string>

#include <thread>

using namespace std;
namespace PC = PeachCore;

static void RenderThread()
{
    while (true)
    {
        // Play audio
        cout << "Playing ur mom LOL...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void AudioThread()
{
    while (true)
    {
        // Play audio
        cout << "Playing audio...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void ResourceLoadingThread()
{
    while (true)
    {
        // Load resources
        cout << "Loading resources...\n";
        this_thread::sleep_for(chrono::milliseconds(100)); // Simulate work
    }
}

static void NetworkThread()
{
    while (true)
    {
        // Handle network communication
        cout << "Handling network...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

static void PhysicsThread() //processes all physics, changing structure of engine because main thread should execute scripts instead of physics calculations
{
    while (true)
    {
        // Handle network communication
        cout << "Handling network...\n";
        this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
    }
}

//PYBIND11_MODULE(peach_engine, fp_Module)
//{
//    PythonScriptManager::Python().InitializePythonBindingsForPeachCore(fp_Module);
//}

int main(int fp_ArgCount, const char* fp_ArgVector[])
{
    cout << "Hello World!\n";

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        PC::LogManager::RenderingLogger().LogAndPrint("SDL could not initialize! SDL_Error: " + string(SDL_GetError()), "RenderingManager", "fatal");
        return false;
    }

    SDL_Quit(); //just makes more sense to have the main method do this

    return 0;
}