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
#include "ThreadPool.h"

#include <fmt/format.h>

namespace PeachCore{

    void
        ThreadPool::Shutdown()
    {
        {
            lock_guard<mutex> lock(m_QueueMutex);
            m_Stop = true;
            PRINT("Shutting down thread pool", Colours::BrightBlue);
            m_Condition.notify_all();
        }

        for (thread& worker : m_Workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void
        ThreadPool::ProcessTasks()
    {
        lock_guard<mutex> lock(m_QueueMutex);
        cout << "Processing Tasks" << endl;

        // Move event tasks to the main queue with highest priority
        while (not pm_EventTaskBatches.empty())
        {
            auto& batch = pm_EventTaskBatches.front();

            cout << "Moving Event Batch of size: " << batch.size() << " to main queue" << endl;

            pm_EventTaskBatches.pop();

            for (auto& task : batch)
            {
                m_Tasks[0].push(task);
            }
        }

        for (auto& [priority, queue] : m_OneTimeTasks)
        {
            while (not queue.empty())
            {
                cout << "Moving One-Time Task at priority: " << priority << " to main queue" << endl;
                m_Tasks[priority].push(queue.front());
                queue.pop();
            }
        }

        m_OneTimeTasks.clear();

        for (auto& [priority, queue] : m_ContinuousTasks)
        {
            while (not queue.empty())
            {
                cout << "Moving Continuous Task at priority: " << priority << " to main queue" << endl;

                m_Tasks[priority].push(queue.front());
                queue.pop();
            }
        }

        m_Condition.notify_all();
    }

    void
        ThreadPool::Worker(int fp_ThreadNumber)
    {
        thread_local Logger worker_logger = Logger::Create(fmt::format("Thread_{}", fp_ThreadNumber), PEACH_LOGGER_DEFAULT_FLAGS).value();

        while (true)
        {
            function<void()> f_Task;

            bool eventTasksPending = false;
            {
                unique_lock<mutex> lock(m_QueueMutex);
                PRINT("Worker waiting for tasks", Colours::White);
                m_Condition.wait(lock, [this] { return m_Stop or not AreTasksEmpty() or not pm_EventTaskBatches.empty(); });
                PRINT("Worker woke up", Colours::White);

                if (m_Stop and AreTasksEmpty())
                {
                    PRINT("Stopping worker as no tasks are left", Colours::White);
                    break;
                }

                // Ensure all event tasks are processed first
                if (not pm_EventTaskBatches.empty())
                {
                    auto& batch = pm_EventTaskBatches.front();
                    PRINT(fmt::format("Processing Event Batch of size: {}", batch.size()), Colours::White);
                    pm_EventTaskBatches.pop();

                    for (auto& task : batch)
                    {
                        task();  // Execute each task in the batch
                    }

                    eventTasksPending = true;  // Set eventTasksPending after processing event tasks
                    m_Condition.notify_all(); // Notify other threads that event tasks are done
                    continue;  // Skip to the next iteration
                }
                else if (eventTasksPending)
                {
                    PRINT("Waiting for other threads to finish event tasks", Colours::White);
                    eventTasksPending = false; // Reset pending flag after handling
                    m_Condition.wait(lock, [this] { return pm_EventTaskBatches.empty() && AreTasksEmpty(); });
                }
                else
                {
                    for (auto& [priority, queue] : m_Tasks)
                    {
                        if (not queue.empty())
                        {
                            PRINT(fmt::format("Worker taking task from priority: {}", priority), Colours::White);
                            f_Task = move(queue.front());
                            queue.pop();
                            currentPriority = priority;
                            break;
                        }
                    }
                }

                // Synchronize event tasks processing
                if (not f_Task and enforceHardEventSync.load(memory_order_acquire))
                {
                    PRINT("Worker waiting for synchronization", Colours::White);
                    m_IdleThreadCount++;

                    if (m_IdleThreadCount == m_MaxThreads)
                    {
                        m_Condition.notify_all();
                    }

                    m_Condition.wait(lock, [this] { return pm_EventTaskBatches.empty() && AreTasksEmpty(); });
                    m_IdleThreadCount--;
                }
            }

            if (f_Task)
            {
                PRINT("Worker executing task", Colours::White);
                f_Task(); // Execute the task
            }

            // Notify other threads if all tasks are done
            if (enforceHardEventSync.load(memory_order_acquire))
            {
                unique_lock<mutex> lock(m_QueueMutex);

                if (pm_EventTaskBatches.empty() and AreTasksEmpty())
                {
                    m_Condition.notify_all();
                }
            }
        }
    }
}