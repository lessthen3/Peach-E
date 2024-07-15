#pragma once

#include "../Unsorted/ShaderProgram.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

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
        explicit OpenGLRenderer(sf::RenderWindow* fp_Window, bool fp_Is3DEnabled = false)
            : pm_CurrentWindow(fp_Window), pm_Is3DEnabled(fp_Is3DEnabled) 
        {
           /* if (!InitOpenGL())
            {
                throw std::runtime_error("Failed to initialize OpenGL.");
            }*/
        }

        ~OpenGLRenderer() {
            //glDeleteVertexArrays(1, &pm_VAO);
            //glDeleteBuffers(1, &pm_VBO);
            //glDeleteBuffers(1, &pm_EBO);
            
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
            //SetupVertexArray(vertices, indices); WHY, WHY ,WHY ,WHY YYYYYYYYYYYYYYYYYYYYYYY
            glDrawElements(
                is3D ? GL_TRIANGLES : GL_TRIANGLE_STRIP,
                indices.size(),
                GL_UNSIGNED_INT,
                nullptr
            );
        }

        void BatchDraw(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, int numObjects, bool is3D = false) 
        {
            //SetupVertexArray(vertices, indices); this should be called once, not every time we draw thats fucking retarded
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

        void DrawTexture(const GLuint& fp_TextureID) 
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

            //// Setup vertices and texture coordinates based on position, size, and UVs
            //Vertex vertices[] = {
            //    // positions         // texture coords
            //    {position.x, position.y, 0.0f, uv0.x, uv0.y},
            //    {position.x + size.x, position.y, 0.0f, uv1.x, uv0.y},
            //    {position.x + size.x, position.y + size.y, 0.0f, uv1.x, uv1.y},
            //    {position.x, position.y + size.y, 0.0f, uv0.x, uv1.y},
            //};

            Vertex vertices[] =
            {
                {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},  // Bottom-left
                {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},   // Bottom-right
                {0.5f,  0.5f, 0.0f, 1.0f, 1.0f},   // Top-right
                {-0.5f,  0.5f, 0.0f, 0.0f, 1.0f}   // Top-left
            };

            unsigned int indices[] =
            {
                0, 1, 2,   // First Triangle
                2, 3, 0    // Second Triangle
            };

            //bind buffers and throw data inside of them
            glBindVertexArray(pm_VAO);

            glBindBuffer(GL_ARRAY_BUFFER, pm_VBO); //bind vertices to vbo
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pm_EBO); //bind indices for drawing triangles
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0); // move index of vertex from 1 to 3rd element, size of vert is 5 floats, 0 offset
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // Texture coord attribute
            glEnableVertexAttribArray(1);

            // Draw the quad
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            // Cleanup
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);

            pm_CurrentWindow->display();
        }

        void BatchDrawTexture(const GLuint& fp_TextureID, const std::vector<glm::vec2>& positions, const glm::vec2& size)
        {
            glBindTexture(GL_TEXTURE_2D, fp_TextureID);

            for (const auto& pos : positions) {
                //DrawTexture(fp_TextureID, pos, size, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)); //NOT EFFICIENT NEED TO CHANGE TO REDUCE CALL OVERHEAD
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

            glClearColor(0.27f, 0.5f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | (pm_Is3DEnabled ? GL_DEPTH_BUFFER_BIT : 0));
            pm_CurrentWindow->display();
        }

        GLuint RegisterTexture(const unsigned char* fp_Data, const int fp_Width, const int fp_Height, const int fp_Channels)
        {
            GLuint f_TextureID;

            glGenTextures(1, &f_TextureID);
            glBindTexture(GL_TEXTURE_2D, f_TextureID);

            // Set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GLenum format = GL_RGBA;
            if (fp_Channels == 3) {
                format = GL_RGB;
            }
            else if (fp_Channels == 1) {
                format = GL_RED;
            }
            
            // Upload texture and generate mipmaps
            glTexImage2D(GL_TEXTURE_2D, 0, format, fp_Width, fp_Height, 0, format, GL_UNSIGNED_BYTE, fp_Data);
            glGenerateMipmap(GL_TEXTURE_2D);

            //move the pointer off of the texture
            glBindTexture(GL_TEXTURE_2D, 0);

            // Check for OpenGL errors
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << err << std::endl;
            }

            return f_TextureID;
        }

        void DeleteTexture(GLuint fp_TextureID)
        {
            //glDeleteBuffers(fp_TextureID);
        }

        bool DeleteShaderProgram(std::string fp_ShaderProgramName)
        {
            //if( gl) gonna check if it exists first but idk
            glDeleteProgram(pm_ShaderPrograms.at(fp_ShaderProgramName).GetProgramID());
            pm_ShaderPrograms.erase(fp_ShaderProgramName);
            return true;
        }

        void LoadShadersFromSource(const std::string& fp_Name, const std::string& fp_VertexSource, const std::string& fp_FragmentSource) 
        {
            ShaderProgram f_Shader;
            glUseProgram(f_Shader.GetProgramID());
            std::cout << "program id in openglrenderer " << f_Shader.GetProgramID() << "\n";
            f_Shader.LoadAndCompileShaders({{"VertexShader", fp_VertexSource}, {"FragmentShader", fp_FragmentSource}});
            f_Shader.Link();
            pm_ShaderPrograms.insert({fp_Name, f_Shader});
        }

        void LoadShadersFromBinary(const std::string& fp_FilePath)
        {
            //TODO: make this fucking code, i want shipped games to load from binaries, and editor projects to load from source
        }

        ShaderProgram& GetShaderProgram(const std::string& fp_Name) {
            return pm_ShaderPrograms.at(fp_Name);
        }

    


    private:
        sf::RenderWindow* pm_CurrentWindow;
        bool pm_Is3DEnabled;

        GLuint pm_VAO, pm_VBO, pm_EBO; //current state of OpenGL

        std::map<std::string, ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which ShaderProgram

        std::map< std::string, std::tuple<GLuint, GLuint, GLuint> > MapOfVisualElements; //keeps track of the vao,vbo,ebo id's for each element that is registered



    private:

        bool InitOpenGL() //SFML handles context creation and settings for us, we can create a core profile and define major/minor versions there (thank god)
        {
            if (pm_Is3DEnabled) {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
            }

            glGenVertexArrays(1, &pm_VAO); //generate  new buffer objects for each object passed in
            glGenBuffers(1, &pm_VBO);
            glGenBuffers(1, &pm_EBO);
            
            return true;
        }

        void SetupVertexArray(std::string fp_Name, const std::vector<float>& fp_Vertices, const std::vector<unsigned int>& fp_Indices)    
        {
            glGenVertexArrays(1, &pm_VAO); //generate  new buffer objects for each object passed in
            glGenBuffers(1, &pm_VBO);
            glGenBuffers(1, &pm_EBO);

            glBindVertexArray(pm_VAO);

            glBindBuffer(GL_ARRAY_BUFFER, pm_VBO); //bind vertices to vbo
            glBufferData(GL_ARRAY_BUFFER, fp_Vertices.size() * sizeof(float), fp_Vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pm_EBO); //bind indices for drawing triangles
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, fp_Indices.size() * sizeof(unsigned int), fp_Indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // move index of vertex from 1 to 3rd element, size of vert is 5 floats, 0 offset
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texture coord attribute
            glEnableVertexAttribArray(1);                                                                                                       // move index of vertex from 1st to 2nd element, size of vert is 5 floats, 3 offset

            MapOfVisualElements.insert({fp_Name, std::tuple<GLuint, GLuint, GLuint>(pm_VAO, pm_VBO, pm_EBO)});

            glBindBuffer(GL_ARRAY_BUFFER, 0);
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
