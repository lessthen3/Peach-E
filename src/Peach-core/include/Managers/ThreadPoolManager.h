#pragma once

#include <vector>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

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

    private:
        ThreadPoolManager(size_t fp_MaxThreads = 4) : m_Stop(false), m_MaxThreads(fp_MaxThreads) {
            m_Workers.reserve(m_MaxThreads);
            for (size_t i = 0; i < m_MaxThreads; ++i) {
                m_Workers.emplace_back(&ThreadPoolManager::Worker, this);
            }
        }

    public:
        void EnqueueContinuous(const std::function<void()>& fp_Task, int fp_Priority) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_ContinuousTasks[fp_Priority].push(fp_Task);
        }

        void EnqueueOneTime(const std::function<void()>& fp_Task, int fp_Priority) {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_OneTimeTasks[fp_Priority].push(fp_Task);
        }

        void ProcessTasks() {
            std::lock_guard<std::mutex> lock(m_QueueMutex);

            for (auto& [priority, queue] : m_OneTimeTasks) {
                while (!queue.empty()) {
                    m_Tasks[priority].push(queue.front());
                    queue.pop();
                }
            }
            m_OneTimeTasks.clear();

            for (auto& [priority, queue] : m_ContinuousTasks) {
                while (!queue.empty()) {
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
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    m_Condition.wait(lock, [this] { return m_Stop || !AreTasksEmpty(); });
                    if (m_Stop && AreTasksEmpty()) 
                        {break;}

                    for (auto& [priority, queue] : m_Tasks) {
                        if (!queue.empty()) {
                            f_Task = std::move(queue.front());
                            queue.pop();
                            break;
                        }
                    }
                }
                if (f_Task) 
                    {f_Task();} // Execute the task
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
        std::mutex m_QueueMutex;
        std::condition_variable m_Condition;
        bool m_Stop;
        size_t m_MaxThreads;
    };

}
