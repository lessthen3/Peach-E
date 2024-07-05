#pragma once

#include <vector>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <iostream>

namespace PeachCore {

    struct ThreadPoolManager {
    public:
        static ThreadPoolManager& ThreadPool() {
            static ThreadPoolManager instance; // Default to system concurrency
            return instance;
        }

        ~ThreadPoolManager() {
            Shutdown();
        }

        void SetEnforceHardEventSync(bool value) {
            enforceHardEventSync.store(value, std::memory_order_release);
        }

    private:
        ThreadPoolManager() = default;

    public:

        void Initialize(size_t fp_MaxThreads = 4)
        {
            m_Stop = false;
            m_MaxThreads = fp_MaxThreads;

            m_Workers.reserve(m_MaxThreads);
            for (size_t i = 0; i < m_MaxThreads; ++i) {
                m_Workers.emplace_back(&ThreadPoolManager::Worker, this);
            }
        }

        void EnqueueEventBatch(const std::vector<std::function<void()>>& fp_Tasks) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            std::cout << "Enqueueing Event Batch of size: " << fp_Tasks.size() << std::endl;
            pm_EventTaskBatches.push(fp_Tasks);
        }

        void EnqueueContinuous(const std::function<void()>& fp_Task, int fp_Priority) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            std::cout << "Enqueueing Continuous Task at priority: " << fp_Priority << std::endl;
            m_ContinuousTasks[fp_Priority].push(fp_Task);
        }

        void EnqueueOneTime(const std::function<void()>& fp_Task, int fp_Priority) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            std::cout << "Enqueueing One-Time Task at priority: " << fp_Priority << std::endl;
            m_OneTimeTasks[fp_Priority].push(fp_Task);
        }

        void ProcessTasks() {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            std::cout << "Processing Tasks" << std::endl;

            // Move event tasks to the main queue with highest priority
            while (!pm_EventTaskBatches.empty()) {
                auto& batch = pm_EventTaskBatches.front();
                std::cout << "Moving Event Batch of size: " << batch.size() << " to main queue" << std::endl;
                pm_EventTaskBatches.pop();
                for (auto& task : batch) {
                    m_Tasks[0].push(task);
                }
            }

            for (auto& [priority, queue] : m_OneTimeTasks) {
                while (!queue.empty()) {
                    std::cout << "Moving One-Time Task at priority: " << priority << " to main queue" << std::endl;
                    m_Tasks[priority].push(queue.front());
                    queue.pop();
                }
            }
            m_OneTimeTasks.clear();

            for (auto& [priority, queue] : m_ContinuousTasks) {
                while (!queue.empty()) {
                    std::cout << "Moving Continuous Task at priority: " << priority << " to main queue" << std::endl;
                    m_Tasks[priority].push(queue.front());
                    queue.pop();
                }
            }

            m_Condition.notify_all();
        }

        void Shutdown() {
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                m_Stop = true;
                std::cout << "Shutting down thread pool" << std::endl;
                m_Condition.notify_all();
            }
            for (std::thread& worker : m_Workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

    private:
        void Worker() {
            while (true) {
                std::function<void()> f_Task;
                bool eventTasksPending = false;

                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    std::cout << "Worker waiting for tasks" << std::endl;
                    m_Condition.wait(lock, [this] { return m_Stop || !AreTasksEmpty() || !pm_EventTaskBatches.empty(); });
                    std::cout << "Worker woke up" << std::endl;

                    if (m_Stop && AreTasksEmpty()) {
                        std::cout << "Stopping worker as no tasks are left" << std::endl;
                        break;
                    }

                    // Ensure all event tasks are processed first
                    if (!pm_EventTaskBatches.empty()) {
                        auto& batch = pm_EventTaskBatches.front();
                        std::cout << "Processing Event Batch of size: " << batch.size() << std::endl;
                        pm_EventTaskBatches.pop();
                        for (auto& task : batch) {
                            task();  // Execute each task in the batch
                        }
                        eventTasksPending = true;  // Set eventTasksPending after processing event tasks
                        m_Condition.notify_all(); // Notify other threads that event tasks are done
                        continue;  // Skip to the next iteration
                    }
                    else if (eventTasksPending) {
                        std::cout << "Waiting for other threads to finish event tasks" << std::endl;
                        eventTasksPending = false; // Reset pending flag after handling
                        m_Condition.wait(lock, [this] { return pm_EventTaskBatches.empty() && AreTasksEmpty(); });
                    }
                    else {
                        for (auto& [priority, queue] : m_Tasks) {
                            if (!queue.empty()) {
                                std::cout << "Worker taking task from priority: " << priority << std::endl;
                                f_Task = std::move(queue.front());
                                queue.pop();
                                currentPriority = priority;
                                break;
                            }
                        }
                    }

                    // Synchronize event tasks processing
                    if (!f_Task && enforceHardEventSync.load(std::memory_order_acquire)) {
                        std::cout << "Worker waiting for synchronization" << std::endl;
                        m_IdleThreadCount++;
                        if (m_IdleThreadCount == m_MaxThreads) {
                            m_Condition.notify_all();
                        }
                        m_Condition.wait(lock, [this] { return pm_EventTaskBatches.empty() && AreTasksEmpty(); });
                        m_IdleThreadCount--;
                    }
                }

                if (f_Task) {
                    std::cout << "Worker executing task" << std::endl;
                    f_Task(); // Execute the task
                }

                // Notify other threads if all tasks are done
                if (enforceHardEventSync.load(std::memory_order_acquire)) {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    if (pm_EventTaskBatches.empty() && AreTasksEmpty()) {
                        m_Condition.notify_all();
                    }
                }
            }
        }

        bool AreTasksEmpty() const {
            for (const auto& [priority, queue] : m_Tasks) {
                if (!queue.empty()) return false;
            }
            return true;
        }

        std::vector<std::thread> m_Workers;
        std::map<int, std::queue<std::function<void()>>> m_Tasks; // Main task queue categorized by priority
        std::map<int, std::queue<std::function<void()>>> m_ContinuousTasks; // Continuous tasks
        std::map<int, std::queue<std::function<void()>>> m_OneTimeTasks; // One-time tasks
        std::queue<std::vector<std::function<void()>>> pm_EventTaskBatches; // Event task batches
        std::mutex m_QueueMutex;
        std::condition_variable m_Condition;
        bool m_Stop = false;
        size_t m_MaxThreads;
        std::atomic<bool> enforceHardEventSync{ false };
        std::atomic<size_t> m_IdleThreadCount{ 0 };
        int currentPriority = 0;
    };

}
