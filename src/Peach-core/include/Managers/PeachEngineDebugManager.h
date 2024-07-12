#pragma once


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