#pragma once
#include <map>
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <typeindex>
#include <functional>

namespace PeachCore {

    template<typename Event>
    struct EventQueueManager {

    public:
        static EventQueueManager& EventQueue() {
            static EventQueueManager<Event> instance;
            return instance;
        }

    public:

        template<typename EventType>
        void PostEvent(const EventType& event) {
            static_assert(std::is_base_of<Event, EventType>::value, "EventType must derive from Event");
            std::lock_guard<std::mutex> lock(mutex);
            m_Events[typeid(EventType)].push({ std::make_shared<EventType>(event), m_CurrentFrame });
        }

        template<typename EventType, typename Func>
        void Subscribe(Func&& func) {
            std::lock_guard<std::mutex> lock(mutex);
            handlers[typeid(EventType)].push_back([func = std::forward<Func>(func)](std::shared_ptr<Event> evt) {
                func(*evt);
                });
        }

        template<typename EventType, typename Func>
        void Unsubscribe(Func&& func) {
            std::lock_guard<std::mutex> lock(mutex);
            auto& handlersList = handlers[typeid(EventType)];
            handlersList.erase(std::remove_if(handlersList.begin(), handlersList.end(),
                [&func](const std::function<void(std::shared_ptr<Event>)>& handler) {
                    // Here we assume that we can compare handlers directly, which is a simplification.
                    // In practice, you may need to store a unique identifier for each subscription to enable precise unsubscription.
                    return handler.target<Func>() == func.target<Func>();
                }), handlersList.end());
        }

        void ProcessEvents() {
            std::unique_lock<std::mutex> lock(mutex);
            for (auto& [type, queue] : m_Events) {
                std::vector<std::function<void()>> eventBatch;
                while (!queue.empty() && queue.front().frameQueued < m_CurrentFrame) {
                    auto event = queue.front().event;
                    queue.pop();
                    for (auto& handler : handlers[type]) {
                        eventBatch.push_back([handler, event]() { handler(event); });
                    }
                }
                if (!eventBatch.empty()) {
                    PeachCore::ThreadPoolManager::ThreadPool().EnqueueEventBatch(eventBatch);
                }
            }
        }

        void IncrementFrame() {
            std::lock_guard<std::mutex> lock(mutex);
            m_CurrentFrame++;
        }

    public:
        struct TimedEvent {
            std::shared_ptr<Event> event;
            unsigned long int frameQueued;
        };

        std::map<std::type_index, std::queue<TimedEvent>> m_Events;
        std::map<std::type_index, std::vector<std::function<void(std::shared_ptr<Event>)>>> handlers;
        std::mutex mutex;
        unsigned long int m_CurrentFrame = 0; // Tracks global frame count for 'game' runtime
    };

}
