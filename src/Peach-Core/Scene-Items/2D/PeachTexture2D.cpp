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
#include "../../include/Peach-Core/Scene-Items/2D/PeachTexture2D.h"

namespace PeachCore {

    PeachTexture2D::~PeachTexture2D()
    {
        //RenderingManager::Renderer().GetOpenGLRenderer()->DeleteTexture(m_ID);
        cout << "Hey! I am out of this joint, let's blow this popsicle stand buckoo" << "\n";
    }

    PeachTexture2D::PeachTexture2D(const string& fp_Name, const uint32_t fp_TextureWidth, const uint32_t fp_TextureHeight)
    {
        m_Name = fp_Name;
        m_Width = fp_TextureWidth; //I AM NOT SURE WHICH ONE IS X AND WHICH ONE IS Y TBH SO IMA ASSUME IT WORKS WITH NORMAL LOGIC LOL
        m_Height = fp_TextureHeight;
        pm_IsValid = true;
    }


    void
        PeachTexture2D::OnEnter()
    {

    }

    void
        PeachTexture2D::OnUpdate(float fp_TimeSinceLastFrame)
    {

    }

    void
        PeachTexture2D::OnConstantUpdate(float fp_TimeSinceLastFrame)
    {

    }

    void
        PeachTexture2D::OnExit()
    {

    }

    void
        PeachTexture2D::QueueRemoval()
    {

    }

    void
        PeachTexture2D::Draw()
    {

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