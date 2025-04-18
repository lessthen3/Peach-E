#pragma once

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#include <Rendering/OpenGLRenderer.h>
#include <Rendering/VulkanRenderer.h>

#include "nuklear/nuklear_impl_opengl3.h"

#include <SDL3/SDL_system.h>
#include <SDL3/SDL_vulkan.h>

#include "ShaderProgram.h"

#include "../2D/PeachCamera2D.h"

#include "stb/stb_image.h"

#include <memory>

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
        SDL_Window* pm_MainWindow = nullptr;
        bool pm_Is3DEnabled = false;

        vector<unique_ptr<PeachCamera2D>> pm_ListOfScenePeachCameras2D; //only the renderer cares about cameras

        map<string, ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which ShaderProgram

        map<string, GLuint> pm_ListOfRegisteredTextures;

        SDL_GLContext pm_OpenGLContext;

        shared_ptr<LogManager> pm_RenderingLogger = nullptr;


    public:

        ~PeachRenderer()
        {
            SDL_GL_DestroyContext(pm_OpenGLContext);

            if (pm_MainWindow) //RenderingManager handles bookeeping and creation, after though each PeachRenderer takes exclusive control over its SDL window
            {
                SDL_DestroyWindow(pm_MainWindow);
                pm_MainWindow = nullptr;
            }

            pm_ListOfScenePeachCameras2D.clear();
            pm_ShaderPrograms.clear();
            pm_ListOfRegisteredTextures.clear();
            pm_RenderingLogger.reset();
        }

        explicit 
            PeachRenderer //peach renderer is never supposed to create an sdl window, it only manages closing it
            (
                SDL_Window* fp_CurrentWindow, 
                shared_ptr<LogManager> fp_RenderingLogger,
                const bool fp_Is3DEnabled = false
            )
        {
            if (not fp_RenderingLogger) //MAYBE: maybe we should just create a new logger actually nvm that involves getting a reference to the console lmfao
            {
                PrintError("Tried to initialize PeachRenderer with a nullptr for the Rendering Logger doofus");
                throw runtime_error("Ending program execution immediately since no valid logger was found"); //idk how else to stop the rest of initialization
            }

            pm_RenderingLogger = fp_RenderingLogger;

            if (not fp_CurrentWindow)
            {
                pm_RenderingLogger->LogAndPrint("Tried to initialize PeachRenderer with a nullptr for the SDL Window doofus", "PeachRenderer", LogManager::LogLevel::Fatal);
                throw runtime_error("Ending program execution immediately since no valid SDL Window was found"); //idk how else to stop the rest of initialization
            }

            pm_MainWindow = fp_CurrentWindow;
            
            pm_Is3DEnabled = fp_Is3DEnabled;

            ////Set Core Profile for OpenGL Context whatever the fuck that means
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__) //Using this in place for now since apple and opengl are kinda janky
            //// Set OpenGL version (e.g., OpenGL 3.3 core profile)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif defined(__APPLE__) // need core profile 3 but even then still kinda doesnt work because apple idk whatever we write metal backend
            //// Set OpenGL version (e.g., OpenGL 3.3 core profile)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
            // Create an OpenGL context associated with the window
            pm_OpenGLContext = SDL_GL_CreateContext(pm_MainWindow);
            SDL_GL_MakeCurrent(pm_MainWindow, pm_OpenGLContext);

            if (not pm_OpenGLContext)
            {
                pm_RenderingLogger->LogAndPrint("Failed to create OpenGL context: " + static_cast<string>(SDL_GetError()), "PeachRenderer", LogManager::LogLevel::Fatal);
                SDL_DestroyWindow(pm_MainWindow);
            }

            pm_RenderingLogger->LogAndPrint("OpenGL initialized properly", "PeachRenderer", LogManager::LogLevel::Debug);

            if (pm_Is3DEnabled) 
            {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
            }

        }

        void
            SetSDLWindow(SDL_Window*& fp_SDLWindow)
        {
            pm_MainWindow = fp_SDLWindow;
        }

        [[nodiscard]] SDL_GLContext*
            GetGLContext()
        {
            return &pm_OpenGLContext;
        }

        void 
            DeleteTexture(const uint32_t fp_TextureID)
        {
            //glDeleteBuffers(fp_TextureID);
        }

        bool 
            DeleteShaderProgram
            (
                const string& fp_ShaderProgramName            
            )
        {
            try //idk lazy way of dealing with repeated deletes of a shader program
            {
                glDeleteProgram(pm_ShaderPrograms.at(fp_ShaderProgramName).GetProgramID());
                pm_ShaderPrograms.erase(fp_ShaderProgramName);
                return true;
            }
            catch (const exception& ex)
            {
                pm_RenderingLogger->LogAndPrint("An error occurred: " + string(ex.what()), "PeachRenderer", LogManager::LogLevel::Warning); //this might not work LOL
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

        void 
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
                pm_RenderingLogger->LogAndPrint("Successfully freed data from: " + fp_PeachObjectID, "PeachRenderer", LogManager::LogLevel::Info);
            }
            else
            {
                pm_RenderingLogger->LogAndPrint("Failed to Register Texture", "PeachRenderer", LogManager::LogLevel::Info);
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
            glUseProgram(fp_Shader.GetProgramID());
            //glEnable(GL_TEXTURE_2D);
            //glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, fp_Texture);
            glBindVertexArray(fp_VAO);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            //glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            //glDisable(GL_TEXTURE_2D);
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

        SDL_Window*&
            GetMainWindow()
        {
            return pm_MainWindow;
        }

    };
}
