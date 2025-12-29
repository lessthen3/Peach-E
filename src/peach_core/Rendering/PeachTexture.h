/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///PeachCore
#include "../Utils/Logger.h"

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
        ~PeachTexture();

        PeachTexture& 
            operator=(PeachTexture&& other) //move operator
            noexcept 
        {
            if (this != &other) 
            {
                // Clean up existing resources if necessary
                // No need to explicitly delete the texture since sf::Texture manages its own memory

                // Transfer object based resources 
                pm_TileUVs = move(other.pm_TileUVs);
                m_Name = move(other.m_Name);

                //Create new copies of primitive types
                m_Width = other.m_Width;
                m_Height = other.m_Height;

                pm_TileWidth = other.pm_TileWidth;
                pm_TileHeight = other.pm_TileHeight;

                pm_IsValid = other.pm_IsValid;

                // "Reset" the other object
                other.m_Width = 0;
                other.m_Height = 0;

                other.pm_TileWidth = 0;
                other.pm_TileHeight = 0;

                other.pm_IsValid = false;
            }
            return *this;
        }

        PeachTexture&
            operator=(nullptr_t fp_NullPtr) //null operator
            noexcept
        {
            pm_TileUVs.clear();
            m_Name = "";

            m_Width = 0;
            m_Height = 0;

            pm_TileWidth = 0;
            pm_TileHeight = 0;

            pm_IsValid = false;

            return *this;
        }

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