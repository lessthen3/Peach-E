#pragma once
#include <map>
#include <vector>
#include <queue>
#include <memory>
#include <typeindex>
#include <functional>

using namespace std;

namespace PeachCore {

    template<typename Event>
    struct EventQueueManager {

    public:
        static EventQueueManager& EventQueue() {
            static EventQueueManager<Event> instance;
            return instance;
        }

    public:

        // Post an event and handle it immediately
        template<typename EventType>
        void PostEvent(const EventType& event)
        {
            static_assert(is_base_of<Event, EventType>::value, "EventType must derive from Event");

            shared_ptr<EventType> sharedEvent = make_shared<EventType>(event);

            auto& handlerList = handlers[typeid(EventType)];
            for (auto& handler : handlerList) {
                handler(sharedEvent);
            }
        }

        // Subscribe to an event type with a function
        template<typename EventType, typename Func>
        void Subscribe(Func&& func) {
            handlers[typeid(EventType)].push_back([func = forward<Func>(func)](shared_ptr<Event> evt) {
                func(*static_pointer_cast<EventType>(evt));
                });
        }

        // Unsubscribe a handler (by function address, simplified version)
        template<typename EventType, typename Func>
        void Unsubscribe(Func&& func) {
            auto& handlersList = handlers[typeid(EventType)];
            handlersList.erase(remove_if(handlersList.begin(), handlersList.end(),
                [&func](const auto& handler) {
                    return handler.target<Func>() == func.target<Func>();
                }), handlersList.end());
        }

        void IncrementFrame() {
            m_CurrentFrame++;
        }

    public:
        struct TimedEvent
        {
            shared_ptr<Event> event;
            unsigned long int frameQueued;
        };

        map<type_index, queue<TimedEvent>> m_Events;
        map<type_index, vector<function<void(shared_ptr<Event>)>>> handlers;
        unsigned long int m_CurrentFrame = 0; // Tracks global frame count for 'game' runtime
    };

}
