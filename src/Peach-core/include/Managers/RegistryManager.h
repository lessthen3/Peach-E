#pragma once

#include <string>
#include <entt/entt.hpp>

#include <unordered_map>
#include <stdexcept>

namespace PeachCore {

    class RegistryManager {
    public:
        static RegistryManager& Registry() {
            static RegistryManager instance;
            return instance;
        }

        entt::registry& GetRegistry();

        entt::entity CreateEntity(int systemID);

        void DestroyEntity(const entt::entity& fp_Entity);

        template<typename T, typename... Args>
        T& PatchComponent(const entt::entity& entity, int systemID, Args&&... args);

        template<typename T, typename... Args>
        T& AddComponent(const entt::entity& entity, int systemID, Args&&... args);

        template<typename T>
        void RemoveComponent(const entt::entity& entity, int systemID);

        template<typename T>
        T& GetComponent(const entt::entity& entity, int systemID);

        template<typename T>
        const T& GetComponentReadOnly(const entt::entity& entity) const;

    private:
        RegistryManager() = default;

        RegistryManager(const RegistryManager&) = delete;
        RegistryManager& operator=(const RegistryManager&) = delete;

        entt::registry registry;
        std::unordered_map<entt::entity, int> entitySystemMap;  // Map to store entity SYSTEM_IDs
    };

    // Component management
    template<typename T, typename... Args>
    T& RegistryManager::AddComponent(const entt::entity& entity, int systemID, Args&&... args) {
        if (entitySystemMap.at(entity) != systemID) {
            throw std::runtime_error("System ID mismatch when adding component.");
        }
        return registry.emplace<T>(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    const T& RegistryManager::GetComponentReadOnly(const entt::entity& entity) const {
        return registry.get<T>(entity);
    }

    template<typename T>
    T& RegistryManager::GetComponent(const entt::entity& entity, int systemID) {
        if (entitySystemMap.at(entity) != systemID) {
            throw std::runtime_error("System ID mismatch when accessing component.");

        }
        return registry.get<T>(entity);
    }

    template<typename T>
    void RegistryManager::RemoveComponent(const entt::entity& entity, int systemID) {
        if (entitySystemMap.at(entity) != systemID) {
            throw std::runtime_error("System ID mismatch when removing component.");
        }
        registry.remove<T>(entity);
    }

    // Patch a component
    template<typename T, typename... Args>
    T& RegistryManager::PatchComponent(const entt::entity& entity, int systemID, Args&&... args) {
        if (entitySystemMap.at(entity) != systemID) {
            throw std::runtime_error("System ID mismatch when patching component.");
        }
        if (!registry.any_of<T>(entity)) {
            return registry.emplace<T>(entity, std::forward<Args>(args)...);
        }
        return registry.replace<T>(entity, std::forward<Args>(args)...);
    }
}
