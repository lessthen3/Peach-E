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
#include "PeachTexture.h"

namespace PeachCore {

    PeachTexture::PeachTexture(const uint32_t fp_TextureWidth, const uint32_t fp_TextureHeight)
    {
        m_Width = fp_TextureWidth; //I AM NOT SURE WHICH ONE IS X AND WHICH ONE IS Y TBH SO IMA ASSUME IT WORKS WITH NORMAL LOGIC LOL
        m_Height = fp_TextureHeight;
        pm_IsValid = true;
    }

    void 
        PeachTexture::DefineTileSize(const int tileWidth, const int tileHeight)
    {
        pm_TileWidth = tileWidth;
        pm_TileHeight = tileHeight;
        CalculateTileUVs();
    }

    void 
        PeachTexture::CalculateTileUVs()
    {
        pm_TileUVs.clear(); //reset if UV's needed to be recalculated for whatever reason

        int cols = m_Width / pm_TileWidth;
        int rows = m_Height / pm_TileHeight;

        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < cols; ++x)
            {
                float u1 = (float)(x * pm_TileWidth) / m_Width;
                float v1 = (float)(y * pm_TileHeight) / m_Height;
                float u2 = (float)((x + 1) * pm_TileWidth) / m_Width;
                float v2 = (float)((y + 1) * pm_TileHeight) / m_Height;
                pm_TileUVs.emplace_back(u1, v1, u2, v2);
            }
        }
    }

    vector<tuple<float, float, float, float>> 
        PeachTexture::GetTileUVs()
        const
    {
        return pm_TileUVs;
    }

    tuple<float, float, float, float> 
        PeachTexture::GetTileUV(const int tileIndex) 
        const 
    {
        if (tileIndex < 0 || tileIndex >= pm_TileUVs.size())
        {
            throw out_of_range("Tile index is out of range.");
        }
        return pm_TileUVs[tileIndex];
    }

    int 
        PeachTexture::GetTileCount() 
        const 
    {
        return pm_TileUVs.size();
    }

    bool 
        PeachTexture::IsValid() 
        const 
    { 
        return pm_IsValid; 
    }
}