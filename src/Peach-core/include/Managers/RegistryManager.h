#pragma once

#include <string>

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

        std::string CreateEntity(int systemID);

        void DestroyEntity(const std::string& fp_Entity, int fp_CurrentSystemID);

        void TransferOwnership(const std::string& fp_Entity, int fp_CurrentOwnerID, int fp_FutureOwnerID);
        void ReleaseEntity(const std::string& fp_Entity, int fp_CurrentOwnerID);

        template<typename T, typename... Args>
        T& PatchComponent(const std::string& fp_Entity, int fp_SystemID, Args&&... args);

        template<typename T, typename... Args>
        T& AddComponent(const std::string& fp_Entity, int fp_SystemID, Args&&... args);

        template<typename T>
        void RemoveComponent(const std::string& fp_Entity, int fp_SystemID);

        template<typename T>
        T& GetComponent(const std::string& fp_Entity, int fp_SystemID);

        template<typename T>
        const T& GetComponentReadOnly(const std::string& fp_Entity) const;

    private:
        RegistryManager() = default;

        RegistryManager(const RegistryManager&) = delete;
        RegistryManager& operator=(const RegistryManager&) = delete;

        entt::registry pm_Registry;
        std::unordered_map<uint32_t, int> pm_EntitySystemMap;  // Map to store entity SYSTEM_IDs
        std::vector<uint32_t> pm_FreeEntities;
    };

    // Component management
    template<typename T, typename... Args>
    T& RegistryManager::AddComponent(const std::string& fp_Entity, int fp_SystemID, Args&&... args) {
        if (pm_EntitySystemMap.at(fp_Entity) != fp_SystemID) {
            throw std::runtime_error("System ID mismatch when adding component.");
        }
        return pm_Registry.emplace<T>(fp_Entity, std::forward<Args>(args)...);
    }

    template<typename T>
    const T& RegistryManager::GetComponentReadOnly(const std::string& fp_Entity) const {
        return pm_Registry.get<T>(fp_Entity);
    }

    template<typename T>
    T& RegistryManager::GetComponent(const std::string& fp_Entity, int fp_SystemID) {
        if (pm_EntitySystemMap.at(fp_Entity) != fp_SystemID) {
            throw std::runtime_error("System ID mismatch when accessing component.");

        }
        return pm_Registry.get<T>(fp_Entity);
    }

    template<typename T>
    void RegistryManager::RemoveComponent(const std::string& fp_Entity, int fp_SystemID) {
        if (pm_EntitySystemMap.at(fp_Entity) != fp_SystemID) {
            throw std::runtime_error("System ID mismatch when removing component.");
        }
        pm_Registry.remove<T>(fp_Entity);
    }

    // Patch a component
    template<typename T, typename... Args>
    T& RegistryManager::PatchComponent(const std::string& fp_Entity, int fp_SystemID, Args&&... args) {
        if (pm_EntitySystemMap.at(fp_Entity) != fp_SystemID) {
            throw std::runtime_error("System ID mismatch when patching component.");
        }
        if (!pm_Registry.any_of<T>(fp_Entity)) {
            return pm_Registry.emplace<T>(fp_Entity, std::forward<Args>(args)...);
        }
        return pm_Registry.replace<T>(fp_Entity, std::forward<Args>(args)...);
    }
}
