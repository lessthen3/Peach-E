#pragma once
#include <vector>
#include <functional>
#include <map>
#include <tuple>

namespace PeachCore {

    class ScheduleManager {
    public:
        static ScheduleManager& Schedule() {
            static ScheduleManager instance;
            return instance;
        }

       
    private:
        ScheduleManager() = default;

    };

}





/////////////////////////////////////////// TODO(maybe):
        /////////////////// add a 2nd frame ready function for caching variables so that all initailization stuff can be done, actually wait, we can just make a function called
        /////////////////// Initialize() which is called first for every system. then LateInitialize() for variable caching at he beginning of a scene if needed.