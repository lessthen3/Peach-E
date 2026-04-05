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
#include "TextureAtlas.h"

namespace PeachCore {

    TextureAtlas::~TextureAtlas() 
    {
        // Cleanup Box2D bodies if needed OLD BOX2D V2.4.1 OOOOOOO BOY
        /*for (auto& tile : m_Tiles) {
            if (tile.m_PhysicsBody) {
                tile.m_PhysicsBody->GetWorld()->DestroyBody(tile.m_PhysicsBody);
            }
        }*/
    }

    void 
        TextureAtlas::Initialize()
    {

    }

    bool 
        TextureAtlas::CreateTilesFromTexture2D()
    {
        //if (not pm_Texture.IsValid())
        //{
        //    //TODO: should push this logger warning/error to the rendering manager in the rendering thread
        //    //LogManager::MainLogger().LogAndPrint("Attempted to create tiles for tile set when no texture was loaded", m_Name, "warn");
        //    return false;
        //}

        //m_Tiles.clear(); //clear list in case new tile UV's are generated
        //m_Tiles.resize(pm_Texture.GetTileCount());  // Resize m_Tiles vector to match the number of UVs calculated

        //for (auto& tuple : pm_Texture.GetTileUVs())
        //{
        //    m_Tiles.push_back(Tile(tuple));
        //}

        return true;
    }
   
    //void 
    //    TextureAtlas::SetCurrentTexture(TextureData& fp_NewTexture) //idk how moving the unique_ptr reference is gonna act 2 functions deep, but it should work normally
    //{
    //    pm_Texture = PeachTexture2D(m_Name, fp_NewTexture);
    //}

    bool 
        TextureAtlas::SetUVs(const int fp_DesiredTileWidth, const int fp_DesiredTileHeight)
    {
        if (not pm_Texture.IsValid())
        {
            //TODO: should push this logger warning/error to the rendering manager in the rendering thread
            //LogManager::MainLogger().LogAndPrint("Attempted to set UV's for tile set when no texture was loaded", m_Name, "warn");
            return false;
        }

        pm_TileWidth = fp_DesiredTileWidth;
        pm_TileHeight = fp_DesiredTileHeight;

        pm_Texture.DefineTileSize(fp_DesiredTileWidth, fp_DesiredTileHeight);
        pm_Texture.CalculateTileUVs();
        
        return true;
    }

    //void TextureAtlas::DefinePhysicsForTile(int fp_TileIndex, b2World& world, float metersPerPixel) {
    //    ValidateTileIndex(fp_TileIndex);


    //    Tile& tile = m_Tiles[fp_TileIndex];
    //    b2BodyDef bodyDef;
    //    bodyDef.type = b2_staticBody; // Typically m_Tiles are static
    //    b2PolygonShape shape;

    //    float physWidth = pm_TileWidth * metersPerPixel;
    //    float physHeight = pm_TileHeight * metersPerPixel;

    //    // Define the center position based on tile index if necessary
    //    // Example for simplicity: assuming all m_Tiles are at (0,0) origin
    //    b2Vec2 position(0, 0); // Modify as needed based on actual tile positioning logic
    //    bodyDef.position.Set(position.x + physWidth / 2, position.y + physHeight / 2);

    //    shape.SetAsBox(physWidth / 2, physHeight / 2); // SetAsBox takes half-width and half-height
    //    b2FixtureDef fixtureDef;
    //    fixtureDef.shape = &shape;

    //    // Additional fixture properties like density, friction, and restitution can be set here
    //    fixtureDef.density = 1.0f;
    //    fixtureDef.friction = 0.3f;

    //    tile.m_PhysicsBody = world.CreateBody(&bodyDef);
    //    tile.m_PhysicsBody->CreateFixture(&fixtureDef);
    //}

    Tile 
        TextureAtlas::GetTile(int fp_TileIndex) 
        const 
    {
        if (not IsValidTileIndex(fp_TileIndex))
        {
            //do smth idk
        }

        return m_Tiles[fp_TileIndex];
    }

    tuple<float, float, float, float> 
        TextureAtlas::GetTileUV(const int index) 
        const 
    {
        //return pm_Texture.GetTileUV(index);  // Delegate to Texture2D to fetch UVs
        return { 0,0,0,0 };
    }

    bool 
        TextureAtlas::IsValidTileIndex(int fp_Index) 
        const
    {
        if (fp_Index < 0 || fp_Index >= m_Tiles.size())
        {
            //TODO: should push this logger warning/error to the rendering manager in the rendering thread
            //LogManager::MainLogger().LogAndPrint("Attempted to access invalid index number for tile set list", m_Name, "warn");

            //idk if i should just brick the program runtime cause an invalid tile is chosen lmfao we can just choose a default "invalid tile" texture as a fallback like the source pink checkerboard
            //throw out_of_range("Tile index is out of range.");

            return false;
        }

        return true;
    }

}