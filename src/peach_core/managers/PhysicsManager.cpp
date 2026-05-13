/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#include "PhysicsManager.h"
#include "LogManager.h"

namespace PeachCore {

    bool
        PhysicsManager::Initialize(const string& fp_LogOutputDirectory)
    {
        physics_logger = LogManager::get_single().CreateUniqueLogger("PhysicsManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_LogOutputDirectory);

        if (not physics_logger)
        {
            PEACH_PRINT_ERROR("PhysicsManager failed to initialize the physics_thread logger >w<");
            return false;
        }

        physics_logger->Info("PhysicsLogger successfully initialized", "PhysicsManager");

        return true;
    }

    bool
        PhysicsManager::InitializePhysicsEngine2D
        (
            const string& fp_LogOutputDirectory,
            const float fp_GravityX,
            const float fp_GravityY
        )
    {
        if (not Initialize(fp_LogOutputDirectory))
        {
            return false;
        }

        PEACH_TO_DO_UNUSED(fp_GravityX);
        PEACH_TO_DO_UNUSED(fp_GravityY);

        pm_PhysicsThread = thread
        (
            &PhysicsManager::PhysicsLoop2D,
            this
        );

        pm_IsInitialized = true;

        return true;
    }

    bool
        PhysicsManager::InitializePhysicsEngine3D(const string& fp_LogOutputDirectory)
    {
        if (not Initialize(fp_LogOutputDirectory))
        {
            return false;
        }

        pm_PhysicsThread = thread
        (
            &PhysicsManager::PhysicsLoop3D,
            this
        );
        
        //Jolt stuff uwu

        pm_IsInitialized = true;

        return true;
    }

    void
        PhysicsManager::PhysicsLoop2D()
    {
        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            // Block until main thread wakes us
            pm_PhysicsSemaphore.acquire();

            if (not pm_IsRunning.load(std::memory_order_acquire)) //IMPORTANT: this is here so that when we call release() from Stop() it exits and hits the latch owo 
            {
                break; // Double check after wake
            }

            //ProcessCommands();
        }

        pm_IsFinishedStoppingLatch.count_down(); //IMPORTANT: this is required so the thread can be stopped properly and joined safely owo
    }

    void
        PhysicsManager::PhysicsLoop3D()
    {
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

        pm_IsFinishedStoppingLatch.count_down(); //IMPORTANT: this is required so the thread can be stopped properly and joined safely owo
    }

    void
        PhysicsManager::RequestPhysicsWorldStep
        (
            float fp_Dt, 
            size_t fp_Steps
        )
    {
        PEACH_TO_DO_UNUSED(fp_Dt);
        PEACH_TO_DO_UNUSED(fp_Steps);

        pm_PhysicsSemaphore.release();
    }

    void
        PhysicsManager::ShutdownSubsystem(Logger*const logger)
    {
        if(not logger) [[unlikely]]
        {
            PEACH_PRINT_ERROR("Tried to pass nullptr reference to logger inside PhysicsManager::ShutdownSubsystem()");
            return;
        }

        if(not pm_IsInitialized.load(std::memory_order_acquire))
        {
            logger->Error("Tried to call ShutdownSubsystem() on Physics Manager when physics thread was never started owo wtf mang ;w;", "PhysicsManager"); 
            return;
        }

        pm_IsRunning.store(false, std::memory_order_release);
        pm_PhysicsSemaphore.release(); // Wake it up to exit        

        pm_IsFinishedStoppingLatch.wait(); //block until thread loop has exited the loop owo

        if (pm_PhysicsThread.joinable()) 
        {
            pm_PhysicsThread.join(); 
            logger->Info("Successfully joined physics thread", "PhysicsManager");
        }
    }
}