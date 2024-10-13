#pragma once

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#include "nuklear_impl_opengl3.h"

#include <SDL3/SDL_system.h>

#include "../General/ShaderProgram.h"

#include "../2D/PeachCamera2D.h"

#include "stb/stb_image.h"

using namespace std;

namespace PeachCore
{
    enum class TextureFiltering
    {
        Nearest,
        Linear,
        MipMapNearestNearest,
        MipMapLinearNearest,
        MipMapNearestLinear,
        MipMapLinearLinear
    };

    class PeachRenderer
    {
    private:
        SDL_Window* pm_MainWindow;
        bool pm_Is3DEnabled;

        vector<unique_ptr<PeachCamera2D>> pm_ListOfScenePeachCameras2D; //only the renderer cares about cameras

        map<string, ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which ShaderProgram

        map<string, GLuint> pm_ListOfRegisteredTextures;

        SDL_GLContext pm_OpenGLContext;


    public:

        ~PeachRenderer()
        {
            if (pm_MainWindow) //RenderingManager handles bookeeping and creation, after though each PeachRenderer takes exclusive control over its SDL window
            {
                SDL_DestroyWindow(pm_MainWindow);
                pm_MainWindow = nullptr;
            }

            SDL_GL_DestroyContext(pm_OpenGLContext);

            pm_ListOfScenePeachCameras2D.clear();
            pm_ShaderPrograms.clear();
        }

        explicit 
            PeachRenderer //peach renderer is never supposed to create an sdl window, it only manages closing it
            (
                SDL_Window* fp_CurrentWindow, 
                const bool fp_Is3DEnabled = false
            )
        {
            pm_MainWindow = fp_CurrentWindow;
            pm_Is3DEnabled = fp_Is3DEnabled;

            // Create an OpenGL context associated with the window
            pm_OpenGLContext = SDL_GL_CreateContext(pm_MainWindow);

            if (not pm_OpenGLContext)
            {
                LogManager::RenderingLogger().LogAndPrint("Failed to create OpenGL context: " + static_cast<string>(SDL_GetError()), "PeachRenderer", "fatal");
                SDL_DestroyWindow(pm_MainWindow);
            }

            LogManager::RenderingLogger().LogAndPrint("OpenGL initialized properly", "PeachRenderer", "debug");

            if (pm_Is3DEnabled) 
            {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
            }

        }

        void
            SetSDLWindow(SDL_Window* fp_SDLWindow)
        {
            pm_MainWindow = fp_SDLWindow;
        }

        void 
            DeleteTexture(const uint32_t fp_TextureID)
        {
            //glDeleteBuffers(fp_TextureID);
        }

        bool 
            DeleteShaderProgram(const string& fp_ShaderProgramName)
        {
            try //idk lazy way of dealing with repeated deletes of a shader program
            {
                glDeleteProgram(pm_ShaderPrograms.at(fp_ShaderProgramName).GetProgramID());
                pm_ShaderPrograms.erase(fp_ShaderProgramName);
                return true;
            }
            catch (const exception& ex)
            {
                LogManager::RenderingLogger().LogAndPrint("An error occurred: " + string(ex.what()), "PeachRenderer", "warn"); //this might not work LOL
                return false;
            }
        }

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
            const
        {

        }

        // Example usage: suppose you want to pass a model matrix per instance
        // You would call setupInstancedArray four times, one for each row of the matrix, because glVertexAttribPointer can only handle up to 4 float components at a time.

        void
            GenerateUniformBufferObject() //used for uniforms that are shared across multiple shaders
        {

        }

        void SetTextureFiltering(GLuint fp_TextureID, TextureFiltering fp_Filter) 
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
            RegisterTexture
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
                stbi_image_free(fp_Data);
            }
            else
            {
                LogManager::RenderingLogger().LogAndPrint("Failed to Register Texture", "PeachRenderer", "error");
            }

            glBindTexture(GL_TEXTURE_2D, 0);

            return f_Texture;
        }

        void
            DrawTexture
            (
                const ShaderProgram& fp_Shader, 
                GLuint fp_VAO, 
                GLuint fp_Texture
            )
        {
            glActiveTexture(fp_Texture); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, fp_Texture);
            glBindVertexArray(fp_VAO);

            glUseProgram(fp_Shader.GetProgramID());

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            //glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        }

        void
            DrawShapePrimitive
            (
                const ShaderProgram& fp_Shader, 
                GLuint fp_VAO
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
            Generate2DBuffers(const vector<float>& fp_Vertices, const vector<unsigned int>& fp_Indices)
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
            Generate3DBuffers(const vector<float>& fp_Vertices, const vector<unsigned int>& fp_Indices)
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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // 0th point start stride by 8, eg 0-3, 8-11, 16-19,. . .
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

        ShaderProgram*
            GetShaderProgram(const string& fp_Name)
        {
            return &pm_ShaderPrograms.at(fp_Name);
        }

        SDL_Window*
            GetMainWindow()
        {
            return pm_MainWindow;
        }

    };
}
