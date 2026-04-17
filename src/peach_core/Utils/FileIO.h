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
#pragma once

#include "Logger.h"
#include <vector>

namespace PeachCore::FileIO
{
//////////////////////////////////////////////
// Binary File Read/Write Functions
//////////////////////////////////////////////

[[nodiscard]] bool
    WriteToBinary
    (
        const string& fp_DesiredOutputDirectory,
        const string& fp_DesiredName,
        const vector<uint8_t>& fp_Binary,
        Logger*const logger
    );

[[nodiscard]] bool
    ReadBinaryIntoVector //i think this'll work lmfao
    (
        const string& fp_ScriptFilePath,
        const vector<string>& fp_Extensions,
        vector<uint8_t>& fp_Binary,
        Logger*const logger
    );

//////////////////////////////////////////////
// JSON File Read/Write Functions
//////////////////////////////////////////////

[[nodiscard]] bool
    WriteStringToFile
    (
        const string& fp_DesiredOutputDirectory,
        const string& fp_DesiredName,
        const string& fp_FileString,
        Logger*const logger
    );

[[nodiscard]] bool
    ReadFileIntoCharBuffer
    (
        const string& fp_ScriptFilePath,
        const vector<string>& fp_Extensions,
        vector<char>& fp_CharBuffer,
        Logger*const logger
    );
}//namespace PeachCore::FileIO