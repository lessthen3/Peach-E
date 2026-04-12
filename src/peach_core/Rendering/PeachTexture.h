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

///PeachCore
#include "Utils/Logger.h"

///STL
#include <tuple>
#include <vector>

namespace PeachCore {

    class PeachTexture
    {
    public:
        int m_Width = -1;
        int m_Height = -1;

        string m_Name;

    private:
        int pm_TileWidth = -1;
        int pm_TileHeight = -1;

        uint64_t pm_TextureID = 0;

        bool pm_IsValid = false; //used for tracking whether LoadTexture() was successful/ if a texture is currently loaded

        vector<tuple<float, float, float, float>> pm_TileUVs; // UV coordinates for each tile

    public:
        PeachTexture(const uint32_t fp_TextureWidth, const uint32_t fp_TextureHeight);
        ~PeachTexture() = default;


        // Define tile size and calculate UVs for spritesheets
        void 
            DefineTileSize(const int tileWidth, const int tileHeight);
        void 
            CalculateTileUVs();        

        vector<tuple<float, float, float, float>> 
            GetTileUVs()
            const;

        tuple<float, float, float, float> 
            GetTileUV(const int tileIndex) 
            const;

        int 
            GetTileCount() 
            const;

        bool 
            IsValid() 
            const;

        uint64_t
            GetTextureID()
            const noexcept
        {
            return pm_TextureID;
        }

    };
}
//then i Want a TileMap that takes the TileSet, and has lists for holding data about currently placed tiles from the tile map.Im not
