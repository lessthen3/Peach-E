#pragma once



#include <SDL2/SDL.h>
#include <GL/glew.h>
#include<GL/GL.h>
#include <glm/glm.hpp>

#include "../Unsorted/ShaderProgram.h"

#include <vector>
#include <iostream>

namespace PeachCore
{

    struct Vertex {
        glm::vec3 position;
        glm::vec2 texCoord;

        Vertex(float x, float y, float z, float u, float v)
            : position(x, y, z), texCoord(u, v) {}
    };

    enum class TextureFiltering {
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
        OpenGLRenderer(SDL_Window* window, bool enable3D = false)
            : m_window(window), m_enable3D(enable3D)
        {
            if (!InitOpenGL()) {
                throw std::runtime_error("Failed to initialize OpenGL.");
            }
        }

        ~OpenGLRenderer() {
            glDeleteVertexArrays(1, &m_vao);
            glDeleteBuffers(1, &m_vbo);
            glDeleteBuffers(1, &m_ebo);
            SDL_GL_DeleteContext(m_glContext);
        }

        void SetTextureFiltering(GLuint textureId, TextureFiltering filtering) {
            glBindTexture(GL_TEXTURE_2D, textureId);
            switch (filtering) {
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

        void Draw(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, bool is3D = false) 
        {
            SetupVertexArray(vertices, indices);
            glDrawElements(
                is3D ? GL_TRIANGLES : GL_TRIANGLE_STRIP,
                indices.size(),
                GL_UNSIGNED_INT,
                nullptr
            );
        }

        void BatchDraw(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, int numObjects, bool is3D = false) 
        {
            SetupVertexArray(vertices, indices);
            for (int i = 0; i < numObjects; ++i) {
                glDrawElementsBaseVertex(
                    is3D ? GL_TRIANGLES : GL_TRIANGLE_STRIP,
                    indices.size() / numObjects,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(unsigned int) * (i * indices.size() / numObjects)),
                    i * (vertices.size() / numObjects / (is3D ? 3 : 2))
                );
            }
        }

        void DrawTexture(const GLuint& fp_TextureID, const glm::vec2& position, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1) 
        {
            GLuint f_ShaderProgramID = pm_ShaderPrograms.at("INSERT PROGRAM NAME HERE").GetProgramID();
            // Assuming shaderProgram is active and configured
            glUseProgram(f_ShaderProgramID);
            //glBindVertexArray(VAO);  // Bind the VAO

            // Bind the texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fp_TextureID);

            glUniform1i(glGetUniformLocation(f_ShaderProgramID, "TextureSampler"), 0);

            // Bind the VAO that has quad configurations
            //glBindVertexArray(quadVAO);

            //// Setup vertices and texture coordinates based on position, size, and UVs
            Vertex vertices[] = {
                // positions         // texture coords
                {position.x, position.y, 0.0f, uv0.x, uv0.y},
                {position.x + size.x, position.y, 0.0f, uv1.x, uv0.y},
                {position.x + size.x, position.y + size.y, 0.0f, uv1.x, uv1.y},
                {position.x, position.y + size.y, 0.0f, uv0.x, uv1.y},
            };

            // Update vertex data
            //glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            //// Draw the quad
            //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // Assuming you're drawing a quad

            // Draw the quad
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void BatchDrawTexture(const GLuint& fp_TextureID, const std::vector<glm::vec2>& positions, const glm::vec2& size)
        {
            glBindTexture(GL_TEXTURE_2D, fp_TextureID);

            for (const auto& pos : positions) {
                DrawTexture(fp_TextureID, pos, size, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)); //NOT EFFICIENT NEED TO CHANGE TO REDUCE CALL OVERHEAD
            }                                                                                                                          //THIS IS LITERALLY THE WHOLE POINT OF BATCHING

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void RenderFrame() 
        {
            //auto pendingResources = FetchPendingResources();

            //for (const auto& resource : pendingResources) {
            //    if (resource.type == "Texture") {
            //        GLuint textureID;
            //        glGenTextures(1, &textureID);
            //        glBindTexture(GL_TEXTURE_2D, textureID);

            //        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resource.width, resource.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, resource.data.data());
            //        glGenerateMipmap(GL_TEXTURE_2D);

            //        glBindTexture(GL_TEXTURE_2D, 0);

            //        // Store the texture ID in a map or wherever necessary
            //        textureMap[resource.identifier] = textureID;
            //    }
            //    // Handle other resource types similarly
            //}
            //glClearColor(0.27f, 0.5f, 0.8f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | (m_enable3D ? GL_DEPTH_BUFFER_BIT : 0));
            //SDL_GL_SwapWindow(m_window);
        }

        GLuint RegisterTexture(const unsigned char* fp_Data, const int fp_Width, const int fp_Height)
        {
            GLuint f_TestID = 1337;

            glGenTextures(1, &f_TestID);
            glBindTexture(GL_TEXTURE_2D, f_TestID);

            // Set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Upload texture and generate mipmaps
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fp_Width, fp_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fp_Data);
            glGenerateMipmap(GL_TEXTURE_2D);

            //move the pointer off of the texture
            glBindTexture(GL_TEXTURE_2D, 0);

            return f_TestID;
        }

        bool DeleteTexture(GLuint fp_TextureID)
        {
            //glDeleteBuffers(fp_TextureID);
        }

        bool DeleteShaderProgram(std::string fp_ShaderProgramName)
        {
            //if( gl) gonna check if it exists first but idk
            glDeleteProgram(pm_ShaderPrograms.at(fp_ShaderProgramName).GetProgramID());
        }

        void LoadShadersFromSource(const std::string& fp_Name, const std::string& fp_VertexSource, const std::string& fp_FragmentSource) 
        {
            ShaderProgram f_Shader;
            f_Shader.LoadAndCompileShaders({{"VertexShader", fp_VertexSource}, {"FragmentShader", fp_FragmentSource}});
            pm_ShaderPrograms.insert({fp_Name, f_Shader});
        }

        void LoadShadersFromBinary(const std::string& fp_FilePath)
        {
            //TODO: make this fucking code, i want shipped games to load from binaries, and editor projects to load from source
        }

        ShaderProgram& GetShaderProgram(const std::string& fp_Name) {
            return pm_ShaderPrograms.at(fp_Name);
        }

public:


    private:
        SDL_Window* m_window;
        SDL_GLContext m_glContext;
        bool m_enable3D;

        GLuint m_vao, m_vbo, m_ebo;

        std::map<std::string, ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which ShaderProgram

        std::map< std::string, std::tuple<GLuint, GLuint, GLuint> > MapOfVisualElements; //keeps track of the vao,vbo,ebo id's for each element that is drawn



    private:

        bool InitOpenGL() 
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

            m_glContext = SDL_GL_CreateContext(m_window);
            if (!m_glContext) {
                std::cerr << "SDL_GL_CreateContext: " << SDL_GetError() << std::endl;
                return false;
            }

            GLenum err = glewInit();
            if (GLEW_OK != err) {
                std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
                return false;
            }

            if (m_enable3D) {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
            }

            glGenVertexArrays(1, &m_vao);
            glGenBuffers(1, &m_vbo);
            glGenBuffers(1, &m_ebo);

            return true;
        }

        void SetupVertexArray(const std::vector<float>& fp_Vertices, const std::vector<unsigned int>& fp_Indices) 
            const 
        {
            glBindVertexArray(m_vao);

            glBindBuffer(GL_ARRAY_BUFFER, m_vbo); //vertices
            glBufferData(GL_ARRAY_BUFFER, fp_Vertices.size() * sizeof(float), fp_Vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo); //faces eg. triangles
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, fp_Indices.size() * sizeof(unsigned int), fp_Indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // move index of vertex from 1 to 3rd element, size of vert is 5 floats, 0 offset
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texture coord attribute
            glEnableVertexAttribArray(1);                                                                                                       // move index of vertex from 1st to 2nd element, size of vert is 5 floats, 3 offset

            glBindVertexArray(0);
        }

        void SetupInstancedArray(GLuint instanceVBO, const std::vector<float>& instanceData, int attributeIndex, int size, int instanceDataLength, int offset)
            const
        {
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(attributeIndex);
            glVertexAttribPointer(attributeIndex, size, GL_FLOAT, GL_FALSE, instanceDataLength * sizeof(float), (void*)(offset * sizeof(float)));
            glVertexAttribDivisor(attributeIndex, 1);  // This tells OpenGL this is an instanced attribute.

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        // Example usage: suppose you want to pass a model matrix per instance
        // You would call setupInstancedArray four times, one for each row of the matrix, because glVertexAttribPointer can only handle up to 4 float components at a time.

    };
}
