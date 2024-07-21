#include "../../../include/Peach-Core2D/Rendering2D/PeachTexture2D.h"

namespace PeachCore {

    PeachTexture2D::~PeachTexture2D()
    {
        //RenderingManager::Renderer().GetOpenGLRenderer()->DeleteTexture(m_ID);
        std::cout << "Hey! I am out of this joint, let's blow this popsicle stand buckoo" << "\n";
    }

    PeachTexture2D::PeachTexture2D(const std::string& name, const std::string& imagePath) : m_Name(name) 
    {
        //if (!pm_Texture.loadFromFile(imagePath))
        //{
        //    std::cerr << "Failed to load texture from: " << imagePath << std::endl;
        //    throw std::runtime_error("Failed to load texture");
        //}

        //m_Width = pm_Texture.getSize().x;
        //m_Height = pm_Texture.getSize().y;
        //pm_IsValid = true;
    }

    PeachTexture2D::PeachTexture2D(const PeachTexture2D& other)
        : m_Width(other.m_Width), m_Height(other.m_Height),
        pm_TileWidth(other.pm_TileWidth), pm_TileHeight(other.pm_TileHeight),
        pm_IsValid(other.pm_IsValid) //INCOMPLETE COPY CONSTRUCTOR, DO NOT USE PLEASE I BEG YOU PLEASE
    {
        if (other.pm_IsValid) 
        {
            // Optionally load texture again from a known path or duplicate the texture handle
            LoadTexture("/* path to image or other mechanism to access image data */");
        }
    }

    void PeachTexture2D::LoadTexture(const std::string& fp_ImagePath)
    {
        //retrieve texture id when ready if this fails it will return the default texture
        //GLuint f_TextureID = ResourceLoadingManager::ResourceLoader().LoadTextureData(fp_ImagePath, RenderingManager::Renderer().GetOpenGLRenderer()); 
        pm_IsValid = true;

        return;
    }

    void PeachTexture2D::DeleteTexture() //resets the texture2D object
    {
        if (pm_IsValid) {
            pm_Texture = Texture2D();  // Assign a new, empty sf::Texture to release the old one
            m_Width = 0;
            m_Height = 0;
            pm_TileWidth = 0;
            pm_TileHeight = 0;
            pm_TileUVs.clear();
            pm_IsValid = false;
        }
    }

    void PeachTexture2D::DefineTileSize(const int tileWidth, const int tileHeight) {
        pm_TileWidth = tileWidth;
        pm_TileHeight = tileHeight;
        CalculateTileUVs();
    }

    void PeachTexture2D::CalculateTileUVs()
    {
        pm_TileUVs.clear(); //reset if UV's needed to be recalculated for whatever reason

        int cols = m_Width / pm_TileWidth;
        int rows = m_Height / pm_TileHeight;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                float u1 = (float)(x * pm_TileWidth) / m_Width;
                float v1 = (float)(y * pm_TileHeight) / m_Height;
                float u2 = (float)((x + 1) * pm_TileWidth) / m_Width;
                float v2 = (float)((y + 1) * pm_TileHeight) / m_Height;
                pm_TileUVs.emplace_back(u1, v1, u2, v2);
            }
        }
    }

    std::vector<std::tuple<float, float, float, float>> PeachTexture2D::GetTileUVs()
        const
    {
        return pm_TileUVs;
    }
    std::tuple<float, float, float, float> PeachTexture2D::GetTileUV(const int tileIndex) 
        const 
    {
        if (tileIndex < 0 || tileIndex >= pm_TileUVs.size()) {
            throw std::out_of_range("Tile index is out of range.");
        }
        return pm_TileUVs[tileIndex];
    }
   
    // Additional methods to interact with SFML's Texture
    const Texture2D& PeachTexture2D::GetPeachTexture2D()
        const 
    {
        return pm_Texture;
    }
    int PeachTexture2D::GetTileCount() 
        const 
    {
        return pm_TileUVs.size();
    }
    bool PeachTexture2D::IsValid() 
        const 
    { 
        return pm_IsValid; 
    }
}