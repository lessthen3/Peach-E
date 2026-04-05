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

#include "Logger.h"

namespace PeachCore {

    using namespace std;

    struct ThreadPool 
    {
    private:
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
   
    public:
        ThreadPool() = default;

        ~ThreadPool() 
        {
            Shutdown();
        }

    public:

        inline void 
            SetEnforceHardEventSync(bool value)
        {
            enforceHardEventSync.store(value, memory_order_release);
        }

        inline void 
            Initialize(size_t fp_MaxThreads = 4)
        {
            m_MaxThreads = fp_MaxThreads;
            m_Stop = false;

            m_Workers.reserve(m_MaxThreads);

            for (size_t lv_Index = 0; lv_Index < m_MaxThreads; ++lv_Index)
            {
                m_Workers.emplace_back(&ThreadPool::Worker, this, lv_Index);
            }
        }

        void
            ResizeThreadPool()
        {

        }

        inline void 
            EnqueueEventBatch(const vector<function<void()>>& fp_Tasks) 
        {
            lock_guard<mutex> lock(m_QueueMutex);
            pm_EventTaskBatches.push(fp_Tasks);
        }

        inline void 
            EnqueueContinuous(const function<void()>& fp_Task, int fp_Priority) 
        {
            lock_guard<mutex> lock(m_QueueMutex);
            m_ContinuousTasks[fp_Priority].push(fp_Task);
        }

        inline void 
            EnqueueOneTime(const function<void()>& fp_Task, int fp_Priority) 
        {
            lock_guard<mutex> lock(m_QueueMutex);
            m_OneTimeTasks[fp_Priority].push(fp_Task);
        }

        void
            ProcessTasks();

        void
            Shutdown();

    private:
        void
            Worker(int fp_ThreadNumber);

        [[nodiscard]] bool 
            AreTasksEmpty() 
            const 
        {
            for (const auto& [priority, queue] : m_Tasks)
            {
                if (not queue.empty()) 
                {
                    return false;
                }
            }

            return true;
        }
    };
}