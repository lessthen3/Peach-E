#include "../../../include/Peach-Core2D/Rendering2D/Texture2D.h"

namespace PeachCore {

    Texture2D::Texture2D(const std::string& fp_Name, const std::string& fp_ImagePath)
    {
        m_Name = fp_Name;
        //if (!LoadTexture(imagePath)) {
        //    std::cerr << "Failed to load texture" << std::endl;
        //    throw std::runtime_error("Failed to load texture");
        //}

        //ID = ResourceLoadingManager::ResourceLoader().LoadTextureData(fp_ImagePath); //retrieve texture id when ready if this fails it will return the default texture

    }

    Texture2D::Texture2D(const Texture2D& other)
        : m_Width(other.m_Width), m_Height(other.m_Height),
        m_TileWidth(other.m_TileWidth), m_TileHeight(other.m_TileHeight),
        pm_IsValid(other.pm_IsValid), ID(other.ID) //INCOMPLETE COPY CONSTRUCTOR, DO NOT USE PLEASE I BEG YOU PLEASE
    {
        if (other.pm_IsValid) {
            // Optionally load texture again from a known path or duplicate the texture handle
            LoadTexture("/* path to image or other mechanism to access image data */");
        }
    }

    Texture2D::~Texture2D() {
        //if (bgfx::isValid(pm_BGFXTexture)) {
        //    bgfx::destroy(pm_BGFXTexture);
        //}
        std::cout << "Hey! I am out of this joint, let's blow this popsicle stand buckoo" << "\n";
    }

    bool Texture2D::LoadTexture(const char* imagePath) 
    {
        //if (!f_Data) {
        //    LogManager::MainLogger().Warn("Failed to load texture at: " + std::string(imagePath), "Texture2D");
        //    return false;
        //}
        //Make call to ResourceLoading Thread via a call to ResourceLoadingManager
        pm_IsValid = true;

        return true;
    }

    void Texture2D::Bind(const uint8_t slot) const {
        
    }

    void Texture2D::Unbind() const {
        // Generally, you do not need to unbind textures in BGFX explicitly
    }

    void Texture2D::DefineTileSize(const int tileWidth, const int tileHeight) {
        m_TileWidth = tileWidth;
        m_TileHeight = tileHeight;
        CalculateTileUVs();
    }

    void Texture2D::CalculateTileUVs()
    {
        m_TileUVs.clear(); //reset if UV's needed to be recalculated for whatever reason

        int cols = m_Width / m_TileWidth;
        int rows = m_Height / m_TileHeight;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                float u1 = (float)(x * m_TileWidth) / m_Width;
                float v1 = (float)(y * m_TileHeight) / m_Height;
                float u2 = (float)((x + 1) * m_TileWidth) / m_Width;
                float v2 = (float)((y + 1) * m_TileHeight) / m_Height;
                m_TileUVs.emplace_back(u1, v1, u2, v2);
            }
        }
    }


    //bgfx::TextureHandle Texture2D::GetTextureHandle() const
    //{
    //    return pm_BGFXTexture;
    //}

    std::tuple<float, float, float, float> Texture2D::GetTileUV(const int tileIndex) const {
        if (tileIndex < 0 || tileIndex >= m_TileUVs.size()) {
            throw std::out_of_range("Tile index is out of range.");
        }
        return m_TileUVs[tileIndex];
    }
    int Texture2D::GetTileCount() const {
        return m_TileUVs.size();
    }
    bool Texture2D::IsValid() const { 
        return pm_IsValid; 
    }
}