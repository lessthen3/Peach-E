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
#pragma once

///PeachCore
#include <utils/Logger.h>

///STL
#include <vector>
#include <string>

namespace PeachEditor::ResourceManagement{

    using namespace std;

    enum class TargetTextureEncoding { BC7, ASTC_4x4, ASTC_6x6, ASTC_8x8 };


    struct RawTextureData
    {
        vector<uint8_t> PixelData; // RGBA always, lodepng default
        uint32_t        Width = 0;
        uint32_t        Height = 0;
        // Channels is always 4 since lodepng decodes to RGBA
        // store original path for UID/metadata generation later
        string          SourcePath;
    };

    struct RawMeshVertex
    {
        float Position[3];
        float Normal[3];
        float UV[2];
        float Tangent[3];
    };

    // one RawMeshData per aiMesh, a single file can produce multiple of these
    // (e.g. a character fbx with body/hair/armour as separate meshes)
    struct RawMeshData
    {
        vector<RawMeshVertex> Vertices;
        vector<uint32_t>      Indices;
        string                Name;
        string                SourcePath;
    };

    [[nodiscard]] bool
        WriteSPIRVToFile
        (
            const vector<uint32_t>& fp_SpirvBytecode,
            const string& fp_DesiredOutputDirectory,
            const string& fp_DesiredName,
            PeachCore::Logger* logger
        );

    [[nodiscard]] bool
        LoadRawShaderSource
        (
            string* fp_SourceCode,
            const string& fp_ShaderSourcePath,
            PeachCore::Logger* logger
        );

    [[nodiscard]] bool
        LoadTextureFFS
        (
            const string& fp_FilePath,
            RawTextureData& fp_OutTexture,
            PeachCore::Logger* logger
        );

    [[nodiscard]] bool
        EncodeTexture
        (
            const RawTextureData& fp_TextureData,
            vector<uint8_t>& fp_BinaryStream,
            TargetTextureEncoding  fp_Encoding,
            PeachCore::Logger* logger
        );
    
}