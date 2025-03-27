#pragma once


//WE WANT THIS CLASS TO PARSE THROUGH LOGS AND PROVIDE TOOLS TO INTROSPECT INTO THE ENGINE AND ITS ACTIVITIES

//THE SIMPLEST WAY I CAN THINK OF DOING THIS IS TO PROVIDE A WAY TO QUERY LOGS EFFICIENTLY AND POSSIBLY OTHER USEFUL DEBUGGING TOOLS

//FOR CREATING SANDBOX ENVIRONMENTS AND ISOLATING CERTAIN PARTS OF THE EXECUTION

namespace PeachEditor {

    class Debugger {
    public:
        static Debugger& DebugManager() 
        {
            static Debugger debugmanager;
            return debugmanager;
        }

    private:
        Debugger() = default;

        Debugger(const Debugger&) = delete;
        Debugger& operator=(const Debugger&) = delete;
    };

}