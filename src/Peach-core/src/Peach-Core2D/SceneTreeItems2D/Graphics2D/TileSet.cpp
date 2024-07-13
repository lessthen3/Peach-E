#include "../../../../include/Peach-Core2D/SceneTreeItems2D/Graphics2D/TileSet.h"
//#include "../../../../include/Peach-Core2D/Rendering2D/Texture2D.h"
namespace PeachCore {

    //void TileSet::CreateTilesFromTexture2D()
    //{
    //    if (!pm_Texture.IsValid())
    //    {
    //        LogManager::MainLogger().Warn("Attempted to create tiles for tile set when no texture was loaded", name);
    //        return;
    //    }

    //    m_Tiles.clear(); //clear list in case new tile UV's are generated
    //    m_Tiles.resize(pm_Texture.GetTileCount());  // Resize m_Tiles vector to match the number of UVs calculated

    //    for (auto& tuple : pm_Texture.m_TileUVs)
    //    {
    //        m_Tiles.push_back(Tile(tuple));
    //    }
    //}

    TileSet::~TileSet() {
        // Cleanup Box2D bodies if needed
        for (auto& tile : m_Tiles) {
            if (tile.m_PhysicsBody) {
                tile.m_PhysicsBody->GetWorld()->DestroyBody(tile.m_PhysicsBody);
            }
        }
    }

    void TileSet::Initialize()
    {

    }

    void TileSet::Update(float fp_TimeSinceLastFrame)
    {

    }

    void TileSet::ConstantUpdate(float fp_TimeSinceLastFrame)
    {

    }

    void TileSet::OnSceneTreeExit()
    {

    }

    void TileSet::QueueRemoval()
    {

    }

   
    void TileSet::LoadTexture(const std::string fp_Name, const std::string& fp_ImagePath) 
    {
        //pm_Texture = Texture2D(fp_Name, fp_ImagePath);
    }

    //void TileSet::SetUVs(const int fp_DesiredTileWidth, const int fp_DesiredTileHeight)
    //{
    //    if (!pm_Texture.IsValid())
    //    {
    //        LogManager::MainLogger().Warn("Attempted to set UV's for tile set when no texture was loaded", name);
    //        return;
    //    }

    //    pm_TileWidth = fp_DesiredTileWidth;
    //    pm_TileHeight = fp_DesiredTileHeight;

    //    pm_Texture.DefineTileSize(fp_DesiredTileWidth, fp_DesiredTileHeight);
    //    pm_Texture.CalculateTileUVs();
    //    
    //}

    void TileSet::DefinePhysicsForTile(int fp_TileIndex, b2World& world, float metersPerPixel) {
        ValidateTileIndex(fp_TileIndex);


        Tile& tile = m_Tiles[fp_TileIndex];
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody; // Typically m_Tiles are static
        b2PolygonShape shape;

        float physWidth = pm_TileWidth * metersPerPixel;
        float physHeight = pm_TileHeight * metersPerPixel;

        // Define the center position based on tile index if necessary
        // Example for simplicity: assuming all m_Tiles are at (0,0) origin
        b2Vec2 position(0, 0); // Modify as needed based on actual tile positioning logic
        bodyDef.position.Set(position.x + physWidth / 2, position.y + physHeight / 2);

        shape.SetAsBox(physWidth / 2, physHeight / 2); // SetAsBox takes half-width and half-height
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;

        // Additional fixture properties like density, friction, and restitution can be set here
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;

        tile.m_PhysicsBody = world.CreateBody(&bodyDef);
        tile.m_PhysicsBody->CreateFixture(&fixtureDef);
    }

    Tile TileSet::GetTile(int fp_TileIndex) const {
        ValidateTileIndex(fp_TileIndex);
        return m_Tiles[fp_TileIndex];
    }

    std::tuple<float, float, float, float> TileSet::GetTileUV(const int index) const {
        //return pm_Texture.GetTileUV(index);  // Delegate to Texture2D to fetch UVs
        return { 0,0,0,0 };
    }

    void TileSet::ValidateTileIndex(int fp_Index) const
    {
        if (fp_Index < 0 || fp_Index >= m_Tiles.size())
        {
            LogManager::MainLogger().Warn("Attempted to access invalid index number for tile set list", name);
            throw std::out_of_range("Tile index is out of range.");
        }
    }

}