#pragma once

#include "../../../Managers/LogManager.h"
#include "../../../Unsorted/PeachNode.h"
#include "Texture2D.h"

#include <vector>
#include <tuple>
#include <Box2D/Box2D.h>


namespace PeachCore {

    struct Tile {
    public:
        std::tuple<float, float, float, float> m_UVCoords;
        b2Body* m_PhysicsBody = nullptr;  // Pointer to the Box2D body associated with this tile, if any

        Tile(std::tuple<float, float, float, float>& fp_UVCoords)
        {
            m_UVCoords = fp_UVCoords;
        }//can use a reference here since the lifetime of the attached Texture2D is directly linked to the TileSet

        Tile() = default;
        ~Tile() = default;
    };


    class TileSet: public PeachNode{
    public:
        TileSet();
        ~TileSet(); //pm_Texture automatically de-referenced off stack
                          //All tiles will be cleaned up in destructor definition, and then deallocated off the stack

        void Initialize();
        void Update(float fp_TimeSinceLastFrame);
        void ConstantUpdate(float fp_TimeSinceLastFrame);
        void OnSceneTreeExit();
        void QueueRemoval();

    public:
        Tile GetTile(int index) const;
        void DefinePhysicsForTile(int index, b2World& world, float metersPerPixel); //int index, b2World& world, const b2BodyDef& bodyDef, const b2FixtureDef& fixtureDef
        void LoadTexture(const char* fp_ImagePath);
        void SetUVs(const int fp_DesiredTileWidth, const int fp_DesiredTileHeight);

        void CreateTilesFromTexture2D();

        std::vector<Tile> m_Tiles;
        
    private:
        Texture2D pm_Texture = NULL; //null on construction so that the texture can be validated to have a value
        int pm_TileWidth;
        int pm_TileHeight;
        

        std::tuple<float, float, float, float> GetTileUV(const int index) const;
        void ValidateTileIndex(int fp_Index) const;
        // Other methods...
    };

}