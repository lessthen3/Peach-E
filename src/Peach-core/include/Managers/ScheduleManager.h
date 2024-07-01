#pragma once
#include <vector>
#include <functional>
#include <map>
#include <entt/entt.hpp>

namespace PeachCore {

    class ScheduleManager 
    {
    public:
        static ScheduleManager& Schedule() {
            static ScheduleManager instance;
            return instance;
        }

        // Adding a system with a priority
        void AddContinuousSystem(std::function<void(entt::registry&, float)> system, int priority) {
            m_Systems.emplace_back(priority, system);
            std::sort(m_Systems.begin(), m_Systems.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
                });
        }

        // Adding a system with a priority
        void AddStaticSystem(std::function<void(entt::registry&, float)> system, int priority) {
            m_Systems.emplace_back(priority, system);
            std::sort(m_Systems.begin(), m_Systems.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
                });
        }

        // Executing all systems
        void UpdateSystems(entt::registry& registry, float deltaTime) {
            for (const auto& [priority, system] : m_Systems) {
                system(registry, deltaTime);
            }
        }


        /////////////////////////////////////////// TODO:
        /////////////////// add a 2nd frame ready function for caching variables so that all initailization stuff can be done, actually wait, we can just make a function called
        /////////////////// Initialize() which is called first for every system. then LateInitialize() for variable caching at he beginning of a scene if needed.
    private:
        ScheduleManager() = default;
        std::vector<std::pair<int, std::function<void(entt::registry&, float)>>> m_Systems;
    };
}