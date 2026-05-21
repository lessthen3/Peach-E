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
#include "ObjLoader.h"

///cglm
#include <cglm/cglm.h>
#include <cglm/struct.h>

///STL
#include <filesystem>
#include <fstream>
#include <string_view>
#include <charconv>

// Static Internal Linkage
namespace PeachCore::OBJ {

    struct IndexGroup
    {
        int32_t Position = IDX_NO_VALUE;
        int32_t TextureCoord = IDX_NO_VALUE;
        int32_t Normal = IDX_NO_VALUE;
    };

    static PEACH_FORCEINLINE PEACH_STATUS_CODE
        ParseVertexTokens
        (
            std::string_view fp_VertexToken, 
            IndexGroup& fp_OutIdxGroup
        )
        noexcept//can we really noexcept here idk substr isnt noexcept guaranteed but maybe we do w our structure weoweoweo blitzcrank
    {
        size_t f_FirstSlashIndex = fp_VertexToken.find('/');  // Find the slashes to separate v/vt/vn

        ////////////////////////////////////////////// Parse Position //////////////////////////////////////////////

        std::string_view f_PositionToken = fp_VertexToken.substr(0, f_FirstSlashIndex); // Position is everything up to the first slash (or the end of string if no slashes)

        if (not f_PositionToken.empty())
        {
            std::from_chars(f_PositionToken.data(), f_PositionToken.data() + f_PositionToken.size(), fp_OutIdxGroup.Position);
            fp_OutIdxGroup.Position -= 1; //convert to 0 based indexing cause wavefront is fucking weird
        }

        ////////////////////////////////////////////// Parse Texture Coordinate //////////////////////////////////////////////

        if (f_FirstSlashIndex == std::string_view::npos) //WARNING UNSURE TBH
        {

            return PEACH_ERROR_FOUND_INVALID_TOKEN_IN_MISFORMED_OBJ;
        }

        size_t f_SecondSlashIndex = fp_VertexToken.find('/', f_FirstSlashIndex + 1);

        // Texture coordinate is between first and second slash
        size_t f_TextureTokenLength = f_SecondSlashIndex - (f_FirstSlashIndex + 1);
        std::string_view f_TextureCoordToken = fp_VertexToken.substr(f_FirstSlashIndex + 1, f_TextureTokenLength);

        if (not f_TextureCoordToken.empty())
        {
            std::from_chars(f_TextureCoordToken.data(), f_TextureCoordToken.data() + f_TextureCoordToken.size(), fp_OutIdxGroup.TextureCoord);
            fp_OutIdxGroup.TextureCoord -= 1;
        }

        ////////////////////////////////////////////// Parse Normal //////////////////////////////////////////////

        if (f_SecondSlashIndex == std::string_view::npos)
        {

            return PEACH_ERROR_FOUND_INVALID_TOKEN_IN_MISFORMED_OBJ;
        }

        std::string_view f_NormalToken = fp_VertexToken.substr(f_SecondSlashIndex + 1);

        if (not f_NormalToken.empty())
        {
            std::from_chars(f_NormalToken.data(), f_NormalToken.data() + f_NormalToken.size(), fp_OutIdxGroup.Normal);
            fp_OutIdxGroup.Normal -= 1;
        }

        return PEACH_OK;
    }
}

namespace PeachCore::OBJ {

    PEACH_STATUS_CODE
        LoadMesh
        (
            std::string fp_ObjFilePathOwO,
            InterleavedMesh& fp_OutMesh,
            Logger* logger
        )
    {
        ////////////////////////////////////////////// Safety Checks //////////////////////////////////////////////

        if (not logger) [[unlikely]]
        {
            PEACH_PRINT_ERROR("tried to pass nullptr reference to logger inside PeachCore::OBJ::LoadMesh()");
            return PEACH_INTERNAL_ERROR_PASSED_NULLPTR_REFERENCE_TO_LOGGER;
        }

        ////////////////////////////////////////////// Validate File //////////////////////////////////////////////

        if (not filesystem::exists(fp_ObjFilePathOwO))
        {
            logger->Error(fmt::format("Unable to load OBJ mesh from file named: '{}', file does not exist at the location requested", fp_ObjFilePathOwO), "OBJ::LoadMesh");
            return PEACH_ERROR_FILE_NOT_FOUND_WHEN_ASKED_TO_LOAD;
        }

        // Extract file extension assuming fmt::format "filename.ext"
        size_t f_LastDotIndex = fp_ObjFilePathOwO.rfind('.');

        if (f_LastDotIndex == string::npos)
        {
            logger->Error(fmt::format("Serialization Error: No file extension found for Peach-E Binary"), "OBJ::LoadMesh");
            return PEACH_ERROR_NO_FILE_EXTENSION_FOUND_FOR_FILE_ASKED_TO_LOAD;
        }

        string f_FileExtension = fp_ObjFilePathOwO.substr(f_LastDotIndex); //TODO: convert to lowercase idrc ab case sensitivity here owo

        if (f_FileExtension != ".obj")
        {
            logger->Error(fmt::format("Invalid file extension found: '{}' for file named: '{}', when .obj was expected when asked to load a obj file", fp_ObjFilePathOwO, f_FileExtension), "OBJ::LoadMesh");
            return PEACH_ERROR_FOUND_INVALID_FILE_EXTENSION_FOR_FILE_ASKED_TO_LOAD;
        }

        ////////////////////////////////////////////// Open File //////////////////////////////////////////////

        std::ifstream f_OpenObjFileStream(fp_ObjFilePathOwO);

        if (not f_OpenObjFileStream.is_open())
        {
            logger->Error(fmt::format("Unable to open obj file with file name: {}", fp_ObjFilePathOwO), "LoadMesh"); //the sender bt is kinda redundant for error here but w/e
            return PEACH_ERROR_FAILED_TO_OPEN_OBJ_FILE;
        }

        ////////////////////////////////////////////// First Parsing Pass //////////////////////////////////////////////

        std::vector<vec3s> f_Positions;
        std::vector<vec2s> f_TextureCoords;
        std::vector<vec3s> f_Normals;
        
        std::vector<IndexGroup> f_Faces;

        std::string f_Prefix;

        while (f_OpenObjFileStream >> f_Prefix) // Read the first token of every line automatically
        {
            if (f_Prefix.empty()) //OwO
            {
                continue;
            }

            if (f_Prefix[0] == 'v')
            {
                if (f_Prefix.length() == 1) // just "v"
                {
                    float x, y, z;
                    f_OpenObjFileStream >> x >> y >> z;

                    f_Positions.emplace_back(vec3s{ x, y, z });
                }
                else if (f_Prefix[1] == 't') // "vt"
                {
                    float u, v;
                    f_OpenObjFileStream >> u >> v;

                    f_TextureCoords.emplace_back(vec2s{ u, 1 - v }); //flip v for opengl uwu
                }
                else if (f_Prefix[1] == 'n')  // "vn"
                {
                    float x, y, z;
                    f_OpenObjFileStream >> x >> y >> z;

                    f_Normals.emplace_back(vec3s{ x, y, z });
                }
                else
                {
                    logger->Error(fmt::format("unable to parse misformed obj file with file name: '{}', found: '{}' when (v, vt, vn) was expected, loading failed ;w;", fp_ObjFilePathOwO, f_Prefix), "LoadMesh"); //the sender bt is kinda redundant for error here but w/e
                    return PEACH_ERROR_FOUND_INVALID_TOKEN_IN_MISFORMED_OBJ;
                }
            }
            else if (f_Prefix == "f")
            {
                std::string t1, t2, t3;
                f_OpenObjFileStream >> t1 >> t2 >> t3;

                IndexGroup f_IdxGroup;

                PEACH_STATUS_CODE result = ParseVertexTokens(t1, f_IdxGroup);

                if (result != PEACH_OK)
                {

                    return result;
                }

                f_Faces.push_back(f_IdxGroup);

                result = ParseVertexTokens(t2, f_IdxGroup);

                if (result != PEACH_OK)
                {

                    return result;
                }

                f_Faces.push_back(f_IdxGroup);

                result = ParseVertexTokens(t3, f_IdxGroup);

                if (result != PEACH_OK)
                {

                    return result;
                }

                f_Faces.push_back(f_IdxGroup);

            }
            else  // It's a comment (#), material file (mtllib), usemtl, or something we don't care about, skip the rest of the current line //TODO ACTUALLY FIX THIS FOR BETTER ERROR HANDLING UWU
            {
                std::string dummy;
                std::getline(f_OpenObjFileStream, dummy);
            }
        }

        ////////////////////////////////////////////// Reconcile Faces into Interleaved GPU format //////////////////////////////////////////////

        size_t f_CurrentIndex = 0;

        for (IndexGroup lv_IndexValues : f_Faces)
        {
            if (static_cast<size_t>(lv_IndexValues.Position) >= f_Positions.size()) 
            {
                logger->Error(fmt::format("obj file named: '{}', contains invalid index values for positions, did you edit the .obj file by hand? if not your modeling software generated an invalid obj file ;w;", fp_ObjFilePathOwO), "LoadMesh");
                return PEACH_ERROR_INVALID_OBJ_REFERENCING_NON_EXISTENT_INDICES;
            }

            {
                vec3s f_PositionVector = f_Positions[lv_IndexValues.Position];

                fp_OutMesh.Vertices.push_back(f_PositionVector.x);
                fp_OutMesh.Vertices.push_back(f_PositionVector.y);
                fp_OutMesh.Vertices.push_back(f_PositionVector.z);
            }

            if (static_cast<size_t>(lv_IndexValues.TextureCoord) >= f_TextureCoords.size())
            { 
                logger->Error(fmt::format("obj file named: '{}', contains invalid index values for texture uvs, did you edit the .obj file by hand? if not your modeling software generated an invalid obj file ;w;", fp_ObjFilePathOwO), "LoadMesh");
                return PEACH_ERROR_INVALID_OBJ_REFERENCING_NON_EXISTENT_INDICES;
            }

            {
                vec2s f_TextureCoordVector = f_TextureCoords[lv_IndexValues.TextureCoord];

                fp_OutMesh.Vertices.push_back(f_TextureCoordVector.x);
                fp_OutMesh.Vertices.push_back(f_TextureCoordVector.y);
            }

            if (static_cast<size_t>(lv_IndexValues.Normal) >= f_Normals.size())
            {
                logger->Error(fmt::format("obj file named: '{}', contains invalid index values for normals, did you edit the .obj file by hand? if not your modeling software generated an invalid obj file ;w;", fp_ObjFilePathOwO), "LoadMesh");
                return PEACH_ERROR_INVALID_OBJ_REFERENCING_NON_EXISTENT_INDICES;
            }

            {
                vec3s f_NormalVector = f_Normals[lv_IndexValues.Normal];

                fp_OutMesh.Vertices.push_back(f_NormalVector.x);
                fp_OutMesh.Vertices.push_back(f_NormalVector.y);
                fp_OutMesh.Vertices.push_back(f_NormalVector.z);
            }

            //then calculate 4 float tangent?

            fp_OutMesh.Indices.push_back(++f_CurrentIndex); //wouldn't this be a vector of 0,1 ,2 ,3 ,4 ,5 lmfao idk
        }

        ////////////////////////////////////////////// Success! //////////////////////////////////////////////

        return PEACH_OK;
    }
}