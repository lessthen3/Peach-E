#include "../../../../include/Peach-Core2D/SceneTreeItems2D/Graphics2D/Texture2D.h"

namespace PeachCore {

    Texture2D::Texture2D(const char* imagePath) : bgfxTexture(BGFX_INVALID_HANDLE), width(0), height(0) {
        if (!loadTexture(imagePath)) {
            std::cerr << "Failed to load texture" << std::endl;
            throw std::runtime_error("Failed to load texture");
        }
    }

    Texture2D::~Texture2D() {
        if (bgfx::isValid(bgfxTexture)) {
            bgfx::destroy(bgfxTexture);
        }
    }

    bool Texture2D::loadTexture(const char* imagePath) {
        int f_NRChannels;
        unsigned char* f_Data = stbi_load(imagePath, &width, &height, &f_NRChannels, 4); // Always load 4 channels (RGBA)
        if (!f_Data) {
            std::cerr << "Failed to load texture: " << imagePath << std::endl;
            return false;
        }

        const bgfx::Memory* f_Mem = bgfx::copy(f_Data, width * height * 4);
        bgfxTexture = bgfx::createTexture2D(static_cast<uint16_t>(width), static_cast<uint16_t>(height), false, 1, bgfx::TextureFormat::RGBA8, 0, f_Mem);
        stbi_image_free(f_Data);
        return true;
    }

    void Texture2D::bind(uint8_t slot) const {
        bgfx::setTexture(slot, bgfx::createUniform("s_texture", bgfx::UniformType::Sampler), bgfxTexture);
    }

    void Texture2D::unbind() const {
        // Generally, you do not need to unbind textures in BGFX explicitly
    }

    void Texture2D::defineTileSize(int tileWidth, int tileHeight) {
        m_tileWidth = tileWidth;
        m_tileHeight = tileHeight;
        calculateTileUVs();
    }

    void Texture2D::calculateTileUVs() {
        tileUVs.clear();
        int cols = width / m_tileWidth;
        int rows = height / m_tileHeight;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                float u1 = (float)(x * m_tileWidth) / width;
                float v1 = (float)(y * m_tileHeight) / height;
                float u2 = (float)((x + 1) * m_tileWidth) / width;
                float v2 = (float)((y + 1) * m_tileHeight) / height;
                tileUVs.emplace_back(u1, v1, u2, v2);
            }
        }
    }

    std::tuple<float, float, float, float> Texture2D::getTileUV(int tileIndex) const {
        if (tileIndex < 0 || tileIndex >= tileUVs.size()) {
            throw std::out_of_range("Tile index is out of range.");
        }
        return tileUVs[tileIndex];
    }
}