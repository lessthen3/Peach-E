#pragma once
class EntityManager
{
public:
    static EntityManager& Entity() {
        static EntityManager instance;
        return instance;
    }

private:
    EntityManager() = default;
    ~EntityManager() = default;

    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
};

