#pragma once


//WE WANT THIS CLASS TO PARSE THROUGH LOGS AND PROVIDE TOOLS TO INTROSPECT INTO THE ENGINE AND ITS ACTIVITIES

//THE SIMPLEST WAY I CAN THINK OF DOING THIS IS TO PROVIDE A WAY TO QUERY LOGS EFFICIENTLY AND POSSIBLY OTHER USEFUL DEBUGGING TOOLS

//FOR CREATING SANDBOX ENVIRONMENTS AND ISOLATING CERTAIN PARTS OF THE EXECUTION

namespace PeachCore {

    class PeachEngineDebugManager {
    public:
        static PeachEngineDebugManager& DebugManager() {
            static PeachEngineDebugManager debugmanager;
            return debugmanager;
        }

    private:
        PeachEngineDebugManager() = default;

        PeachEngineDebugManager(const PeachEngineDebugManager&) = delete;
        PeachEngineDebugManager& operator=(const PeachEngineDebugManager&) = delete;
    };

}