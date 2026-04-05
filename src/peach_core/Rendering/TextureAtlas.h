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

#include "../Utils/Logger.h"
#include "PeachTexture.h"

#include <vector>
#include <tuple>

namespace PeachCore {

    struct Tile
    {
        tuple<float, float, float, float> m_UVCoords;
        //b2Body* m_PhysicsBody = nullptr;  // Pointer to the Box2D body associated with this tile, if any

        Tile(const tuple<float, float, float, float>& fp_UVCoords)
        {
            m_UVCoords = fp_UVCoords;
        }//can use a reference here since the lifetime of the attached Texture2D is directly linked to the TileSet

        Tile() = default;
        ~Tile() = default;
    };


    struct TextureAtlas
    {
    private:
        PeachTexture pm_Texture;
        uint32_t pm_TileWidth;
        uint32_t pm_TileHeight;

    //Constructor and Destructor
    public:
        TextureAtlas() = default;
        ~TextureAtlas(); //pm_Texture automatically de-referenced off stack
                          //All tiles will be cleaned up in destructor definition, and then deallocated off the stack

    //Inherited Functions
    public:
        void 
            Initialize();

    //Class Specific Methods
    public:
        Tile 
            GetTile(int index) 
            const;

        //void DefinePhysicsForTile(int index, b2World& world, float metersPerPixel); //int index, b2World& world, const b2BodyDef& bodyDef, const b2FixtureDef& fixtureDef
        void 
            SetCurrentTexture(const uint32_t fp_TextureHandle);

        bool
            SetUVs(const int fp_DesiredTileWidth, const int fp_DesiredTileHeight);

        bool
            CreateTilesFromTexture2D();

        vector<Tile> m_Tiles;

    private:
        tuple<float, float, float, float>
            GetTileUV(const int index)
            const;

        bool
            IsValidTileIndex(int fp_Index)
            const;
    };

}