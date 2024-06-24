#pragma once
class ScheduleManager
{
public:
    static ScheduleManager& Schedule() {
        static ScheduleManager instance;
        return instance;
    }

private:
    ScheduleManager() = default;
    ~ScheduleManager() = default;

    ScheduleManager(const ScheduleManager&) = delete;
    ScheduleManager& operator=(const ScheduleManager&) = delete;
};

