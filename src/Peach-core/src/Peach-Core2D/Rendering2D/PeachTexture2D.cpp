#include "../../../include/Peach-Core2D/Rendering2D/PeachTexture2D.h"

namespace PeachCore {

    PeachTexture2D::~PeachTexture2D()
    {
        //RenderingManager::Renderer().GetOpenGLRenderer()->DeleteTexture(m_ID);
        cout << "Hey! I am out of this joint, let's blow this popsicle stand buckoo" << "\n";
    }

    PeachTexture2D::PeachTexture2D(const string& fp_Name,  unique_ptr<sf::Texture>& fp_Texture) : m_Name(fp_Name)
    {
        pm_Texture = move(fp_Texture);

        sf::Vector2u f_TextureSize = pm_Texture->getSize();

        m_Width = f_TextureSize.x; //I AM NOT SURE WHICH ONE IS X AND WHICH ONE IS Y TBH SO IMA ASSUME IT WORKS WITH NORMAL LOGIC LOL
        m_Height = f_TextureSize.y;

        pm_IsValid = true;
    }

    void PeachTexture2D::DeleteTexture() //resets the texture2D object
    {
        if (pm_IsValid) 
        {
            pm_Texture = nullptr; //reassign texture to nullptr so that the unique_ptr gets dereferenced and is cleaned up automatically
        }
    }

    void 
        PeachTexture2D::DefineTileSize(const int tileWidth, const int tileHeight)
    {
        pm_TileWidth = tileWidth;
        pm_TileHeight = tileHeight;
        CalculateTileUVs();
    }

    void 
        PeachTexture2D::CalculateTileUVs()
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
        PeachTexture2D::GetTileUVs()
        const
    {
        return pm_TileUVs;
    }

    tuple<float, float, float, float> 
        PeachTexture2D::GetTileUV(const int tileIndex) 
        const 
    {
        if (tileIndex < 0 || tileIndex >= pm_TileUVs.size())
        {
            throw out_of_range("Tile index is out of range.");
        }
        return pm_TileUVs[tileIndex];
    }
   
    // Additional methods to interact with SFML's Texture
    const sf::Texture* 
        PeachTexture2D::GetPeachTexture2D()
        const 
    {
        return pm_Texture.get();
    }

    int 
        PeachTexture2D::GetTileCount() 
        const 
    {
        return pm_TileUVs.size();
    }

    bool 
        PeachTexture2D::IsValid() 
        const 
    { 
        return pm_IsValid; 
    }
}