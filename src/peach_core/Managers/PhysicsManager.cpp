/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#include "PhysicsManager.h"

namespace PeachCore {

    void
        PhysicsManager::PhysicsLoop
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch
        )
    {
        if (not InitializePhysicsEngine(fp_LogOutputDirectory))
        {

            return;
        }

        fp_InitLatch.count_down();

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            // Block until main thread wakes us
            pm_PhysicsSemaphore.acquire();

            if (not pm_IsRunning.load(std::memory_order_acquire))
            {
                break; // Double check after wake
            }

            //ProcessCommands();
        }
    }

    void
        PhysicsManager::RequestPhysicsFrame()
    {
        pm_PhysicsSemaphore.release();
    }

    void
        PhysicsManager::Stop()
    {
        pm_IsRunning.store(false, std::memory_order_release);
        pm_PhysicsSemaphore.release(); // Wake it up to exit        
    }
}