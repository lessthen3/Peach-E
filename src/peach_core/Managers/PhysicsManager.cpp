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

    [[nodiscard]] bool
        PhysicsManager::Initialize(const string& fp_LogOutputDirectory)
    {
        physics_logger = Logger::CreateUnique("PhysicsManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_LogOutputDirectory);

        if (not physics_logger)
        {
            PRINT_ERROR("PhysicsManager failed to initialize the physics_thread logger >w<");
            return false;
        }

        physics_logger->Debug("PhysicsLogger successfully initialized", "PhysicsManager");

        pm_PhysicsCommandQueue = make_shared<PhysicsCommandPipe>();

        return true;
    }

    [[nodiscard]] bool
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

        pm_IsInitialized = true;

        return true;
    }

    void
        PhysicsManager::PhysicsLoop2D
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch,
            const float fp_GravityX,
            const float fp_GravityY
        )
    {
        if (not InitializePhysicsEngine2D(fp_LogOutputDirectory, fp_GravityX, fp_GravityY))
        {
            PRINT_ERROR("Failed to Initialize Physics Thread!");
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

    [[nodiscard]] bool
        PhysicsManager::InitializePhysicsEngine3D
        (
            const string& fp_LogOutputDirectory
        )
    {
        if (not Initialize(fp_LogOutputDirectory))
        {
            return false;
        }
        
        //Jolt stuff uwu

        pm_IsInitialized = true;

        return true;
    }

    void
        PhysicsManager::PhysicsLoop3D
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch
        )
    {
        if (not InitializePhysicsEngine3D(fp_LogOutputDirectory))
        {
            PRINT_ERROR("Failed to Initialize Physics Thread!");
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
        PhysicsManager::RequestPhysicsWorldStep()
    {
        pm_PhysicsSemaphore.release();
    }

    void
        PhysicsManager::Stop()
    {
        pm_IsRunning.store(false, std::memory_order_release);
        pm_PhysicsSemaphore.release(); // Wake it up to exit        
    }

    [[nodiscard]] shared_ptr<PhysicsCommandPipe>
        PhysicsManager::GetPhysicsCommandQueue(Logger* const logger)
    {
        if (not logger)
        {
            PRINT_ERROR("TRIED TO PASS NULL_PTR REF TO LOGGER INSIDE GetAudioCommandQueue()");
            return nullptr;
        }
        else if (not pm_IsInitialized)
        {
            logger->Error("Attempted to get a reference to AudioManager's AudioCommandQueue before AudioManager was initialized, please initialize AudioManager first UwU", "AudioManager");
            return nullptr;
        }
        else if (pm_PhysicsCommandQueue.use_count() >= 2)
        {
            logger->Warning("AudioManager has already issued a reference to the audio command queue, fuck off", "AudioManager");
            return nullptr;
        }

        return pm_PhysicsCommandQueue;
    }
}