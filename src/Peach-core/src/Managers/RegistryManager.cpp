#include "../../include/Managers/RegistryManager.h"

namespace PeachCore
{
    entt::registry& RegistryManager::GetRegistry() {
        return registry;
    }

    // Entity management
    entt::entity RegistryManager::CreateEntity(int systemID) {
        entt::entity entity = registry.create();
        entitySystemMap[entity] = systemID;
        return entity;
    }

    void RegistryManager::DestroyEntity(const entt::entity& entity) {
        entitySystemMap.erase(entity);
        registry.destroy(entity);
    }

    

}