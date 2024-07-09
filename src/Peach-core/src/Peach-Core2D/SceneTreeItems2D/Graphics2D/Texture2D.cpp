#include "../../../../include/Peach-Core2D/SceneTreeItems2D/Graphics2D/Texture2D.h"

namespace PeachCore {

    Texture2D::Texture2D(const char* imagePath) : bgfxTexture(BGFX_INVALID_HANDLE), pm_Width(0), pm_Height(0) {
        if (!LoadTexture(imagePath)) {
            std::cerr << "Failed to load texture" << std::endl;
            throw std::runtime_error("Failed to load texture");
        }
    }

    Texture2D::~Texture2D() {
        if (bgfx::isValid(bgfxTexture)) {
            bgfx::destroy(bgfxTexture);
        }
    }

    bool Texture2D::LoadTexture(const char* imagePath) 
    {
        int f_NRChannels;
        unsigned char* f_Data = stbi_load(imagePath, &pm_Width, &pm_Height, &f_NRChannels, 4); // Always load 4 channels (RGBA)

        if (!f_Data) {
            LogManager::Logger().Warn("Failed to load texture at: " + std::string(imagePath), "Texture2D");
            return false;
        }

        const bgfx::Memory* f_Mem = bgfx::copy(f_Data, pm_Width * pm_Height * 4);
        bgfxTexture = bgfx::createTexture2D(static_cast<uint16_t>(pm_Width), static_cast<uint16_t>(pm_Height), false, 1, bgfx::TextureFormat::RGBA8, 0, f_Mem);
        
        stbi_image_free(f_Data);
        pm_IsValid = true;

        return true;
    }

    void Texture2D::Bind(const uint8_t slot) const {
        bgfx::setTexture(slot, bgfx::createUniform("s_texture", bgfx::UniformType::Sampler), bgfxTexture);
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

        int cols = pm_Width / m_TileWidth;
        int rows = pm_Height / m_TileHeight;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                float u1 = (float)(x * m_TileWidth) / pm_Width;
                float v1 = (float)(y * m_TileHeight) / pm_Height;
                float u2 = (float)((x + 1) * m_TileWidth) / pm_Width;
                float v2 = (float)((y + 1) * m_TileHeight) / pm_Height;
                m_TileUVs.emplace_back(u1, v1, u2, v2);
            }
        }
    }


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