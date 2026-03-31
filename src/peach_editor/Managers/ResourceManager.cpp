/*******************************************************************
 *                     Peach Editor v0.0.7
 *             Created by Ranyodh Mandur - 🍑 2024
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#include "ResourceManager.h"

#include <fmt/format.h>

namespace PeachEditor{

    bool
        ResourceManager::WriteSPIRVToFile
        (
            const vector<uint32_t>& fp_SpirvBytecode,
            const string& fp_DesiredOutputDirectory,
            const string& fp_DesiredName,
            PeachCore::Logger* logger
        )
    {
        //always check for nullptrs kids >O<
        if (not logger)
        {
            PRINT_ERROR("ShaderCompilerUtils Error: Tried to pass nullptr reference to logger during WriteSPIRVToFile()");
            return false;
        }
        // Ensure directory exists
        else if (not filesystem::exists(fp_DesiredOutputDirectory))
        {
            logger->Error(fmt::format("ShaderCompilerUtils Error: Tried to pass invalid write directory: '{}' to WriteSPIRVToFile()", fp_DesiredOutputDirectory), "ShaderCompilerUtils");
            return false;
        }
        else if (fp_SpirvBytecode.empty()) //check if the byte vector is empty uwu
        {
            logger->Error(fmt::format("ShaderCompilerUtils Error: Tried passing empty byte vector for writing to file name: '{}', nothing was done.", fp_DesiredName), "ShaderCompilerUtils");
            return false;
        }

        string f_FileName;

        if (fp_DesiredOutputDirectory == "./")
        {
            f_FileName = "./" + fp_DesiredName + ".spv";
        }
        else
        {
            f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName + ".spv";
        }

        ofstream file(f_FileName, ios::binary);  // Open in binary mode

        if (not file.is_open())
        {
            logger->Error(fmt::format("ShaderCompilerUtils Error: Failed to open file: '{}' for writing.", f_FileName), "ShaderCompilerUtils");
            return false;
        }

        //now we can confidently write the bytecode into a file knowing its open, the vector is filled and the directory exists
        file.write(reinterpret_cast<const char*>(fp_SpirvBytecode.data()), fp_SpirvBytecode.size() * sizeof(uint32_t));

        return true;
    }

    bool
        ResourceManager::LoadRawShaderSource
        (
            string* fp_SourceCode,
            const string& fp_ShaderSourcePath,
            PeachCore::Logger* logger
        )
    {
        //check for nullptr for logger ref
        if (not logger)
        {
            PRINT_ERROR("ShaderCompilerUtils Error: Tried to pass nullptr reference to logger during LoadRawShaderSource()");
            return false;
        }
        //more nullptr checking
        else if (not fp_SourceCode)
        {
            logger->Error("ShaderCompilerUtils Error: Nullptr string reference passed to LoadRawShaderSource()", "ShaderCompilerUtils");
            return false;
        }
        // Ensure directory exists
        else if (not filesystem::exists(fp_ShaderSourcePath))
        {
            logger->Error("ShaderCompilerUtils Error: Tried to pass invalid filepath to LoadRawShaderSource()", "ShaderCompilerUtils");
            return false;
        }

        // Extract file extension assuming fmt::format "filename.ext"
        size_t lastDotIndex = fp_ShaderSourcePath.rfind('.');

        if (lastDotIndex == string::npos)
        {
            logger->Error(fmt::format("ShaderCompilerUtils Error: No file extension found at filepath: '{}'", fp_ShaderSourcePath), "ShaderCompilerUtils");
            return false;
        }

        string f_FileExtension = fp_ShaderSourcePath.substr(lastDotIndex);

        if (
            f_FileExtension != ".fs" and
            f_FileExtension != ".vs" and
            f_FileExtension != ".glsl" and
            f_FileExtension != ".vert" and
            f_FileExtension != ".frag"
            )
        {
            logger->Error(fmt::format("Found file extension: '{}', when GLSL Shader was expected at specified filepath: '{}'", f_FileExtension, fp_ShaderSourcePath), "ShaderCompilerUtils");
            return false;
        }

        ifstream f_ShaderFile(fp_ShaderSourcePath, ios::in);

        if (not f_ShaderFile.is_open())
        {
            logger->Error(fmt::format("ShaderCompilerUtils Error: Failed to open shader at filepath: '{}', for reading.", fp_ShaderSourcePath), "ShaderCompilerUtils");
            return false;
        }

        stringstream f_TempStringBuffer;
        f_TempStringBuffer << f_ShaderFile.rdbuf();
        *fp_SourceCode = f_TempStringBuffer.str();

        return true;
    }


    
}