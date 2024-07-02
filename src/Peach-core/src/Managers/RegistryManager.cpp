#include "../../include/Managers/RegistryManager.h"
#include "../../include/Managers/LogManager.h"

namespace PeachCore
{
    entt::registry& RegistryManager::GetRegistry() {
        return pm_Registry;
    }

    // Entity management
    entt::entity RegistryManager::CreateEntity(int fp_SystemID) {
        entt::entity f_Entity = pm_Registry.create();
        pm_EntitySystemMap[f_Entity] = fp_SystemID;
        return f_Entity;
    }

    void RegistryManager::DestroyEntity(const entt::entity& fp_Entity, int fp_CurrentSystemID) {
        if (pm_EntitySystemMap[fp_Entity] != fp_CurrentSystemID)
        {
            LogManager::Logger().Warn("System Ownership Violation: non-owner system tried to delete entity illegally", "RegistryManager");
            return;
        }
        pm_EntitySystemMap.erase(fp_Entity);
        pm_Registry.destroy(fp_Entity);
    }

    void RegistryManager::TransferOwnership(const entt::entity& fp_Entity, int fp_CurrentSystemID, int fp_NewSystemID) {
        if (pm_EntitySystemMap[fp_Entity] != fp_CurrentSystemID)
        {
            LogManager::Logger().Warn("System Ownership Violation: non-owner system tried to transfer ownership illegally", "RegistryManager");
            return;
        }
        pm_EntitySystemMap[fp_Entity] = fp_NewSystemID;
    }

    void RegistryManager::ReleaseEntity(const entt::entity& fp_Entity, int fp_CurrentSystemID) {
        if (pm_EntitySystemMap[fp_Entity] != fp_CurrentSystemID)
        {
            LogManager::Logger().Warn("System Ownership Violation: non-owner system tried to release ownership illegally", "RegistryManager");
            return;
        }
        pm_EntitySystemMap.erase(fp_Entity);
    }

}