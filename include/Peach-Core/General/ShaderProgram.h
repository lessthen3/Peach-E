#pragma once

#include "../Managers/LogManager.h"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>


using namespace std;

namespace PeachCore {

    class ShaderProgram 
    {
    public:
        ~ShaderProgram()
        {
            CleanUp();
        }

        ShaderProgram() = default;

    private:
        map<string, GLuint> pm_Shaders; //stores references to all shader IDs that have been registered with the ShaderProgram
        map<string, GLuint> pm_Uniforms; //stores all information relevant to program uniforms

        string pm_ProgramName;

        GLuint pm_ProgramID = 0;

    public:
        ShaderProgram
            (
                const string& fp_ShaderName, 
                const string& fp_VertexSourceFilePath, 
                const string& fp_FragmentSourceFilePath
            )
        {
            pm_ProgramName = fp_ShaderName;
            pm_ProgramID = glCreateProgram();

            string f_VertexSourceCode, f_FragmentSourceCode;
            
            ReadFileIntoString(fp_VertexSourceFilePath, &f_VertexSourceCode);
            ReadFileIntoString(fp_FragmentSourceFilePath, &f_FragmentSourceCode);

            CreateVertexShader(f_VertexSourceCode);
            CreateFragmentShader(f_FragmentSourceCode);

            Link();
        }

        ///////////////////////////////////////////////
        // Create Shaders
        ///////////////////////////////////////////////

        void CreateVertexShader(const string& fp_ShaderCode)
        {
            pm_Shaders.insert({"VertexShader", CreateShader(fp_ShaderCode, GL_VERTEX_SHADER) });
        }

        void CreateFragmentShader(const string& fp_ShaderCode)
        {
            pm_Shaders.insert({"FragmentShader", CreateShader(fp_ShaderCode, GL_FRAGMENT_SHADER) });
        }

        int 
            CreateShader
            (
                const string& fp_ShaderSourceCode, 
                GLuint fp_ShaderType
            ) //creates, compiles and attaches desired shader type to current shaderprogram
            const
        {
            int f_ShaderID = glCreateShader(fp_ShaderType);

            if (f_ShaderID == 0) 
            {
                return 0;
            }

            const char* f_Cstr = fp_ShaderSourceCode.c_str(); //idk why cpp makes me do this in two lines but whatever

            glShaderSource(f_ShaderID, 1, &f_Cstr, NULL);
            glCompileShader(f_ShaderID);

            int success;
            GLchar infoLog[512];

            // After glCompileShader(f_ShaderID);
            glGetShaderiv(f_ShaderID, GL_COMPILE_STATUS, &success);

            if (not success)
            {
                glGetShaderInfoLog(f_ShaderID, 512, NULL, infoLog);
                LogManager::RenderingLogger().LogAndPrint("Shader compilation error: " + static_cast<string>(infoLog), "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "error");
                return 0; // Or handle the error appropriately
            }

            return f_ShaderID;
        }

        ///////////////////////////////////////////////
        // Generic Uniform Setters 
        ///////////////////////////////////////////////

        /// Matrices 

        void 
            SetUniform(const string& fp_UniformName, const glm::mat4& fp_Matrix) 
        {
            glUniformMatrix4fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        void
            SetUniform(const string& fp_UniformName, const glm::mat3& fp_Matrix)
        {
            glUniformMatrix3fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        void
            SetUniform(const string& fp_UniformName, const glm::mat2& fp_Matrix)
        {
            glUniformMatrix2fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        void
            SetUniform(const string& fp_UniformName, const glm::mat2x3& fp_Matrix)
        {
            glUniformMatrix2x3fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        void
            SetUniform(const string& fp_UniformName, const glm::mat3x2& fp_Matrix)
        {
            glUniformMatrix3x2fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        void
            SetUniform(const string& fp_UniformName, const glm::mat2x4& fp_Matrix)
        {
            glUniformMatrix2x4fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        void
            SetUniform(const string& fp_UniformName, const glm::mat4x2& fp_Matrix)
        {
            glUniformMatrix4x2fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        void
            SetUniform(const string& fp_UniformName, const glm::mat3x4& fp_Matrix)
        {
            glUniformMatrix3x4fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        void
            SetUniform(const string& fp_UniformName, const glm::mat4x3& fp_Matrix)
        {
            glUniformMatrix4x3fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(fp_Matrix));
        }

        /// int vecs 

        void 
            SetUniform(const string& fp_UniformName, const int fp_Value)
        {
            glUniform1i(pm_Uniforms.at(fp_UniformName), fp_Value);
        }

        void
            SetUniform(const string& fp_UniformName, const glm::ivec2& fp_Value)
        {
            glUniform2i(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y);
        }

        void
            SetUniform(const string& fp_UniformName, const glm::ivec3& fp_Value)
        {
            glUniform3i(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y, fp_Value.z);
        }

        void
            SetUniform(const string& fp_UniformName, const glm::ivec4& fp_Value)
        {
            glUniform4i(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y, fp_Value.z, fp_Value.w);
        }

        /// bool vecs 

        void
            SetUniform(const string& fp_UniformName, const bool fp_Value)
        {
            glUniform1i(pm_Uniforms.at(fp_UniformName), fp_Value);
        }

        void
            SetUniform(const string& fp_UniformName, const glm::bvec2& fp_Value)
        {
            glUniform2i(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y);
        }

        void
            SetUniform(const string& fp_UniformName, const glm::bvec3& fp_Value)
        {
            glUniform3i(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y, fp_Value.z);
        }

        void
            SetUniform(const string& fp_UniformName, const glm::bvec4& fp_Value)
        {
            glUniform4i(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y, fp_Value.z, fp_Value.w);
        }

        /// float vecs 

        void 
            SetUniform(const string& fp_UniformName, const float fp_Value) 
        {
            glUniform1f(pm_Uniforms.at(fp_UniformName), fp_Value);
        }

        void
            SetUniform(const string& fp_UniformName, const glm::vec2& fp_Value)
        {
            glUniform2f(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y);
        }

        void 
            SetUniform(const string& fp_UniformName, const glm::vec3& fp_Value) 
        {
            glUniform3f(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y, fp_Value.z);
        }

        void 
            SetUniform(const string& fp_UniformName, const glm::vec4& fp_Value) 
        {
            glUniform4f(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y, fp_Value.z, fp_Value.w);
        }

        //////////////////////////////////////////////
        // Fog Uniform Creator Setters
        //////////////////////////////////////////////

        //void SetUniform(string uniformName, Fog fog) {
        //    SetUniform(uniformName + ".activeFog", fog.isActive() ? 1 : 0);
        //    SetUniform(uniformName + ".colour", fog.getColour());
        //    SetUniform(uniformName + ".density", fog.getDensity());
        //}

        //////////////////////////////////////////////
        // Material Uniform Setters
        //////////////////////////////////////////////

        //void SetMaterialUniforms(string uniformName, Material material) {
        //    SetUniform(uniformName + ".ambient", material.getAmbientColour());
        //    SetUniform(uniformName + ".diffuse", material.getDiffuseColour());
        //    SetUniform(uniformName + ".specular", material.getSpecularColour());
        //    SetUniform(uniformName + ".hasTexture", material.isTextured() ? 1 : 0);
        //    SetUniform(uniformName + ".reflectance", material.getReflectance());
        //}

        //////////////////////////////////////////////
        // Texture Uniform Setter
        //////////////////////////////////////////////

        void 
            SetTexture
            (
                const string& fp_UniformName, 
                int fp_TextureID, 
                int fp_TextureUnit
            ) 
        {

        }

        //////////////////////////////////////////////
        // Shader Linker 
        //////////////////////////////////////////////

        void 
            Link()
        {
            for (auto& shader : pm_Shaders)
            {
                glAttachShader(pm_ProgramID, shader.second);
            }

            glLinkProgram(pm_ProgramID);

            LogManager::RenderingLogger().LogAndPrint("Successfully Linked!", "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "debug");

            GLint success;
            GLchar infoLog[512];

            glGetProgramiv(pm_ProgramID, GL_LINK_STATUS, &success);
            if (not success)
            {
                glGetProgramInfoLog(pm_ProgramID, 512, NULL, infoLog);
                LogManager::RenderingLogger().LogAndPrint("ERROR::SHADER::PROGRAM::LINKING_FAILED" + static_cast<string>(infoLog), "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "error");
            }

            for (auto& shader : pm_Shaders)
            {
                glDetachShader(pm_ProgramID, shader.second);
                glDeleteShader(shader.second);  // Delete the shader as it's no longer needed
            }

            glValidateProgram(pm_ProgramID);

            glGetProgramiv(pm_ProgramID, GL_VALIDATE_STATUS, &success);
            if (not success)
            {
                glGetProgramInfoLog(pm_ProgramID, 512, NULL, infoLog);
                LogManager::RenderingLogger().LogAndPrint("Shader validation error: " + static_cast<string>(infoLog), "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "error");
            }

            AutoCaptureActiveUniforms();

            pm_Shaders.clear(); //don't need the contents anymore since they are stored inside the GL context currently
        }

        void 
            AutoCaptureActiveUniforms() //gets uniforms detected by current glContext, then puts them into a map of the form <uniform-name, uniformLocation>
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

                pm_Uniforms.insert({ f_temp, location });

                LogManager::RenderingLogger().LogAndPrint("Uniform #: " + to_string(i) + ", Type(GLenum): " + to_string(type) + ", Name: " + f_temp + ", Location(GLuint): " + to_string(location), "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "debug");
            }
        }

       //////////////////////////////////////////////
       // Shader Cleanup
       //////////////////////////////////////////////

        void 
            CleanUp()
            const
        {
            glDeleteProgram(pm_ProgramID);
        }

       //////////////////////////////////////////////
       // Utility Functions
       //////////////////////////////////////////////

        bool
            ReadFileIntoString
            (
                const string& fp_ScriptFilePath,
                string* fp_SourceCode
            )
            const
        {
            // Extract file extension assuming format "filename.ext"
            size_t lastDotIndex = fp_ScriptFilePath.rfind('.');

            if (lastDotIndex == string::npos)
            {
                PeachCore::LogManager::RenderingLogger().LogAndPrint("No file extension found for GLSL Shader at specified filepath: " + fp_ScriptFilePath, "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "error");
                return false;
            }

            string f_FileExtension = fp_ScriptFilePath.substr(lastDotIndex);

            if (f_FileExtension != ".fs" and f_FileExtension != ".vs" and f_FileExtension != ".glsl")
            {
                PeachCore::LogManager::RenderingLogger().LogAndPrint("Invalid file extension found when GLSL Shader was expected at specified filepath: " + fp_ScriptFilePath, "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "error");
                return false;
            }

            ifstream f_FileStream(fp_ScriptFilePath);

            if (not f_FileStream)
            {
                PeachCore::LogManager::RenderingLogger().LogAndPrint("Shader failed to load at file path: " + fp_ScriptFilePath, "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "error");
                return false;
            }

            stringstream f_Buffer;
            f_Buffer << f_FileStream.rdbuf();
            *fp_SourceCode = f_Buffer.str();

            PeachCore::LogManager::RenderingLogger().LogAndPrint("Shader successfully loaded at file path: " + fp_ScriptFilePath, "ShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, "debug");

            return true;
        }

        void 
            PrintShaderProgramUniformList()
        {
            for (auto& uniform : pm_Uniforms)
            {
                cout << "Uniform Name: " << uniform.first << "\n";
                cout << "Uniform Location: " << uniform.second << "\n";
            }
        }

        int 
            GetUniformLocation(const string& fp_UniformName)
        {
            return pm_Uniforms.at(fp_UniformName);
        }

        string 
            GetProgramName() 
            const 
        {
            return pm_ProgramName;
        }

        int 
            GetProgramID() 
            const
        {
            return pm_ProgramID;
        }
    };
}



