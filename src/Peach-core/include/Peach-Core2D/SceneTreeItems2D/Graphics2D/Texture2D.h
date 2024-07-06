#pragma once



#include <SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <memory>
#include <iostream>

namespace PeachCore {


    // SDL window dimensions
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // SDL window and BGFX variables
    SDL_Window* window = nullptr;
    bgfx::RendererType::Enum bgfxRendererType = bgfx::RendererType::Count; // Initialized in initBGFX
    bgfx::Init bgfxInit;

    // BGFX variables
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    bgfx::ProgramHandle program;

    // Vertex shader source
    const char* vertexShaderSource = R"(
    attribute vec2 a_position;
    attribute vec2 a_texCoord;
    varying vec2 v_texCoord;
    void main() {
        gl_Position = vec4(a_position, 0.0, 1.0);
        v_texCoord = a_texCoord;
    }
)";

    // Fragment shader source
    const char* fragmentShaderSource = R"(
    varying vec2 v_texCoord;
    uniform sampler2D s_texture;
    void main() {
        gl_FragColor = texture2D(s_texture, v_texCoord);
    }
)";

    // Vertex data for a simple quad (x, y, u, v)
    float vertices[] = {
        -0.5f,  0.5f,  0.0f, 1.0f, // Top-left
         0.5f,  0.5f,  1.0f, 1.0f, // Top-right
         0.5f, -0.5f,  1.0f, 0.0f, // Bottom-right
        -0.5f, -0.5f,  0.0f, 0.0f  // Bottom-left
    };

    uint16_t indices[] = { 0, 1, 2,  0, 2, 3 }; // Triangle list

    // Texture2D class encapsulating BGFX texture
    struct Texture2D {
    public:
        Texture2D(const char* imagePath) {
            // Load image using stb_image
            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true); // Flip the image vertically if needed
            unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
            if (!data) {
                std::cerr << "Failed to load texture: " << imagePath << std::endl;
                throw std::runtime_error("Failed to load texture");
            }

            // Create BGFX texture
            bgfxTexture = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, 0,
                bgfx::copy(data, width * height * 4));

            // Free image data
            stbi_image_free(data);

            // Store texture dimensions
            this->width = width;
            this->height = height;
        }

        ~Texture2D() {
            bgfx::destroy(bgfxTexture);
        }

        void bind() const {
            //bgfx::setTexture(0, programUniform, bgfxTexture);
        }

        void unbind() const {
            //bgfx::setTexture(0, programUniform, bgfx::TextureHandle());
        }

        bgfx::TextureHandle getBGFXHandle() const {
            return bgfxTexture;
        }

        int getWidth() const { return width; }
        int getHeight() const { return height; }

    private:
        bgfx::TextureHandle bgfxTexture;
        int width;
        int height;
    };
}