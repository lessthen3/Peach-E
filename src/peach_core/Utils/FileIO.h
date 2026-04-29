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
#include <filesystem>
#include <span>
#include <optional>

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


[[nodiscard]] bool 
    WriteAtomic
    (
        const filesystem::path& fp_Path,
        std::span<const std::byte> fp_Data
    );

// Append-only logs, where partial-write-survives-crash is desirable
[[nodiscard]] bool 
    AppendLine
    (
        FILE* fp_File, 
        std::string_view fp_Line
    );

// Read with bounded size — refuses to read >max_bytes to prevent OOM on corrupted size headers
[[nodiscard]] std::optional<std::vector<std::byte>>
    ReadBounded
    (
        const filesystem::path& fp_Path, 
        size_t fp_MaxBytes
    );

// Existence check that doesn't throw on permission errors
[[nodiscard]] bool 
    Exists
    (
        const filesystem::path& fp_Path
    ) 
    noexcept;

// Atomic replace of one file with another (advanced — for editor "save as" workflows)
[[nodiscard]] bool 
    ReplaceAtomic
    (
        const filesystem::path& fp_From,
        const filesystem::path& fp_To
    );

}//namespace PeachCore::FileIO