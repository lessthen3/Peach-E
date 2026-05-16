/*******************************************************************
 *                     Peach Editor v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#include "ResourceManagement.h"

#include <fmt/format.h>
#include <lodepng/lodepng.h>

#include <bc7enc_rdo/bc7enc.h>
#include <astcenc.h>

#include <filesystem>
#include <fstream>

namespace PeachEditor::ResourceManagement {

    //////////////////////////////////////////////
    // Internal linkage
    //////////////////////////////////////////////

     // BC7: all desktop GPUs. 4x4 blocks only, always 16 bytes/block, output size = ceil(W/4) * ceil(H/4) * 16
    [[nodiscard]] static bool
        EncodeTextureBC7
        (
            const RawTextureData& fp_TextureData,
            vector<uint8_t>& fp_BinaryStream,
            PeachCore::Logger* logger
        )
    {
        const uint32_t f_W = fp_TextureData.Width;
        const uint32_t f_H = fp_TextureData.Height;

        const uint32_t f_BlocksX = (f_W + 3) / 4;
        const uint32_t f_BlocksY = (f_H + 3) / 4;

        // always cast block counts through uint64_t before multiplying a 16k x 16k texture overflows uint32_t here >O<
        const uint64_t f_OutputBytes = static_cast<uint64_t>(f_BlocksX) * f_BlocksY * 16;

        bc7enc_compress_block_params f_Params;
        bc7enc_compress_block_params_init(&f_Params);
        // f_Params.m_max_partitions_mode1 etc can be tuned later for quality vs speed

        const size_t f_StreamOffset = fp_BinaryStream.size();
        fp_BinaryStream.resize(f_StreamOffset + f_OutputBytes);

        // 4x4 RGBA block = 16 pixels * 4 bytes = 64 bytes — stack is fine here
        uint8_t f_BlockPixels[64];

        for (uint32_t lv_By = 0; lv_By < f_BlocksY; lv_By++)
        {
            for (uint32_t lv_Bx = 0; lv_Bx < f_BlocksX; lv_Bx++)
            {
                for (uint32_t lv_Py = 0; lv_Py < 4; lv_Py++)  // extract 4x4 RGBA block with edge clamping so we don't read OOB on textures where W or H isn't a multiple of 4
                {
                    for (uint32_t lv_Px = 0; lv_Px < 4; lv_Px++)
                    {
                        // clamp to last valid pixel for the edge blocks
                        const uint32_t fv_SrcX = min(lv_Bx * 4 + lv_Px, f_W - 1);
                        const uint32_t fv_SrcY = min(lv_By * 4 + lv_Py, f_H - 1);
                        const uint32_t fv_SrcIdx = (fv_SrcY * f_W + fv_SrcX) * 4;
                        const uint32_t fv_DstIdx = (lv_Py * 4 + lv_Px) * 4;

                        f_BlockPixels[fv_DstIdx + 0] = fp_TextureData.PixelData[fv_SrcIdx + 0];
                        f_BlockPixels[fv_DstIdx + 1] = fp_TextureData.PixelData[fv_SrcIdx + 1];
                        f_BlockPixels[fv_DstIdx + 2] = fp_TextureData.PixelData[fv_SrcIdx + 2];
                        f_BlockPixels[fv_DstIdx + 3] = fp_TextureData.PixelData[fv_SrcIdx + 3];
                    }
                }

                const uint64_t fv_DstOffset = f_StreamOffset + (static_cast<uint64_t>(lv_By) * f_BlocksX + lv_Bx) * 16;

                if (not bc7enc_compress_block(fp_BinaryStream.data() + fv_DstOffset, f_BlockPixels, &f_Params))
                {
                    logger->Error
                    (
                        fmt::format("bc7enc_compress_block failed at block ({}, {})", lv_Bx, lv_By),
                        "ResourceManagement"
                    );

                    fp_BinaryStream.resize(f_StreamOffset); // rollback so stream stays clean

                    return false;
                }
            }
        }

        return true;
    }

    // ASTC: ARM GPU targets — iOS, Android, Apple Silicon.
    // block size is variable (4x4 / 6x6 / 8x8), always 16 bytes/block regardless.
    // bigger block = fewer blocks = smaller file but lower quality.
    
    //need a way to do ASTCENC_PRE_THOROUGH and pass a parent context for a resource thread pool for export time and to have exported games be higher quality UwU
    [[nodiscard]] static bool
        EncodeTextureASTC
        (
            const RawTextureData& fp_TextureData,
            vector<uint8_t>& fp_BinaryStream,
            TargetTextureEncoding fp_Encoding,
            PeachCore::Logger* logger
        )
    {
        uint32_t f_BlockX, f_BlockY;

        switch (fp_Encoding)
        {
        case TargetTextureEncoding::ASTC_4x4:
        {
            f_BlockX = 4;
            f_BlockY = 4;
            break;
        }
        case TargetTextureEncoding::ASTC_6x6:
        {
            f_BlockX = 6;
            f_BlockY = 6;
            break;
        }
        case TargetTextureEncoding::ASTC_8x8:
        {
            f_BlockX = 8;
            f_BlockY = 8;
            break;
        }
        default: // shouldn't be reachable but better loud than silent >O<
        {
            logger->Error("EncodeTextureASTC: hit unreachable encoding variant", "ResourceManagement");
            return false;
        }
        }

        ////////////////////// Build astcenc config //////////////////////

        astcenc_config f_Config;
        const astcenc_error f_ConfigErr = astcenc_config_init
        (
            ASTCENC_PRF_LDR,     // LDR = standard 8-bit colour, not HDR
            f_BlockX, f_BlockY,
            1,                   // depth = 1, we're doing 2D textures
            ASTCENC_PRE_MEDIUM,  // quality preset — bump to THOROUGH for final export if needed
            0,                   // no special flags
            &f_Config
        );

        if (f_ConfigErr != ASTCENC_SUCCESS)
        {
            logger->Error(fmt::format("astcenc_config_init failed: {}", astcenc_get_error_string(f_ConfigErr)), "ResourceManagement");
            return false;
        }

        ////////////////////// Allocate context //////////////////////

        astcenc_context* f_Context = nullptr;
        const astcenc_error f_CtxErr = astcenc_context_alloc(&f_Config, 1, &f_Context, nullptr);

        if (f_CtxErr != ASTCENC_SUCCESS)
        {
            logger->Error(fmt::format("astcenc_context_alloc failed: {}", astcenc_get_error_string(f_CtxErr)), "ResourceManagement");
            return false;
        }

        ////////////////////// Describe input image //////////////////////

        // astcenc_image takes void** for data slices (one per Z layer for 3D textures)
        // we're 2D so one slice — const_cast is safe, astcenc won't write to source pixels
        void* f_DataSlices[1] = { const_cast<uint8_t*>(fp_TextureData.PixelData.data()) }; //ew man why tf void* fuck u

        astcenc_image f_Image;
        f_Image.dim_x = fp_TextureData.Width;
        f_Image.dim_y = fp_TextureData.Height;
        f_Image.dim_z = 1;
        f_Image.data_type = ASTCENC_TYPE_U8;
        f_Image.data = f_DataSlices;

        // RGBA → RGBA, no channel swizzling needed
        const astcenc_swizzle f_Swizzle =
        {
            ASTCENC_SWZ_R,
            ASTCENC_SWZ_G,
            ASTCENC_SWZ_B,
            ASTCENC_SWZ_A
        };

        ////////////////////// Compress //////////////////////

        const uint32_t f_BlocksX = (fp_TextureData.Width + f_BlockX - 1) / f_BlockX;
        const uint32_t f_BlocksY = (fp_TextureData.Height + f_BlockY - 1) / f_BlockY;
        const uint64_t f_OutputBytes = static_cast<uint64_t>(f_BlocksX) * f_BlocksY * 16;

        const size_t f_StreamOffset = fp_BinaryStream.size();
        fp_BinaryStream.resize(f_StreamOffset + f_OutputBytes);

        const astcenc_error f_CompressErr = astcenc_compress_image(
            f_Context,
            &f_Image,
            &f_Swizzle,
            fp_BinaryStream.data() + f_StreamOffset,
            static_cast<size_t>(f_OutputBytes),
            0  // thread index — we're single-threaded here, pass 0
        );

        astcenc_context_free(f_Context); // always free before early return

        if (f_CompressErr != ASTCENC_SUCCESS)
        {
            fp_BinaryStream.resize(f_StreamOffset); // rollback
            logger->Error(
                fmt::format("astcenc_compress_image failed: {}", astcenc_get_error_string(f_CompressErr)),
                "ResourceManagement"
            );
            return false;
        }

        return true;
    }

    //////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////

    bool
        WriteSPIRVToFile
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
            PEACH_PRINT_ERROR("ShaderCompilerUtils Error: Tried to pass nullptr reference to logger during WriteSPIRVToFile()");
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
        LoadRawShaderSource
        (
            string* fp_SourceCode,
            const string& fp_ShaderSourcePath,
            PeachCore::Logger* logger
        )
    {
        //check for nullptr for logger ref
        if (not logger)
        {
            PEACH_PRINT_ERROR("ShaderCompilerUtils Error: Tried to pass nullptr reference to logger during LoadRawShaderSource()");
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


    bool
        LoadTextureFFS
        (
            const string& fp_FilePath,
            RawTextureData& fp_OutTexture,
            PeachCore::Logger* logger
        )
    {
        if (not filesystem::exists(fp_FilePath))
        {
            logger->Error(fmt::format("Tried to pass invalid directory: '{}'", fp_FilePath), "ResourceManagement");
            return false;
        }

        ////////////////////// Decode //////////////////////

        const uint32_t f_ErrorCode = lodepng::decode
        (
            fp_OutTexture.PixelData,
            fp_OutTexture.Width,
            fp_OutTexture.Height,
            fp_FilePath
        );

        if (f_ErrorCode != 0)
        {
            logger->Error(fmt::format("failed to decode texture: '{}', error: '{}'", fp_FilePath, lodepng_error_text(f_ErrorCode)), "ResourceManagement");
            return false;
        }

        fp_OutTexture.SourcePath = fp_FilePath;

        return true;
    }

    bool
        EncodeTexture
        (
            const RawTextureData& fp_TextureData,
            vector<uint8_t>& fp_BinaryStream,
            TargetTextureEncoding  fp_Encoding,
            PeachCore::Logger* logger
        )
    {
        if (not logger)
        {
            PEACH_PRINT_ERROR("EncodeTexture: nullptr logger passed, bailing out >O<");
            return false;
        }
        else if (fp_TextureData.PixelData.empty())
        {
            logger->Error("EncodeTexture: pixel data is empty", "ResourceManagement");
            return false;
        }
        else if (fp_TextureData.Width == 0 or fp_TextureData.Height == 0)
        {
            logger->Error(
                fmt::format("EncodeTexture: zero-dimension texture {}x{} — that's not a texture kitten >w<",
                    fp_TextureData.Width, fp_TextureData.Height),
                "ResourceManagement"
            );
            return false;
        }

        if (fp_Encoding == TargetTextureEncoding::BC7)
        {
            return EncodeTextureBC7(fp_TextureData, fp_BinaryStream, logger);
        }
        else
        {
            return EncodeTextureASTC(fp_TextureData, fp_BinaryStream, fp_Encoding, logger);
        }
    }

}//namespace PeachEditor::ResourceManagement