/*******************************************************************
 *                                        Peach Core v0.0.3
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                 Peach Core is an open source game making library
********************************************************************/
#pragma once

#include "../Managers/LogManager.h"
#include "PeachNode2D.h"

#include "../General/LoadingQueue.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <tuple>
#include <future>

using namespace std;

namespace PeachCore {

    class PeachTexture2D: public PeachNode2D
    {
    public:
        PeachTexture2D() : PeachNode2D() {};
        PeachTexture2D(const string& fp_Name) : PeachNode2D(fp_Name) {};

        PeachTexture2D(const string& fp_Name, TextureData& fp_Texture);
        ~PeachTexture2D();

        PeachTexture2D& 
            operator=(PeachTexture2D&& other) //move operator
            noexcept 
        {
            if (this != &other) 
            {
                // Clean up existing resources if necessary
                // No need to explicitly delete the texture since sf::Texture manages its own memory

                // Transfer object based resources 
                pm_Texture.m_TextureByteData = move(other.pm_Texture.m_TextureByteData);  // IDK IF THIS MOVE OPERATION IS KOSCHER
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

        PeachTexture2D&
            operator=(nullptr_t fp_NullPtr) //null operator
            noexcept
        {
            pm_Texture.m_TextureByteData.reset(nullptr);
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
        
        nlohmann::json
            SerializePeachNodeToJSON();
        void
            Initialize();
        void
            Update(float fp_TimeSinceLastFrame);
        void
            ConstantUpdate(float fp_TimeSinceLastFrame);
        void
            OnSceneTreeExit();
        void
            QueueRemoval();
        void
            Draw();
 
        void 
            DeleteTexture();
        

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

    public:
        int m_Width = -1;
        int m_Height = -1;

        string m_Name;

    private:
        TextureData pm_Texture;

        int pm_TileWidth = -1;
        int pm_TileHeight = -1;

        bool pm_IsValid = false; //used for tracking whether LoadTexture() was successful/ if a texture is currently loaded

        vector<tuple<float, float, float, float>> pm_TileUVs; // UV coordinates for each tile
    };
}
//then i Want a TileMap that takes the TileSet, and has lists for holding data about currently placed tiles from the tile map.Im not