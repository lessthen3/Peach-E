////////////////////////////////////////////////////////
/***                             [Peach Core]                                ***/
////////////////////////////////////////////////////////
/***                            [Version 0.0.01]                             ***/
////////////////////////////////////////////////////////
/***  Copyright(c) 2024-present Ranyodh Singh Mandur  ***/
/***                               MIT License                                ***/
/***         Documentation: TBD                                          ***/
/*** GitHub: https://github.com/iLoveJohnFish/Peach-E ***/
/////////////////////////////////////////////////////////
#pragma once

#include <SFML/Graphics.hpp>
#include "../../Managers/LogManager.h"
#include "../../Managers/ResourceLoadingManager.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <tuple>
#include <future>


namespace PeachCore {

    class Texture2D {
    public:
        Texture2D(const std::string& fp_Name ,const std::string& fp_ImagePath);
        ~Texture2D();

        Texture2D(const Texture2D& other); //copy constructor

        Texture2D& operator=(Texture2D&& other) //move operator
            noexcept 
        {
            if (this != &other) {
                // Clean up existing resources if necessary
                // No need to explicitly delete the texture since sf::Texture manages its own memory

                // Transfer resources
                pm_Texture = std::move(other.pm_Texture);  // Move the sf::Texture
                m_Name = std::move(other.m_Name);
                m_Width = other.m_Width;
                m_Height = other.m_Height;
                pm_TileWidth = other.pm_TileWidth;
                pm_TileHeight = other.pm_TileHeight;
                pm_IsValid = other.pm_IsValid;
                pm_TileUVs = std::move(other.pm_TileUVs);

                // Reset the 'other' object
                other.m_Width = 0;
                other.m_Height = 0;
                other.pm_TileWidth = 0;
                other.pm_TileHeight = 0;
                other.pm_IsValid = false;
            }
            return *this;
        }

        // Define tile size and calculate UVs for spritesheets
        void DefineTileSize(const int tileWidth, const int tileHeight);
        void CalculateTileUVs();
        
        GLuint LoadTexture(const std::string& fp_ImagePath); //used to call ResourceLoadingThread to read the desired file using stbi, and then when possible notify the Renderer to attach the texture,                       
        void DeleteTexture();                                                    //probably will do this at a certain interval like at the end of each frame, or render cycle idk
        

        std::vector<std::tuple<float, float, float, float>> GetTileUVs()
            const;
        std::tuple<float, float, float, float> GetTileUV(const int tileIndex) 
            const;
        const sf::Texture& GetSFMLTexture()
            const;
        int GetTileCount() 
            const;
        bool IsValid() 
            const;

    public:
        int m_Width, m_Height;
        std::string m_Name;

    private:
        sf::Texture pm_Texture;
        int pm_TileWidth, pm_TileHeight;
        bool pm_IsValid = false; //used for tracking whether LoadTexture() was successful/ if a texture is currently loaded
        std::vector<std::tuple<float, float, float, float>> pm_TileUVs; // UV coordinates for each tile
    };
}
//then i Want a TileMap that takes the TileSet, and has lists for holding data about currently placed tiles from the tile map.Im not