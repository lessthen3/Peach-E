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
#pragma once

#ifdef PEACH_RENDERER_OPENGL

#include <SDL3/SDL.h>

#include "OpenGLShaderProgram.h"
//#include "../Scene-Items/2D/PeachCamera2D.h"

namespace PeachCore::OpenGL {

    enum class TextureFiltering
    {
        Nearest,
        Linear,
        MipMapNearestNearest,
        MipMapLinearNearest,
        MipMapNearestLinear,
        MipMapLinearLinear
    };


    struct ShaderAsset
    {

    };

    struct MeshData
    {
        GLuint VAO;
        GLuint VBO_Positions;
        GLuint VBO_Normals;
        GLuint EBO;
        GLsizei IndexCount;
    };


    class Renderer
    {
    private:
        SDL_Window* pm_MainWindow = nullptr;
        bool pm_Is3DEnabled = false;

        //vector<unique_ptr<PeachCamera2D>> pm_ListOfScenePeachCameras2D; //only the renderer cares about cameras

        unordered_map<string, ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which OpenGLShaderProgram

        unordered_map<string, GLuint> pm_ListOfRegisteredTextures;

        unordered_map<string, MeshData> pm_ListOfMeshes;

        SDL_GLContext pm_OpenGLContext;

        shared_ptr<Logger> rendering_logger = nullptr;

    public:
        ~Renderer() = default;

        //~Renderer()
        //{
        //    SDL_GL_DestroyContext(pm_OpenGLContext);

        //    if (pm_MainWindow) //RenderingManager handles bookeeping and creation, after though each PeachRenderer takes exclusive control over its SDL window
        //    {
        //        SDL_DestroyWindow(pm_MainWindow);
        //        pm_MainWindow = nullptr;
        //    }

        //    //pm_ListOfScenePeachCameras2D.clear();
        //    pm_ShaderPrograms.clear();
        //    pm_ListOfRegisteredTextures.clear();
        //    pm_RenderingLogger.reset();
        //}

        explicit
            Renderer //peach renderer is never supposed to create an sdl window, it only manages closing it
            (
                SDL_Window* fp_CurrentWindow,
                shared_ptr<Logger> fp_RenderingLogger,
                const bool fp_Is3DEnabled = false
            );

        void
            SetMainWindow(SDL_Window* fp_SDLWindow);

        [[nodiscard]] SDL_GLContext*
            GetGLContext();

        void
            DeleteTexture(const uint32_t fp_TextureID);

        bool
            DeleteShaderProgram
            (
                const string& fp_ShaderProgramName
            );

        void
            SetupInstancedArray
            (
                uint32_t instanceVBO,
                const vector<float>& instanceData,
                uint32_t attributeIndex,
                uint32_t size,
                uint32_t instanceDataLength,
                int offset
            )
            const;

        // Example usage: suppose you want to pass a model matrix per instance
        // You would call setupInstancedArray four times, one for each row of the matrix, because glVertexAttribPointer can only handle up to 4 float components at a time.

        void
            GenerateUniformBufferObject();

        GLuint
            RegisterTexture
            (
                const string& fp_PeachObjectID,
                unsigned char* fp_Data,
                const unsigned int fp_Width,
                const unsigned int fp_Height,
                const unsigned int fp_Channels
            );

        void
            DrawTexture
            (
                const ShaderProgram& fp_Shader,
                GLuint fp_VAO,
                GLuint fp_Texture
            );

        void
            DrawShapePrimitive
            (
                const ShaderProgram& fp_Shader,
                const GLuint fp_VAO
            );

        GLuint //returns the vao id
            Generate2DBuffers
            (
                const vector<float>& fp_Vertices,
                const vector<unsigned int>& fp_Indices
            )
            const;

        //WIP
        GLuint //returns the vao id
            Generate3DBuffers
            (
                const vector<float>& fp_Vertices,
                const vector<unsigned int>& fp_Indices
            )
            const;

        ShaderProgram*
            GetShaderProgram(const string& fp_Name);

        bool
            RenderFrame();
    };
}//namespace PeachCore::OpenGL

#endif