#pragma once

#include "../../Managers/LogManager.h"
#include "../../Managers/ResourceLoadingManager.h"

#include <stb_image.h>
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

       // Texture2D& operator=(Texture2D&& other) noexcept {
            //if (this != &other) {
            //    if (bgfx::isValid(pm_BGFXTexture)) {
            //        bgfx::destroy(pm_BGFXTexture); // Clean up existing texture
            //    }

            //    // Transfer resources from 'other' to 'this'
            //    pm_BGFXTexture = other.pm_BGFXTexture;
            //    m_Width = other.m_Width;
            //    m_Height = other.m_Height;
            //    m_TileWidth = other.m_TileWidth;
            //    m_TileHeight = other.m_TileHeight;
            //    pm_IsValid = other.pm_IsValid;

            //    // Invalidate the moved-from object to prevent it from freeing the resource
            //    other.pm_BGFXTexture = BGFX_INVALID_HANDLE;
            //}
            //return *this;
        //}
        

        void Bind(const uint8_t slot = 0) const;
        void Unbind() const;

        // Define tile size and calculate UVs for spritesheets
        void DefineTileSize(const int tileWidth, const int tileHeight);
        std::tuple<float, float, float, float> GetTileUV(const int tileIndex) const;

        
        void CalculateTileUVs();
        bool LoadTexture(const char* imagePath); //used to call ResourceLoadingThread to read the desired file using stbi, and then when possible notify the Renderer to attach the texture,
        bool IsValid() const;                                   //probably will do this at a certain interval like at the end of each frame, or render cycle idk

        std::vector<std::tuple<float, float, float, float>> m_TileUVs; // UV coordinates for each tile

        int GetTileCount() const;
        GLuint GetTextureHandle() const;

        int m_Width, m_Height;
        //std::future<GLuint> ID;
        GLuint ID;
        GLuint VAO, VBO, EBO; //each resource manages the ids for its vao, vbo and ebo's

        std::string m_Name;

    private:
        int m_TileWidth, m_TileHeight;
        bool pm_IsValid = false; //used for tracking whether LoadTexture() was successful
        

    };
}
//then i Want a TileMap that takes the TileSet, and has lists for holding data about currently placed tiles from the tile map.Im not