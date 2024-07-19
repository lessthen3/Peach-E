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
#include <tbb/tbb.h>
//#include "ScriptEngineManager.h"

using namespace std;

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
            enforceHardEventSync.store(value, memory_order_release);
        }

    private:
        ThreadPoolManager() = default;

    public:

        void Initialize(size_t fp_MaxThreads = 4)
        {
            m_MaxThreads = fp_MaxThreads;
            m_Stop = false;

            // Initialize the script engine manager
           // m_ScriptEngine = ScriptEngineManager::ScriptEngine().CreateScriptEngine(); //lifecycle of scriptengine is the entire program so no need to clean up explicitly

            m_Workers.reserve(m_MaxThreads);
            for (size_t i = 0; i < m_MaxThreads; ++i) {
                m_Workers.emplace_back(&ThreadPoolManager::Worker, this);
            }
        }

        void EnqueueEventBatch(const vector<function<void()>>& fp_Tasks) {
            lock_guard<mutex> lock(m_QueueMutex);
            cout << "Enqueueing Event Batch of size: " << fp_Tasks.size() << endl;
            pm_EventTaskBatches.push(fp_Tasks);
        }

        void EnqueueContinuous(const function<void()>& fp_Task, int fp_Priority) {
            lock_guard<mutex> lock(m_QueueMutex);
            cout << "Enqueueing Continuous Task at priority: " << fp_Priority << endl;
            m_ContinuousTasks[fp_Priority].push(fp_Task);
        }

        void EnqueueOneTime(const function<void()>& fp_Task, int fp_Priority) {
            lock_guard<mutex> lock(m_QueueMutex);
            cout << "Enqueueing One-Time Task at priority: " << fp_Priority << endl;
            m_OneTimeTasks[fp_Priority].push(fp_Task);
        }

        void ProcessTasks() {
            lock_guard<mutex> lock(m_QueueMutex);
            cout << "Processing Tasks" << endl;

            // Move event tasks to the main queue with highest priority
            while (!pm_EventTaskBatches.empty()) {
                auto& batch = pm_EventTaskBatches.front();
                cout << "Moving Event Batch of size: " << batch.size() << " to main queue" << endl;
                pm_EventTaskBatches.pop();
                for (auto& task : batch) {
                    m_Tasks[0].push(task);
                }
            }

            for (auto& [priority, queue] : m_OneTimeTasks) {
                while (!queue.empty()) {
                    cout << "Moving One-Time Task at priority: " << priority << " to main queue" << endl;
                    m_Tasks[priority].push(queue.front());
                    queue.pop();
                }
            }
            m_OneTimeTasks.clear();

            for (auto& [priority, queue] : m_ContinuousTasks) {
                while (!queue.empty()) {
                    cout << "Moving Continuous Task at priority: " << priority << " to main queue" << endl;
                    m_Tasks[priority].push(queue.front());
                    queue.pop();
                }
            }

            m_Condition.notify_all();
        }

        void Shutdown() {
            {
                lock_guard<mutex> lock(m_QueueMutex);
                m_Stop = true;
                cout << "Shutting down thread pool" << endl;
                m_Condition.notify_all();
            }
            for (thread& worker : m_Workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

    private:
        void Worker() {
            while (true) {
                function<void()> f_Task;
                bool eventTasksPending = false;

                {
                    unique_lock<mutex> lock(m_QueueMutex);
                    cout << "Worker waiting for tasks" << endl;
                    m_Condition.wait(lock, [this] { return m_Stop || !AreTasksEmpty() || !pm_EventTaskBatches.empty(); });
                    cout << "Worker woke up" << endl;

                    if (m_Stop && AreTasksEmpty()) {
                        cout << "Stopping worker as no tasks are left" << endl;
                        break;
                    }

                    // Ensure all event tasks are processed first
                    if (!pm_EventTaskBatches.empty()) {
                        auto& batch = pm_EventTaskBatches.front();
                        cout << "Processing Event Batch of size: " << batch.size() << endl;
                        pm_EventTaskBatches.pop();
                        for (auto& task : batch) {
                            task();  // Execute each task in the batch
                        }
                        eventTasksPending = true;  // Set eventTasksPending after processing event tasks
                        m_Condition.notify_all(); // Notify other threads that event tasks are done
                        continue;  // Skip to the next iteration
                    }
                    else if (eventTasksPending) {
                        cout << "Waiting for other threads to finish event tasks" << endl;
                        eventTasksPending = false; // Reset pending flag after handling
                        m_Condition.wait(lock, [this] { return pm_EventTaskBatches.empty() && AreTasksEmpty(); });
                    }
                    else {
                        for (auto& [priority, queue] : m_Tasks) {
                            if (!queue.empty()) {
                                cout << "Worker taking task from priority: " << priority << endl;
                                f_Task = move(queue.front());
                                queue.pop();
                                currentPriority = priority;
                                break;
                            }
                        }
                    }

                    // Synchronize event tasks processing
                    if (!f_Task && enforceHardEventSync.load(memory_order_acquire)) {
                        cout << "Worker waiting for synchronization" << endl;
                        m_IdleThreadCount++;
                        if (m_IdleThreadCount == m_MaxThreads) {
                            m_Condition.notify_all();
                        }
                        m_Condition.wait(lock, [this] { return pm_EventTaskBatches.empty() && AreTasksEmpty(); });
                        m_IdleThreadCount--;
                    }
                }

                if (f_Task) {
                    cout << "Worker executing task" << endl;
                    f_Task(); // Execute the task
                }

                // Notify other threads if all tasks are done
                if (enforceHardEventSync.load(memory_order_acquire)) {
                    unique_lock<mutex> lock(m_QueueMutex);
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

        vector<thread> m_Workers;
        map<int, queue<function<void()>>> m_Tasks; // Main task queue categorized by priority
        map<int, queue<function<void()>>> m_ContinuousTasks; // Continuous tasks
        map<int, queue<function<void()>>> m_OneTimeTasks; // One-time tasks
        queue<vector<function<void()>>> pm_EventTaskBatches; // Event task batches
        mutex m_QueueMutex;
        condition_variable m_Condition;
        bool m_Stop = false;
        size_t m_MaxThreads;
        atomic<bool> enforceHardEventSync{ false };
        atomic<size_t> m_IdleThreadCount{ 0 };
        int currentPriority = 0;
        //asIScriptEngine* m_ScriptEngine;
    };

}