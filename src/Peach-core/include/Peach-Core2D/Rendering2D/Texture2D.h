#pragma once

#include "../../Managers/LogManager.h"
#include "../../Managers/ResourceLoadingManager.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <tuple>
#include <future>

#include <GL/glew.h>

namespace PeachCore {

    class Texture2D {
    public:
        Texture2D(const std::string& fp_Name ,const std::string& fp_ImagePath);
        ~Texture2D();

        Texture2D(const Texture2D& other);

        Texture2D& operator=(Texture2D&& other) noexcept {
            if (this != &other) {
                if (m_ID != 696913376969) {
                    DeleteTexture(); // Clean up existing texture
                }

                // Transfer resources from 'other' to 'this'
                m_ID = other.m_ID;
                m_Name = other.m_Name;
                m_Width = other.m_Width;
                m_Height = other.m_Height;
                m_TileWidth = other.m_TileWidth;
                m_TileHeight = other.m_TileHeight;
                pm_IsValid = other.pm_IsValid;

                m_VAO = other.m_VAO;
                m_VBO = other.m_VBO;
                m_EBO = other.m_EBO;


                // Invalidate the moved-from object to prevent it from freeing the resource
                other.m_ID = 696913376969;
            }
            return *this;
        }
        

        void Bind(const uint8_t slot = 0) const;
        void Unbind() const;

        // Define tile size and calculate UVs for spritesheets
        void DefineTileSize(const int tileWidth, const int tileHeight);
        std::tuple<float, float, float, float> GetTileUV(const int tileIndex) const;

        
        void CalculateTileUVs();
        GLuint LoadTexture(const std::string& fp_ImagePath); //used to call ResourceLoadingThread to read the desired file using stbi, and then when possible notify the Renderer to attach the texture,
        bool IsValid() const;                                   //probably will do this at a certain interval like at the end of each frame, or render cycle idk
        void DeleteTexture();
        std::vector<std::tuple<float, float, float, float>> m_TileUVs; // UV coordinates for each tile

        int GetTileCount() const;
        GLuint GetTextureHandle() const;

        int m_Width, m_Height;
        //std::future<GLuint> ID;
        GLuint m_ID = 696913376969; //default "no-texture" texture, so that if this Texture2D is referenced before fully loaded it doesnt brick the glcontext
        GLuint m_VAO, m_VBO, m_EBO; //each resource manages the ids for its vao, vbo and ebo's

        std::string m_Name;

    private:
        int m_TileWidth, m_TileHeight;
        bool pm_IsValid = false; //used for tracking whether LoadTexture() was successful/ if a texture is currently loaded
        

    };
}
//then i Want a TileMap that takes the TileSet, and has lists for holding data about currently placed tiles from the tile map.Im not