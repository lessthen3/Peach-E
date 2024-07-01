#include "../../include/Managers/EntityManager.h"

namespace PeachCore {

    entt::entity EntityManager::CreateEntity() {
        return RegistryManager::Registry().GetRegistry().create();
    }

    void EntityManager::DestroyEntity(const entt::entity& entity) {
        RegistryManager::Registry().GetRegistry().destroy(entity);
    }

    template<typename T, typename... Args>
    T& EntityManager::AddComponent(const entt::entity& entity, Args&&... args) {
        return RegistryManager::Registry().GetRegistry().emplace<T>(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    T& EntityManager::GetComponent(const entt::entity& entity) const {
        return RegistryManager::Registry().GetRegistry().get<T>(entity);
    }

    template<typename T>
    static void EntityManager::RemoveComponent(const entt::entity& entity) {
        RegistryManager::Registry().GetRegistry().remove<T>(entity);
    }

    // If you need to update a component often, consider this method
    template<typename T, typename... Args>
    T& EntityManager::PatchComponent(const entt::entity& entity, Args&&... args) {
        auto& registry = RegistryManager::Registry().GetRegistry();
        if (!registry.any_of<T>(entity)) {
            return registry.emplace<T>(entity, std::forward<Args>(args)...);
        }
        return registry.replace<T>(entity, std::forward<Args>(args)...);
    }
}