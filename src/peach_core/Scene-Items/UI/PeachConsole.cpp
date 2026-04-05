/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#include "PeachConsole.h"

#include <fmt/format.h>

namespace PeachCore::PUI {

    //PeachConsole::PeachConsole() 
    //{
    //    //pm_PeachLogConsole = make_shared<Console>();
    //}

    Logger*
        CreateLogger
        (
            const string& fp_LoggerName, 
            PEACH_LOGGER_FLAGS fp_LoggerFlags
        )
    {

        return nullptr;
    }

    bool
        PeachConsole::ParseArguments
        (
            const string& fp_CommandString, 
            vector<ParsedArgument>& fp_ParsedArguments
        )
    {
        for (int lv_Index = 0; lv_Index < fp_CommandString.size(); lv_Index++)
        {
            char fv_CurrentChar = fp_CommandString[lv_Index];

            if (fv_CurrentChar != '-')
            {
                PRINT_ERROR(fmt::format("[POORLY FORMED COMMAND]: expected '-' but found '{}' instead", fv_CurrentChar));
                return false;
            }

            string fv_Command = "";

            lv_Index++; // can do this since we do a bounds check immediately after uwu

            while (lv_Index < fp_CommandString.size() && fp_CommandString[lv_Index] != ' ') //either exit at end of string or when theres a space
            {
                fv_Command += fp_CommandString[lv_Index];
                lv_Index++;
            }

            if (fv_Command == "h" || fv_Command == "-help") //will cut the first - and will just be --help or -h uwu
            {
                fp_ParsedArguments.clear(); //make sure empty no commands are allowed w -h or -help
                //DisplayHelp();
                return true; //will just skip the stuff since no commands owo
            }

            if (not pm_CommandList.contains(fv_Command))
            {
                PRINT_ERROR(fmt::format("[INVALID COMMAND]: '{}'", fv_Command));
                return false;
            }

            //command validated, and current index should be -> ' '

            string fv_CommandValue = "";

            lv_Index++; // can do this since we do a bounds check immediately after uwu

            while (lv_Index < fp_CommandString.size() && fp_CommandString[lv_Index] != ' ') //either exit at end of string or when theres a space
            {
                fv_CommandValue += fp_CommandString[lv_Index];
                lv_Index++;
            }

            fp_ParsedArguments.emplace_back(fv_Command, fv_CommandValue);
        }

        return true;
    }

    void
        PeachConsole::GetFormattedHelpString()
    {
        string f_HelpString;

        for (const auto& lv_CurrentCommand : pm_CommandList)
        {
            f_HelpString += lv_CurrentCommand.second.HelpMessage;
            //print to UI ig idk return string to be printed from engine side script uwu
        }
    }

    void
        PeachConsole::AddCommand
        (
            const string& fp_Symbol,
            const string& fp_MetaVariable,
            const string& fp_HelpMessage
        )
    {
        pm_CommandList.try_emplace(fp_Symbol, fp_Symbol, fp_MetaVariable, fp_HelpMessage);
    }


    void 
        PeachConsole::ClearConsole
        (
            const string& fp_DesiredTab
        )
    {

    }
}