#pragma once

#include "../Managers/LogManager.h"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace PeachCore {

    class ShaderProgram {
    public:
        ShaderProgram()
        {
            pm_ProgramID = glCreateProgram();
        }

        ~ShaderProgram() {
            CleanUp();
        }

        ////////////////////////////////////////////////////////////
        //SHADER CREATOR FOR FRAGMENT AND VERTEX SHADERS
        ////////////////////////////////////////////////////////////

        void CreateVertexShader(const std::string fp_ShaderCode)
        {
            pm_Shaders.insert({"VertexShader", CreateShader(fp_ShaderCode, GL_VERTEX_SHADER) });
        }

        void CreateFragmentShader(const std::string fp_ShaderCode)
        {
            pm_Shaders.insert({"FragmentShader", CreateShader(fp_ShaderCode, GL_FRAGMENT_SHADER) });
        }

        GLuint CreateShader(std::string fp_ShaderSourceCode, GLuint fp_ShaderType) //creates, compiles and attaches desired shader type to current shaderprogram
            const
        {
            glUseProgram(pm_ProgramID);
            GLuint f_ShaderID = glCreateShader(fp_ShaderType);

            if (f_ShaderID == 0) 
                {return 0;}

            const char* f_Cstr = fp_ShaderSourceCode.c_str(); //idk why cpp makes me do this in two lines but whatever

            glShaderSource(f_ShaderID, 1, &f_Cstr, NULL);
            glCompileShader(f_ShaderID);

            GLint success;
            GLchar infoLog[512];

            // After glCompileShader(f_ShaderID);
            glGetShaderiv(f_ShaderID, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(f_ShaderID, 512, NULL, infoLog);
                std::cerr << "Shader compilation error: " << infoLog << std::endl;
                return 0; // Or handle the error appropriately
            }

            //glAttachShader(pm_ProgramID, f_ShaderID);

            return f_ShaderID;
        }

        ///////////////////////////////////////////////
        // Generic Uniform Setters 
        ///////////////////////////////////////////////

        void SetUniformMat4(const std::string& fp_UniformName, glm::mat4 matrix) 
        {
            glUniformMatrix4fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(matrix));
        }

        void SetUniform1i(const std::string& fp_UniformName, const int value) { //retrieves uniform and its associated GLuint and sets it
            glUniform1i(pm_Uniforms[fp_UniformName], value);
        }

        void SetUniform1f(const std::string& fp_UniformName, const float value) {
            glUniform1f(pm_Uniforms[fp_UniformName], value);
        }

        void SetUniform3f(const std::string& fp_UniformName, glm::vec3 value) {
            glUniform3f(pm_Uniforms[fp_UniformName], value.x, value.y, value.z);
        }

        void SetUniform4f(const std::string& fp_UniformName, glm::vec4 value) {
            glUniform4f(pm_Uniforms[fp_UniformName], value.x, value.y, value.z, value.w);
        }

        //////////////////////////////////////////////
        // Fog Uniform Creator Setters
        //////////////////////////////////////////////

        //void SetUniform(std::string uniformName, Fog fog) {
        //    SetUniform(uniformName + ".activeFog", fog.isActive() ? 1 : 0);
        //    SetUniform(uniformName + ".colour", fog.getColour());
        //    SetUniform(uniformName + ".density", fog.getDensity());
        //}

        //////////////////////////////////////////////
        // Material Uniform Setters
        //////////////////////////////////////////////

        //void SetMaterialUniforms(std::string uniformName, Material material) {
        //    SetUniform(uniformName + ".ambient", material.getAmbientColour());
        //    SetUniform(uniformName + ".diffuse", material.getDiffuseColour());
        //    SetUniform(uniformName + ".specular", material.getSpecularColour());
        //    SetUniform(uniformName + ".hasTexture", material.isTextured() ? 1 : 0);
        //    SetUniform(uniformName + ".reflectance", material.getReflectance());
        //}

        //////////////////////////////////////////////
        // Texture Uniform Setter
        //////////////////////////////////////////////

        void SetTexture(const std::string& fp_UniformName, GLuint fp_TextureID, GLuint fp_TextureUnit) {
            glActiveTexture(GL_TEXTURE0 + fp_TextureUnit);  // Activate the correct texture unit before binding
            glBindTexture(GL_TEXTURE_2D, fp_TextureID);
            glUniform1i(GetUniformLocation(fp_UniformName), fp_TextureUnit);  // Set the sampler to use the correct texture unit
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //////////////////////////////////////////////
        // Shader Linker 
        //////////////////////////////////////////////

        void Link()   
        {
            glUseProgram(pm_ProgramID);

            for (auto& shader : pm_Shaders)
            {
                glAttachShader(pm_ProgramID, shader.second);
                std::cout << shader.second << "\n";
            }

            glLinkProgram(pm_ProgramID);
            std::cout << "Program ID: " << pm_ProgramID << "Successfully Linked!" << "\n";

            GLint success;
            GLchar infoLog[512];
            
            glGetProgramiv(pm_ProgramID, GL_LINK_STATUS, &success);
            if (!success) 
            {
                glGetProgramInfoLog(pm_ProgramID, 512, NULL, infoLog);
                std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }

            for (auto& shader : pm_Shaders) 
            {
               // glDetachShader(pm_ProgramID, shader.second);
                glDeleteShader(shader.second);  // Delete the shader as it's no longer needed
            }

            glValidateProgram(pm_ProgramID);

            // After glValidateProgram(pm_ProgramID);
            glGetProgramiv(pm_ProgramID, GL_VALIDATE_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(pm_ProgramID, 512, NULL, infoLog);
                std::cerr << "Shader validation error: " << infoLog << std::endl;
                // Handle the error
            }

            AutoCaptureActiveUniforms(); //Does what it says on the tin. please dont ask how many times i cried while writing this class, please just dont'.
        }

        void AutoCaptureActiveUniforms() //gets uniforms detected by current glContext, then puts them into a map of the form <uniform-name, uniformLocation>
        {
            int total = -1;
            glGetProgramiv(pm_ProgramID, GL_ACTIVE_UNIFORMS, &total);

            for (int i = 0; i < total; ++i)
            {
                int name_len = -1, num = -1;
                GLenum type = GL_ZERO;
                char name[100];

                glGetActiveUniform(pm_ProgramID, GLuint(i), sizeof(name) - 1,
                    &name_len, &num, &type, name);

                name[name_len] = 0;
                GLuint location = glGetUniformLocation(pm_ProgramID, name);

                const char* f_temp = name;

                pm_Uniforms.insert({ f_temp, location}); //have to c_str() this our it breaks because i think the string termination is fucked with char[]

                std::cout << "Uniform " << i << " Type: " << type << " Name: " << f_temp << " Location: " << location << "\n";
            }
        }

       //////////////////////////////////////////////
       // Shader Cleanup
       //////////////////////////////////////////////

        void Bind() 
            const
        {
            glUseProgram(pm_ProgramID);
        }

        void Unbind() 
            const
        {
            glUseProgram(0);
        }

        void CleanUp() 
            const
        {
            Unbind();
            if (pm_ProgramID != 0) { glDeleteProgram(pm_ProgramID); }
        }

        //////////////////////////////////////////////
        // load/compilation functions
        //////////////////////////////////////////////

        //loads and compiles one shader, also attaches it to the current ShaderProgram
        bool LoadAndCompileShader(const std::string& kind, const std::string& filePath) 
        {
            // Load the file into a string
            std::ifstream shaderFile(filePath);

            if (!shaderFile)
            {
                LogManager::RenderingLogger().Warn("Failed to open shader file: " + filePath, "ShaderProgram");
                return false;
            }
            std::string source((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
           
            if (kind == "VertexShader")
            {
                CreateVertexShader(source);
            }

            else if (kind == "FragmentShader")
            {
                CreateFragmentShader(source);
            }
            else
            {
                LogManager::RenderingLogger().Warn("Failed to associate loaded shader with pre-defined shader types", "ShaderProgram");
                return false;
            }

            // Compile the source code
            return true;
        }

        //compiles multiple shaders
        bool LoadAndCompileShaders(const std::map<std::string, std::string>& fp_ShadersToLoadAndCompile) //takes a map of "Shader Type" : "Shader Paths"
        {
            glUseProgram(pm_ProgramID);
            for (auto& shader : fp_ShadersToLoadAndCompile)
            {
                LoadAndCompileShader(shader.first, shader.second); //loads shader type from shader path in map
            }

            if (false) {
                std::cerr << "Failed to compile shaders." << std::endl;
                return false;
            }


            return true;
        }

        void PrintShaderProgramDebugVerbose()
            const
        {
            GLint numShaders = 0, numUniforms = 0, maxLength = 0;

            glUseProgram(pm_ProgramID);

            // Get the number of shaders attached to the program
            glGetProgramiv(pm_ProgramID, GL_ATTACHED_SHADERS, &numShaders);
            std::vector<GLuint> shaders(numShaders);
            glGetAttachedShaders(pm_ProgramID, numShaders, nullptr, &shaders[0]);

            std::cout << "Number of shaders attached: " << numShaders << "\n";
            for (GLuint shader : shaders) {
                GLint type;
                glGetShaderiv(shader, GL_SHADER_TYPE, &type);
                std::cout << "Shader ID: " << shader << " Type: " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "\n";
            }

            // Querying all active uniforms
            glGetProgramiv(pm_ProgramID, GL_ACTIVE_UNIFORMS, &numUniforms);
            glGetProgramiv(pm_ProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
            std::vector<char> uniformName(maxLength);

            std::cout << "Active uniforms:\n";
            for (int i = 0; i < numUniforms; i++) {
                GLint size;
                GLenum type;
                glGetActiveUniform(pm_ProgramID, i, maxLength, nullptr, &size, &type, &uniformName[0]);
                GLuint location = glGetUniformLocation(pm_ProgramID, &uniformName[0]);
                std::cout << "Uniform " << i << ": " << &uniformName[0] << " Location: " << location << " Type: " << type << " Size: " << size << "\n";
            }

            // Check link status
            GLint linkStatus;
            glGetProgramiv(pm_ProgramID, GL_LINK_STATUS, &linkStatus);
            if (linkStatus != GL_TRUE) {
                char infoLog[512];
                glGetProgramInfoLog(pm_ProgramID, 512, nullptr, infoLog);
                std::cout << "Linking error: " << infoLog << "\n";
            }
            else {
                std::cout << "Program linked successfully.\n";
            }

            // Validate program
            glValidateProgram(pm_ProgramID);
            GLint validateStatus;
            glGetProgramiv(pm_ProgramID, GL_VALIDATE_STATUS, &validateStatus);
            if (validateStatus != GL_TRUE) {
                char validateInfoLog[512];
                glGetProgramInfoLog(pm_ProgramID, 512, nullptr, validateInfoLog);
                std::cout << "Validation error: " << validateInfoLog << "\n";
            }
            else {
                std::cout << "Program validated successfully.\n";
            }

            glUseProgram(0); // Unbind program after checking
        }

        void PrintShaderProgramUniformList()
        {
            for (auto& uniform : pm_Uniforms)
            {
                std::cout << "Uniform Name: " << uniform.first << "\n";
                std::cout << "Uniform Location: " << uniform.second << "\n";
            }
        }

        GLuint GetUniformLocation(const std::string& fp_UniformName)
        {
            return pm_Uniforms.at(fp_UniformName);
        }

        std::string GetProgramName() const {
            return pm_ProgramName;
        }

        GLuint GetProgramID() 
            const
        {
            return pm_ProgramID;
        }
    
private:
        std::map<std::string, GLuint> pm_Shaders; //stores references to all shader IDs that have been registered with the ShaderProgram
        std::map<std::string, GLuint> pm_Uniforms; //stores all information relevant to program uniforms

        std::string pm_ProgramName;

        GLuint pm_ProgramID;
    };

}



