#pragma once
#include <vector>
#include <functional>
#include <map>
#include <tuple>
#include <entt/entt.hpp>

namespace PeachCore {

    class ScheduleManager {
    public:
        static ScheduleManager& Schedule() {
            static ScheduleManager instance;
            return instance;
        }

        // Adding a continuous system with a priority and SYSTEM_ID
        void AddContinuousSystem(std::function<void(entt::registry&, float)> system, int priority, int systemID) {
            m_ContinuousSystems.emplace_back(priority, system, systemID, true);
            std::sort(m_ContinuousSystems.begin(), m_ContinuousSystems.end(), [](const auto& a, const auto& b) {
                return std::get<0>(a) < std::get<0>(b);
                });
        }

        // Adding a static system with a priority and SYSTEM_ID
        void AddStaticSystem(std::function<void(entt::registry&, float)> system, int priority, int systemID) {
            m_StaticSystems.emplace_back(priority, system, systemID);
            std::sort(m_StaticSystems.begin(), m_StaticSystems.end(), [](const auto& a, const auto& b) {
                return std::get<0>(a) < std::get<0>(b);
                });
        }

        // Executing all continuous systems
        void UpdateContinuousSystems(entt::registry& registry, float deltaTime) {
            for (auto& [priority, system, systemID, isActive] : m_ContinuousSystems) {
                if (isActive) {
                    system(registry, deltaTime);
                }
            }
        }

        // Executing all static systems
        void UpdateStaticSystems(entt::registry& registry, float deltaTime) {
            for (const auto& [priority, system, systemID] : m_StaticSystems) {
                system(registry, deltaTime);
            }
        }

        // Set a continuous system's active status
        void SetContinuousSystemActivity(int systemID, bool isActive) {
            for (auto& [priority, system, id, active] : m_ContinuousSystems) {
                if (id == systemID) {
                    active = isActive;
                    break;
                }
            }
        }

        // Run a specific static system on demand
        void RunStaticSystemNow(int systemID, entt::registry& registry, float deltaTime) {
            for (const auto& [priority, system, id] : m_StaticSystems) {
                if (id == systemID) {
                    system(registry, deltaTime);
                    break;
                }
            }
        }

        // Remove a system
        void RemoveContinuousSystem(int systemID) {
            m_ContinuousSystems.erase(
                std::remove_if(m_ContinuousSystems.begin(), m_ContinuousSystems.end(),
                    [systemID](const auto& system) { return std::get<2>(system) == systemID; }),
                m_ContinuousSystems.end()
            );
        }

        void RemoveStaticSystem(int systemID) {
            m_StaticSystems.erase(
                std::remove_if(m_StaticSystems.begin(), m_StaticSystems.end(),
                    [systemID](const auto& system) { return std::get<2>(system) == systemID; }),
                m_StaticSystems.end()
            );
        }

    private:
        ScheduleManager() = default;

        std::vector<std::tuple<int, std::function<void(entt::registry&, float)>, int, bool>> m_ContinuousSystems; // priority, system, systemID, isActive
        std::vector<std::tuple<int, std::function<void(entt::registry&, float)>, int>> m_StaticSystems; // priority, system, systemID
    };

}





/////////////////////////////////////////// TODO(maybe):
        /////////////////// add a 2nd frame ready function for caching variables so that all initailization stuff can be done, actually wait, we can just make a function called
        /////////////////// Initialize() which is called first for every system. then LateInitialize() for variable caching at he beginning of a scene if needed.