#pragma once

#include <entt/entt.hpp>
#include "RegistryManager.h"

namespace PeachCore {
    
    struct EntityManager
    {
    public:
        static EntityManager& Entity() {
            static EntityManager instance;
            return instance;
        }

    public:
        static entt::entity CreateEntity();

        static void DestroyEntity(const entt::entity& fp_Entity);

        template<typename T, typename... Args>
        static T& PatchComponent(const entt::entity& fp_Entity, Args&&... args);

        template<typename T, typename... Args>
        static T& AddComponent(const entt::entity& fp_Entity, Args&&... args );

        template<typename T>
        static void RemoveComponent(const entt::entity& fp_Entity);

        template<typename T>
        T& GetComponent(const entt::entity& fp_Entity) const;


    private:
        EntityManager() = default; //no private destructor, pybind doesnt like that >w<

        EntityManager(const EntityManager&) = delete;
        EntityManager& operator=(const EntityManager&) = delete;
    };
}