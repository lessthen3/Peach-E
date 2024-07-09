#pragma once

#include "../../../Managers/LogManager.h"

#include <bgfx/bgfx.h>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <tuple>

namespace PeachCore {

    class Texture2D {
    public:
        Texture2D(const char* imagePath);
        ~Texture2D();

        void Bind(const uint8_t slot = 0) const;
        void Unbind() const;

        // Define tile size and calculate UVs for spritesheets
        void DefineTileSize(const int tileWidth, const int tileHeight);
        std::tuple<float, float, float, float> GetTileUV(const int tileIndex) const;

        int GetTileCount() const;
        void CalculateTileUVs();
        bool LoadTexture(const char* imagePath);
        bool IsValid() const;

        std::vector<std::tuple<float, float, float, float>> m_TileUVs; // UV coordinates for each tile

    private:
        bgfx::TextureHandle bgfxTexture;
        int pm_Width, pm_Height;
        int m_TileWidth, m_TileHeight;
        bool pm_IsValid = false; //used for tracking whether LoadTexture() was successful
        

    };
}
//then i Want a TileMap that takes the TileSet, and has lists for holding data about currently placed tiles from the tile map.Im not