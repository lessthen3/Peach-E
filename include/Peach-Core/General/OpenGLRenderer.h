#pragma once

#include <SDL2/SDL.h>

#include "../General/ShaderProgram.h"

#include <vector>
#include <iostream>
#include <GL/glew.h>

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

    class OpenGLRenderer
    {
    public:
        explicit OpenGLRenderer(SDL_Window* fp_CurrentWindow, bool fp_Is3DEnabled = false)
            : pm_Is3DEnabled(fp_Is3DEnabled), pm_CurrentWindow(fp_CurrentWindow)
        {
            if (!InitOpenGL())
            {
                LogManager::RenderingLogger().Fatal("Failed to initialize OpenGL.", "OpenGLRenderer");
                throw runtime_error("Failed to initialize OpenGL.");
            }
        }

        ~OpenGLRenderer()
        {
            //glDeleteVertexArrays(1, &pm_VAO);
            //glDeleteBuffers(1, &pm_VBO);
            //glDeleteBuffers(1, &pm_EBO);
            SDL_GL_DeleteContext(pm_OpenGLContext);
        }

        void 
            SetTextureFiltering(GLuint fp_TextureID, TextureFiltering fp_FilterSettings) 
        {
            glBindTexture(GL_TEXTURE_2D, fp_TextureID);

            switch (fp_FilterSettings)
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

        void 
            DrawTexture(const GLuint& fp_TextureID)
        {
            //clear screen to prepare for drawing
            glClearColor(0.27f, 0.5f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | (pm_Is3DEnabled ? GL_DEPTH_BUFFER_BIT : 0));

            // Assuming shaderProgram is active and configured
            ShaderProgram f_CurrentShaderProgram = pm_ShaderPrograms.at("Test_Program");
            glUseProgram(f_CurrentShaderProgram.GetProgramID());

            // Texture setup
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fp_TextureID);
            glUniform1i(f_CurrentShaderProgram.GetUniformLocation("texture_sampler"), 0);

            const auto& f_CurrentVAO = MapOfVisualElements["Test_Texture"];
            glBindVertexArray(get<0>(f_CurrentVAO)); //binds the vertex array that contains all the vbo's we created for the texture

            // Draw the quad
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            // Cleanup
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);

            SDL_GL_SwapWindow(pm_CurrentWindow);
        }

        void 
            RenderFrame() 
        {
            glClearColor(0.27f, 0.5f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | (pm_Is3DEnabled ? GL_DEPTH_BUFFER_BIT : 0));
            SDL_GL_SwapWindow(pm_CurrentWindow);
        }

        GLuint 
            RegisterTexture(const string& fp_PeachObjectID, const unsigned char* fp_Data, const int fp_Width, const int fp_Height, const int fp_Channels)
        {

            //REMEMBER TO IMPLEMENT THIS IN THE CONSTRUCTORS FOR GRAPHICS BASED NODES, AND CLEAR THE DATA AFTERWARDS
            vector<float> f_Vertices =
            {
                -0.5f, -0.5f, 0.0f,  // Bottom-left
                0.5f, -0.5f, 0.0f,   // Bottom-right
                0.5f,  0.5f, 0.0f,   // Top-right
                -0.5f,  0.5f, 0.0f   // Top-left
            };

            vector<uint32_t> f_Indices =
            {
                0, 1, 2,   // First Triangle
                2, 3, 0    // Second Triangle
            };

            vector<float> f_TexCoords =
            {
                 0.0f, 0.0f,  // Bottom-left
                 1.0f, 0.0f,   // Bottom-right
                 1.0f, 1.0f,   // Top-right
                 0.0f, 1.0f   // Top-left
            };
            //normals are all pointing outwards towards the z-direction since all 2d sprites lie on the x-y plane uwu
            vector<float> f_NormalData =
            {
                 0.0f, 0.0f, 1.0f,  // Bottom-left
                0.0f, 0.0f, 1.0f,   // Bottom-right
                0.0f, 0.0f, 1.0f,   // Top-right
               0.0f, 0.0f, 1.0f,   // Top-left
            };

            SetupMeshDataArrays(fp_PeachObjectID, f_Vertices, f_Indices, f_TexCoords, f_NormalData);

            GLuint f_TextureID;

            glGenTextures(1, &f_TextureID);
            glBindTexture(GL_TEXTURE_2D, f_TextureID);

            // Set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GLenum format = GL_RGBA;
            if (fp_Channels == 3) 
            {
                format = GL_RGB;
            }
            else if (fp_Channels == 1) 
            {
                format = GL_RED;
            }
            
            // Upload texture and generate mipmaps
            glTexImage2D(GL_TEXTURE_2D, 0, format, fp_Width, fp_Height, 0, format, GL_UNSIGNED_BYTE, fp_Data);
            glGenerateMipmap(GL_TEXTURE_2D);

            //move the pointer off of the texture
            glBindTexture(GL_TEXTURE_2D, 0);

            // Check for OpenGL errors
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) 
            {
                cerr << "OpenGL error: " << err << endl;
            }

            return f_TextureID; //return a GLuint ID associated with the desired PeachObjectID
        }

        void 
            DeleteTexture(GLuint fp_TextureID)
        {
            //glDeleteBuffers(fp_TextureID);
        }

        bool 
            DeleteShaderProgram(string fp_ShaderProgramName)
        {
            try //idk lazy way of dealing with repeated deletes of a shader program
            {
                glDeleteProgram(pm_ShaderPrograms.at(fp_ShaderProgramName).GetProgramID());
                pm_ShaderPrograms.erase(fp_ShaderProgramName);
                return true;
            }
            catch (const exception& ex)
            {
                LogManager::RenderingLogger().Warn("An error occurred: " + string(ex.what()), "OpenGLRenderer"); //this might not work LOL
                return false;
            }
        }

        void 
            LoadShadersFromSource(const string& fp_Name, const string& fp_VertexSource, const string& fp_FragmentSource) 
        {
            ShaderProgram f_Shader;

            glUseProgram(f_Shader.GetProgramID());

            cout << "program id in openglrenderer " << f_Shader.GetProgramID() << "\n";

            f_Shader.LoadAndCompileShaders({{"VertexShader", fp_VertexSource}, {"FragmentShader", fp_FragmentSource}});
            f_Shader.Link();

            pm_ShaderPrograms.insert({fp_Name, f_Shader});
        }

        ShaderProgram& 
            GetShaderProgram(const string& fp_Name) 
        {
            return pm_ShaderPrograms.at(fp_Name);
        }

        SDL_GLContext*
            GetCurrentOpenGLContext()
        {
            return &pm_OpenGLContext;
        }

    private:
        //WIP
        bool 
            InitOpenGL() //SFML handles context creation and settings for us, we can create a core profile and define major/minor versions there (thank god)
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

            pm_OpenGLContext = SDL_GL_CreateContext(pm_CurrentWindow);

            if (!pm_OpenGLContext) 
            {
                LogManager::RenderingLogger().Fatal("Failed to at SDL_GL_CreateContext: " + string(SDL_GetError()), "OpenGLRenderer");
                throw runtime_error("Failed to create a valid OpenGL context with SDL");
            }

            GLenum err = glewInit();
            if (GLEW_OK != err) 
            {
                //LogManager::RenderingLogger().Fatal("GLEW Error: " + glewGetErrorString(err), "OpenGLRenderer");
                throw runtime_error("Failed to initialize GLEW, but apparently was able to create a valid OpenGL context and a SDL window lmao");
            }

            if (pm_Is3DEnabled) 
            {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
            }
            
            return true;
        }

        ////////////////////////////////////////////////
        //// Texture OpenGL Setup
        ////////////////////////////////////////////////

        void 
            SetupMeshDataArrays
            (const string& fp_PeachObjectID,
             const vector<float>& fp_Vertices, 
             const vector<uint32_t>& fp_Indices,
             const vector<float>& fp_TexCoords,
             const vector<float>& fp_NormalCoords)    
        {
            GLuint f_VAO, f_VBO, f_EBO, f_TBO, f_NBO; //current state of OpenGL

            glGenVertexArrays(1, &f_VAO); //generate  new buffer objects for each object passed in
            glBindVertexArray(f_VAO);

            //HOLY FUCK I CANT BELIEVE I DIDNT USE STATIC CAST HERE AND FUCKED MYSELF ON THE IMPLICIT CONVERSION FROM FLOAT --> VOID*
            //i needed a ptr ref and not an actual val holy shit

            ////////////////////////////////////////////////
            //// Setup Vertex Data Buffer
            ////////////////////////////////////////////////

            glGenBuffers(1, &f_VBO);

            glBindBuffer(GL_ARRAY_BUFFER, f_VBO); //bind vertices to vbo
            glBufferData(GL_ARRAY_BUFFER, fp_Vertices.size() * sizeof(float), fp_Vertices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0); //enable VAO index 0
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // move index of vertex from 1 to 3rd element, size of vert is 5 floats, 0 offset

            ////////////////////////////////////////////////
            //// Setup Element Data Buffer
            ////////////////////////////////////////////////

            glGenBuffers(1, &f_EBO);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, f_EBO); //bind indices for drawing triangles
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, fp_Indices.size() * sizeof(uint32_t), fp_Indices.data(), GL_STATIC_DRAW);

            ////////////////////////////////////////////////
            //// Setup Texture Coordinates Data Buffer
            ////////////////////////////////////////////////

            glGenBuffers(1, &f_TBO);

            glBindBuffer(GL_ARRAY_BUFFER, f_TBO);
            glBufferData(GL_ARRAY_BUFFER, fp_TexCoords.size() * sizeof(float), fp_TexCoords.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

            ////////////////////////////////////////////////
            //// Setup Normals Data Buffer
            ////////////////////////////////////////////////

            glGenBuffers(1, &f_NBO);

            glBindBuffer(GL_ARRAY_BUFFER, f_NBO);
            glBufferData(GL_ARRAY_BUFFER, fp_NormalCoords.size() * sizeof(float), fp_NormalCoords.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

            //insert buffer values associated to whatever PeachObjectID is passed though
            MapOfVisualElements.insert({fp_PeachObjectID, tuple<GLuint, GLuint, GLuint, GLuint, GLuint>(f_VAO, f_VBO, f_EBO, f_TBO, f_NBO)});

            //unbind current buffer and vertex for proper state management
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void 
            SetupInstancedArray(GLuint instanceVBO, const vector<float>& instanceData, int attributeIndex, int size, int instanceDataLength, int offset)
            const
        {
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_STATIC_DRAW);

            uint32_t f_AttribOffsetIndex = offset * sizeof(float);

            glEnableVertexAttribArray(attributeIndex);
            glVertexAttribPointer(attributeIndex, size, GL_FLOAT, GL_FALSE, instanceDataLength * sizeof(float), static_cast<void*>(&f_AttribOffsetIndex));
            glVertexAttribDivisor(attributeIndex, 1);  // This tells OpenGL this is an instanced attribute.

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        // Example usage: suppose you want to pass a model matrix per instance
        // You would call setupInstancedArray four times, one for each row of the matrix, because glVertexAttribPointer can only handle up to 4 float components at a time.

        void
            GenerateUniformBufferObject() //used for uniforms that are shared across multiple shaders
        {
            //GLuint uboIndex;
            //glGenBuffers(1, &uboIndex);
            //glBindBuffer(GL_UNIFORM_BUFFER, uboIndex);
            //glBufferData(GL_UNIFORM_BUFFER, sizeof(data), &data, GL_STATIC_DRAW);
            //glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboIndex);

        }

    private:
        SDL_Window* pm_CurrentWindow;
        SDL_GLContext pm_OpenGLContext;
        bool pm_Is3DEnabled;

        map<string, ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which ShaderProgram
        map< string, tuple<GLuint, GLuint, GLuint, GLuint, GLuint> > MapOfVisualElements; //keeps track of the vao,vbo,ebo id's for each element that is registered
    };
}
