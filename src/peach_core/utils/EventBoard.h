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

#include "debug/Logger.h"

#include <queue>
#include <memory>
#include <typeindex>
#include <functional>

/*
    This class is primarily used for tying an event system from a language runtime into peachy's C++ internals, and also works for
    inter language communication if developers want to use multiple languages to write their game uwu owo
*/

namespace PeachCore {

    template<typename Event>
    struct EventBoard 
    {
    public:
        static EventBoard& get_single() 
        {
            static EventBoard<Event> event_board;
            return event_board;
        }

    public:

        // Post an event and handle it immediately
        template<typename EventType>
        void 
            PostEvent(const EventType& event)
        {
            static_assert(is_base_of<Event, EventType>::value, "EventType must derive from Event");

            shared_ptr<EventType> sharedEvent = make_shared<EventType>(event);

            auto& handlerList = handlers[typeid(EventType)];

            for (auto& handler : handlerList) 
            {
                handler(sharedEvent);
            }
        }

        // Subscribe to an event type with a function
        template<typename EventType, typename Func>
        void 
            Subscribe(Func&& func) 
        {
            handlers[typeid(EventType)].push_back
            (
                [func = forward<Func>(func)](shared_ptr<Event> evt) 
                {
                    func(*static_pointer_cast<EventType>(evt));
                }
            );
        }

        // Unsubscribe a handler (by function address, simplified version)
        template<typename EventType, typename Func>
        void 
            Unsubscribe(Func&& func) 
        {
            auto& handlersList = handlers[typeid(EventType)];

            handlersList.erase
            (
                remove_if
                (
                    handlersList.begin(), 
                    handlersList.end(),
                    [&func](const auto& handler)
                    {
                        //return handler.target<Func>() == func.target<Func>(); NOT SURE WHY THIS IS RETURNING SMTH LMFAO, future ryan: cause it's a lambda it's not returning thru the main function owo ur cute o7
                    }
                ), 
                handlersList.end()
            );
        }

        void 
            IncrementFrame()
        {
            m_CurrentFrame++;
        }

    public:
        struct TimedEvent
        {
            shared_ptr<Event> event;
            uint64_t frameQueued;
        };

        unordered_map<type_index, queue<TimedEvent>> m_Events;
        unordered_map<type_index, vector<function<void(shared_ptr<Event>)>>> handlers;
        uint64_t m_CurrentFrame = 0; // Tracks global frame count for 'game' runtime
    };

}
