/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#ifdef PEACH_RENDERER_OPENGL

///SDL
#include <SDL3/SDL.h>

///PeachCore
#include "OpenGLShaderProgram.h"
#include "rendering/Camera.h"
#include "managers/ResourceManager.h"
#include "debug/StatusCodes.h"

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

    struct TextureSlot
    {
        TextureID ID;
    };

    struct MeshSlot
    {
        MeshID ID;
    };

    struct RenderObject //Fits within one cache line >w<
    {
        GLuint VAO = 0;
        GLuint PositionsVBO = 0;
        GLuint NormalsVBO = 0;
        GLuint EBO = 0;
        GLsizei IndexCount;

        uint32_t ShaderProgramID = 0; //doesn't need to be serialized is pure runtime, also this decouples shaders and objects so shaders can be bound in groups and drawn owo
        uint32_t Generation = 0; 

        bool InUse = false;
        uint8_t Flags = 0;
    };


    class Renderer
    {
    private:
        unique_ptr<Logger> gl_logger = nullptr;

        SDL_Window* pm_MainWindow = nullptr;
        SDL_GLContext pm_OpenGLContext;

        vector<Camera2D> pm_Camera2Ds; //only the renderer cares about cameras
        vector<ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which OpenGLShaderProgram

        ///unsUre
        vector<TextureSlot> pm_TextureSlots; //indexed via TextureID's, if a script wants to access a texture or remap UV's it'll use the TextureID and ask renderingmanager to do that owo
        vector<MeshSlot> pm_MesheSlots;

        std::vector<RenderObject> pm_RenderObjects;

        bool pm_Is3DEnabled = false;

    public:
        ~Renderer() = default; //driver + OS will clean things up faster tbh
        Renderer() = default;

        [[nodiscard]] PEACH_STATUS_CODE //peach renderer is never supposed to create an sdl window, it only manages closing it
            Initialize 
            (
                SDL_Window* fp_CurrentWindow,
                SDL_GLContext fp_OpenGLContext,
                const bool fp_Is3DEnabled,
                const string& fp_LogOutputDirectory
            );

        void
            UpdateForNewThread(); 

        void
            DeleteTexture(const uint32_t fp_TextureID);

        void
            CleanUp();

        //bool
        //    DeleteShaderProgram
        //    (
        //        const string& fp_ShaderProgramName
        //    );

        // void
        //     SetupInstancedArray
        //     (
        //         uint32_t instanceVBO,
        //         const vector<float>& instanceData,
        //         uint32_t attributeIndex,
        //         uint32_t size,
        //         uint32_t instanceDataLength,
        //         int offset
        //     )
        //     const;

        // Example usage: suppose you want to pass a model matrix per instance
        // You would call setupInstancedArray four times, one for each row of the matrix, because glVertexAttribPointer can only handle up to 4 float components at a time.

        void
            GenerateUniformBufferObject();

        GLuint
            RegisterTexture
            (
                const string& fp_PeachObjectID,
                unique_ptr<uint8_t>&& fp_Data,
                const uint32_t fp_Width,
                const uint32_t fp_Height,
                const uint32_t fp_Channels
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
                const vector<uint32_t>& fp_Indices
            )
            const;

        //WIP
        GLuint //returns the vao id
            Generate3DBuffers
            (
                const vector<float>& fp_Vertices,
                const vector<uint32_t>& fp_Indices
            )
            const;

        bool
            RenderFrame();
    };
}//namespace PeachCore::OpenGL

#endif