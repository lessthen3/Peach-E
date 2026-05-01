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

///PeachCore
#include "FileIO.h"

#ifdef PEACH_PLATFORM_WINDOWS
#   include <windows.h>
#   include <io.h>
#else
#   include <unistd.h>
#   include <fcntl.h>
#endif

#include <fstream>

namespace PeachCore::FileIO
{
    //////////////////////////////////////////////
    // Binary File Read/Write Functions
    //////////////////////////////////////////////

    bool
        WriteToBinary
        (
            const string& fp_DesiredOutputDirectory,
            const string& fp_DesiredName,
            const vector<uint8_t>& fp_Binary,
            Logger*const logger
        )
    {
        ////////////////////////////////////////////// Logger nullptr Safety Check //////////////////////////////////////////////

        if (not logger)
        {
            PEACH_PRINT_ERROR("Serialization Error: Tried to pass nullptr reference to logger during WriteToBinary()");
            return false;
        }

        ////////////////////////////////////////////// Ensure Directory Exists //////////////////////////////////////////////

        if (not filesystem::exists(fp_DesiredOutputDirectory))
        {
            logger->Error(fmt::format("Serialization Error: Tried to pass invalid write directory: '{}' to WriteToBinary", fp_DesiredOutputDirectory), "Serializer");
            return false;
        }

        ////////////////////////////////////////////// Make Sure fp_Binary is Not Empty //////////////////////////////////////////////

        if (fp_Binary.empty()) //check if the byte vector is empty uwu
        {
            logger->Error(fmt::format("Serialization Error: Tried passing empty byte vector for writing to file name: '{}', nothing was done.", fp_DesiredName), "Serializer");
            return false;
        }

        string f_FileName;

        if (fp_DesiredOutputDirectory == "./")
        {
            f_FileName = "./" + fp_DesiredName;
        }
        else
        {
            f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName;
        }

        ofstream file(f_FileName, ios::binary);  // Open in regular string mode

        if (not file)
        {
            logger->Error(fmt::format("Serialization Error: Failed to open file: '{}' for writing.", f_FileName), "Serializer");
            return false;
        }

        // Write the entire contents of the vector -> peach binary
        file.write(reinterpret_cast<const char*>(fp_Binary.data()), fp_Binary.size());
        // Close the file
        file.close();

        return true; //success! wrote byte vector -> peach binary
    }

    bool
        ReadBinaryIntoVector //i think this'll work lmfao
        (
            const string& fp_ScriptFilePath,
            const vector<string>& fp_Extensions,
            vector<uint8_t>& fp_Binary,
            Logger*const logger
        )
    {
        ////////////////////////////////////////////// Logger nullptr Safety Check //////////////////////////////////////////////

        if (not logger) //check for nullptr ref passed to ReadBinaryIntoVector
        {
            PEACH_PRINT_ERROR("Serialization Error: Tried to pass nullptr reference to logger during ReadBinaryIntoVector()");
            return false;
        }

        // Ensure directory exists
        if (not filesystem::exists(fp_ScriptFilePath))
        {
            logger->Error("Serialization Error: Tried to pass invalid directory to ReadBinaryIntoVector()", "Serializer");
            return false;
        }

        if (not fp_Binary.empty()) //check if the byte vector is empty before reading data into it OwO
        {
            logger->Error(fmt::format("Serialization Error: Tried passing non-empty byte vector for reading to file name: '{}', nothing was done.", fp_ScriptFilePath), "Serializer");
            return false;
        }

        // Extract file extension assuming fmt::format "filename.ext"
        size_t f_LastDotIndex = fp_ScriptFilePath.rfind('.');

        if (f_LastDotIndex == string::npos)
        {
            logger->Error("Serialization Error: No file extension found for Peach-E Binary", "Serializer");
            return false;
        }

        string f_FileExtension = fp_ScriptFilePath.substr(f_LastDotIndex);

        bool f_IsValidExtension = false;

        for (const string& lv_ExtensionName : fp_Extensions)
        {
            if (f_FileExtension == lv_ExtensionName) //file extension for peach-e binary encoding, get it? it's like a bin of peaches >w<
            {
                f_IsValidExtension = true;
                break;
            }
        }

        if (not f_IsValidExtension)
        {
            logger->Error(fmt::format("Serialization Error: Attempted to read from an unknown binary extension: '{}'", f_FileExtension), "Serializer");
            return false;
        }

        ifstream f_BinaryStream(fp_ScriptFilePath, ios::binary); //open in binary mode

        if (not f_BinaryStream) //check if the file opened properly
        {
            logger->Error(fmt::format("Serialization Error: Failed to open binary for reading: '{}'", fp_ScriptFilePath), "Serializer");
            return false;
        }

        // Get the size of the file
        f_BinaryStream.seekg(0, ios::end);
        size_t f_Size = f_BinaryStream.tellg();
        f_BinaryStream.seekg(0, ios::beg);

        // Resize the vector to the size of the file
        fp_Binary.resize(f_Size);

        // Read the entire file into the vector
        f_BinaryStream.read(reinterpret_cast<char*>(fp_Binary.data()), f_Size);
        // Close the file
        f_BinaryStream.close();

        return true; //success! file read into vector
    }

    //////////////////////////////////////////////
    // JSON File Read/Write Functions
    //////////////////////////////////////////////

    bool
        WriteStringToFile
        (
            const string& fp_DesiredOutputDirectory,
            const string& fp_DesiredName,
            const string& fp_FileString,
            Logger*const logger
        )
    {
        if (not logger)
        {
            PEACH_PRINT_ERROR("Serialization Error: Tried to pass nullptr reference to logger during WriteToJSON()");
            return false;
        }

        // Ensure directory exists
        if (not filesystem::exists(fp_DesiredOutputDirectory))
        {
            logger->Error("Serialization Error: Tried to pass invalid write directory to WriteToJSON", "Serializer");
            return false;
        }

        string f_FileName;

        if (fp_DesiredOutputDirectory == "./")
        {
            f_FileName = "./" + fp_DesiredName;
        }
        else
        {
            f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName;
        }

        ofstream f_OpenedFile(f_FileName, ios::out);  // Open in regular string mode

        if (not f_OpenedFile)
        {
            logger->Error(fmt::format("Serialization Error: Failed to open file: '{}' for writing.", f_FileName), "Serializer");
            return false;
        }

        // Write JSON string -> .json file
        f_OpenedFile.write(fp_FileString.c_str(), fp_FileString.size());
        // Close the file
        f_OpenedFile.close();

        return true;
    }

    bool
        ReadFileIntoCharBuffer
        (
            const string& fp_ScriptFilePath,
            const vector<string>& fp_Extensions,
            vector<char>& fp_CharBuffer,
            Logger*const logger
        )
    {
        ////////////////////////////////////////////// Logger nullptr Safety Check //////////////////////////////////////////////

        if (not logger)
        {
            PEACH_PRINT_ERROR("Serialization Error: Tried to pass nullptr reference to logger during ReadFileIntoCharBuffer()");
            return false;
        }

        ////////////////////////////////////////////// Ensure directory exists //////////////////////////////////////////////

        if (not filesystem::exists(fp_ScriptFilePath))
        {
            logger->Error("Serialization Error: Tried to pass invalid filepath to ReadFileIntoCharBuffer()", "Serializer");
            return false;
        }

        ////////////////////////////////////////////// Extract file extension assuming fmt::format "filename.ext" //////////////////////////////////////////////

        size_t f_LastDotIndex = fp_ScriptFilePath.rfind('.');

        if (f_LastDotIndex == string::npos)
        {
            logger->Error("Serialization Error: No file extension found", "Serializer");
            return false;
        }

        string f_FileExtension = fp_ScriptFilePath.substr(f_LastDotIndex);

        bool f_IsValidExtension = false;

        for (const string& lv_ExtensionName : fp_Extensions)
        {
            if (f_FileExtension == lv_ExtensionName) //file extension for peach-e binary encoding, get it? it's like a bin of peaches >w<
            {
                f_IsValidExtension = true;
                break;
            }
        }

        if (not f_IsValidExtension)
        {
            logger->Error(fmt::format("Serialization Error: Attempted to read from an unknown text file extension: '{}'", f_FileExtension), "Serializer");
            return false;
        }

        ifstream f_FileStream(fp_ScriptFilePath, ios::in | ios::binary);

        if (not f_FileStream)
        {
            logger->Error(fmt::format("Serialization Error: Failed to open '{}' for reading.", fp_ScriptFilePath), "Serializer");
            return false;
        }

        ////////////////////////////////////////////// Get File Size //////////////////////////////////////////////

        f_FileStream.seekg(0, ios::end);
        streampos f_FileSize = f_FileStream.tellg();
        f_FileStream.seekg(0, ios::beg);

        ////////////////////////////////////////////// Assert File Contains Data //////////////////////////////////////////////

        if (f_FileSize <= 0) //Treat empty files as an error since user thinks the file has something otherwise they wouldn't have tried to read from it UwU!
        {
            logger->Error("Serialization Error: Tried to pass empty file to ReadJSONIntoString()", "Serializer");
            fp_CharBuffer.clear();
            return false;
        }

        ////////////////////////////////////////////// Store Data -> fp_CharBuffer //////////////////////////////////////////////

        fp_CharBuffer.resize(static_cast<size_t>(f_FileSize));
        f_FileStream.read(fp_CharBuffer.data(), f_FileSize);

        f_FileStream.close();

        ////////////////////////////////////////////// Success! //////////////////////////////////////////////

        return true;
    }
    
}//namespace PeachCore::FileIO