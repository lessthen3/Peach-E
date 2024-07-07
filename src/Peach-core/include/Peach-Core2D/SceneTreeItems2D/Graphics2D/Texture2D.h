#pragma once

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

        void bind(uint8_t slot = 0) const;
        void unbind() const;

        // Define tile size and calculate UVs for spritesheets
        void defineTileSize(int tileWidth, int tileHeight);
        std::tuple<float, float, float, float> getTileUV(int tileIndex) const;

    private:
        bgfx::TextureHandle bgfxTexture;
        int width, height;
        int m_tileWidth, m_tileHeight;
        std::vector<std::tuple<float, float, float, float>> tileUVs; // UV coordinates for each tile

        void calculateTileUVs();
        bool loadTexture(const char* imagePath);
    };
}
