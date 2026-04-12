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
#ifdef PEACH_RENDERER_OPENGL

#include "OpenGLRenderer.h"
#include <fmt/format.h>

namespace PeachCore::OpenGL {

    Renderer::Renderer //peach renderer is never supposed to create an sdl window, it only manages closing it
    (
        SDL_Window* fp_CurrentWindow,
        shared_ptr<Logger> fp_RenderingLogger,
        const bool fp_Is3DEnabled
    )
    {
        if (not fp_RenderingLogger) //MAYBE: maybe we should just create a new logger actually nvm that involves getting a reference to the console lmfao
        {
            PRINT_ERROR("Tried to initialize PeachRenderer with a nullptr for the Rendering Logger doofus");
            throw runtime_error("Ending program execution immediately since no valid logger was found"); //idk how else to stop the rest of initialization
        }

        rendering_logger = fp_RenderingLogger;

        if (not fp_CurrentWindow)
        {
            rendering_logger->Fatal("Tried to initialize PeachRenderer with a nullptr for the SDL Window doofus", "PeachRenderer");
            throw runtime_error("Ending program execution immediately since no valid SDL Window was found"); //idk how else to stop the rest of initialization
        }

        pm_MainWindow = fp_CurrentWindow;

        pm_Is3DEnabled = fp_Is3DEnabled;

        ////Set Core Profile for OpenGL Context whatever the fuck that means
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        //// Set OpenGL version
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        // Create an OpenGL context associated with the window
        pm_OpenGLContext = SDL_GL_CreateContext(pm_MainWindow);
        SDL_GL_MakeCurrent(pm_MainWindow, pm_OpenGLContext);

        if (not pm_OpenGLContext)
        {
            rendering_logger->Fatal(fmt::format("Failed to create OpenGL context: {}", SDL_GetError()), "OpenGL::Renderer");
            SDL_DestroyWindow(pm_MainWindow);
        }

        rendering_logger->Debug("OpenGL initialized properly", "OpenGL::Renderer");

        if (pm_Is3DEnabled)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
    }

    void
        Renderer::SetMainWindow(SDL_Window* fp_SDLWindow)
    {
        pm_MainWindow = fp_SDLWindow;
    }

    void
        Renderer::DeleteTexture(const uint32_t fp_TextureID)
    {
        //glDeleteBuffers(fp_TextureID);
    }

    //bool
    //    Renderer::DeleteShaderProgram
    //    (
    //        const string& fp_ShaderProgramName
    //    )
    //{
    //    try //idk lazy way of dealing with repeated deletes of a shader program
    //    {
    //        glDeleteProgram(pm_ShaderPrograms.at(fp_ShaderProgramName).GetProgramID());
    //        pm_ShaderPrograms.erase(fp_ShaderProgramName);
    //        return true;
    //    }
    //    catch (const exception& ex)
    //    {
    //        rendering_logger->Warning(fmt::format("An error occurred: {}", ex.what()), "OpenGL::Renderer"); //this might not work LOL
    //        return false;
    //    }
    //}

    void
        Renderer::SetupInstancedArray
        (
            uint32_t instanceVBO,
            const vector<float>& instanceData,
            uint32_t attributeIndex,
            uint32_t size,
            uint32_t instanceDataLength,
            int offset
        )
        const
    {

    }

    // Example usage: suppose you want to pass a model matrix per instance
    // You would call setupInstancedArray four times, one for each row of the matrix, because glVertexAttribPointer can only handle up to 4 float components at a time.

    void
        Renderer::GenerateUniformBufferObject() //used for uniforms that are shared across multiple shaders
    {

    }

    static void
        SetTextureFiltering
        (
            GLuint fp_TextureID,
            TextureFiltering fp_Filter
        )
    {
        glBindTexture(GL_TEXTURE_2D, fp_TextureID);

        switch (fp_Filter)
        {
        case TextureFiltering::Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case TextureFiltering::Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case TextureFiltering::MipMapNearestNearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            break;
        case TextureFiltering::MipMapLinearNearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            break;
        case TextureFiltering::MipMapNearestLinear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            break;
        case TextureFiltering::MipMapLinearLinear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }


    GLuint
        Renderer::RegisterTexture
        (
            const string& fp_PeachObjectID,
            unsigned char* fp_Data,
            const unsigned int fp_Width,
            const unsigned int fp_Height,
            const unsigned int fp_Channels
        )
    {
        GLuint f_Texture;

        glGenTextures(1, &f_Texture);
        glBindTexture(GL_TEXTURE_2D, f_Texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum f_ColourFormat = GL_RGBA;

        if (fp_Channels == 3)
        {
            f_ColourFormat = GL_RGB;
        }
        else if (fp_Channels == 1)
        {
            f_ColourFormat = GL_RED;
        }

        if (fp_Data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, f_ColourFormat, fp_Width, fp_Height, 0, f_ColourFormat, GL_UNSIGNED_BYTE, fp_Data);
            glGenerateMipmap(GL_TEXTURE_2D);
            rendering_logger->Info(fmt::format("Successfully freed data from: {}", fp_PeachObjectID), "OpenGL::Renderer");
        }
        else
        {
            rendering_logger->Info("Failed to Register Texture", "OpenGL::Renderer");
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        return f_Texture;
    }

    void
        Renderer::DrawTexture
        (
            const ShaderProgram& fp_Shader,
            GLuint fp_VAO,
            GLuint fp_Texture
        )
    {
        glUseProgram(fp_Shader.GetProgramID());
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, fp_Texture);

        // tell the sampler uniform it lives on texture unit 0
        //GLint f_SamplerLocation = glGetUniformLocation(fp_Shader.GetProgramID(), "u_Texture");

        //if (f_SamplerLocation != -1)
        //{
        //    glUniform1i(f_SamplerLocation, 0);
        //}

        glBindVertexArray(fp_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void
        Renderer::DrawShapePrimitive
        (
            const ShaderProgram& fp_Shader,
            const GLuint fp_VAO
        )
    {
        glBindVertexArray(fp_VAO);

        glUseProgram(fp_Shader.GetProgramID());

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    GLuint //returns the vao id
        Renderer::Generate2DBuffers
        (
            const vector<float>& fp_Vertices,
            const vector<unsigned int>& fp_Indices
        )
        const
    {
        GLuint vbo;
        glGenBuffers(1, &vbo);

        GLuint vao;
        glGenVertexArrays(1, &vao);

        GLuint ebo;
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * fp_Indices.size(), fp_Indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fp_Vertices.size(), fp_Vertices.data(), GL_STATIC_DRAW);

        // position coord attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0); //IMPORTANT: REMEMBER TO ALWAYS UNBIND VERTEX ARRAY FIRST SINCE UNBINDING ANYTHING INSIDE OF IT BEFOREHAND WILL DE CONFIGURE IT
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return vao;
    }

    //WIP
    GLuint //returns the vao id
        Renderer::Generate3DBuffers
        (
            const vector<float>& fp_Vertices,
            const vector<unsigned int>& fp_Indices
        )
        const
    {
        GLuint vbo;
        glGenBuffers(1, &vbo);

        GLuint vao;
        glGenVertexArrays(1, &vao);

        GLuint ebo;
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * fp_Indices.size(), fp_Indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fp_Vertices.size(), fp_Vertices.data(), GL_STATIC_DRAW);

        // position coord attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // 0th point start stride by 8, eg 0-3, 8-11, 16-19, . . .
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //3rd point start, stride by 8, eg. 3-5, 11-13, 19-21, . . .
        glEnableVertexAttribArray(1);

        // normal coord attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); //5th point start, stride by 8, eg. 5-8, 13-16, 21-24, . . .
        glEnableVertexAttribArray(2);

        glBindVertexArray(0); //IMPORTANT: REMEMBER TO ALWAYS UNBIND VERTEX ARRAY FIRST SINCE UNBINDING ANYTHING INSIDE OF IT BEFOREHAND WILL DE CONFIGURE IT
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return vao;
    }

    bool
        Renderer::RenderFrame()
    {

        return true;
    }

}//namespace PeachCore::OpenGL

#endif