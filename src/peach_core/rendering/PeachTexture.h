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
#include "utils/PeachForceInline.h"
#include "utils/PeachPrint.h"
#include "math/DoubleBuffered.h"

///STL
#include <vector>

///CSTD
#include <stdint.h>

namespace PeachCore {

    class PeachTexture
    {
    public:
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;

    private:
        uint32_t pm_TileWidth = 0;
        uint32_t pm_TileHeight = 0;

        uint64_t pm_TextureID = 0; //even if each texture is 4 bytes we def going over the limit of modern cards so this is fine lmfao

        bool pm_IsValid = false; //used for tracking whether LoadTexture() was successful/ if a texture is currently loaded

        std::vector<DoubleBuffered::UVs> pm_TileUVs; // UV coordinates for each tile

    public:
        explicit PeachTexture(const uint32_t fp_TextureWidth, const uint32_t fp_TextureHeight);
        ~PeachTexture() = default;

        PeachTexture(const PeachTexture&) = delete;
        PeachTexture& operator=(const PeachTexture&) = delete;
        PeachTexture& operator=(const PeachTexture&) volatile = delete;

        // Define tile size and calculate UVs for spritesheets
        void 
            DefineTileSize(const uint32_t fp_TileWidth, const uint32_t fp_TileHeight);
        void 
            CalculateTileUVs();        

        PEACH_FORCEINLINE UVState
            GetTileUV_ReadOnly
            (
                const size_t fp_TileIndex
            ) 
            const noexcept
        {
            if (fp_TileIndex >= pm_TileUVs.size())
            {
                PEACH_PRINT_ERROR("tried to access an index out of bounds inside PeachTexture::GetTileUV()");
                return pm_TileUVs.back().GetReadSlot();
            }

            return pm_TileUVs[fp_TileIndex].GetReadSlot();
        }

        PEACH_FORCEINLINE int 
            GetTileCount() 
            const noexcept
        {
            return pm_TileUVs.size();
        }

        PEACH_FORCEINLINE bool 
            IsValid() 
            const noexcept
        { 
            return pm_IsValid; 
        }

        PEACH_FORCEINLINE uint64_t
            GetTextureID()
            const noexcept
        {
            return pm_TextureID;
        }

    };
}
//then i Want a TileMap that takes the TileSet, and has lists for holding data about currently placed tiles from the tile map.Im not
