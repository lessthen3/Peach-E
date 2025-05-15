#pragma once

///PeachCore
#include "../Managers/LogManager.h"

///External
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace PeachCore {

    class OpenGLShaderProgram 
    {
    public:
        ~OpenGLShaderProgram()
        {
            if (pm_ProgramID != 0) //delete program if it has been set only
            {
                CleanUp();
            }

            cout << "Destroyed program ID: " << pm_ProgramID << " for object " << this << endl;
        }

        OpenGLShaderProgram(const OpenGLShaderProgram&) = delete;
        OpenGLShaderProgram& operator=(const OpenGLShaderProgram&) = delete;
        //OpenGLShaderProgram(OpenGLShaderProgram&& other) noexcept = default; // Implement this

        OpenGLShaderProgram&
            operator=(OpenGLShaderProgram&& other) //move operator
            noexcept
        {
            if (this != &other)
            {
                // Clean up existing resources if necessary
                if (pm_ProgramID != 0)
                {
                    CleanUp();
                }

                // Transfer object based resources 
                pm_Shaders = move(other.pm_Shaders);  // IDK IF THIS MOVE OPERATION IS KOSCHER
                pm_Uniforms = move(other.pm_Uniforms);
                pm_ProgramName = move(other.pm_ProgramName);

                //Create new copies of primitive types
                pm_ProgramID = other.pm_ProgramID;

                // "Reset" the other object
                other.pm_ProgramID = 0;
            }
            return *this;
        }

        OpenGLShaderProgram(OpenGLShaderProgram&& other) 
            noexcept
            :   
            pm_Shaders(move(other.pm_Shaders)),
            pm_Uniforms(move(other.pm_Uniforms)),
            pm_ProgramName(move(other.pm_ProgramName))
        {
            if (pm_ProgramID != 0) //delete program if it has been set only
            {
                CleanUp();
            }

            pm_ProgramID = other.pm_ProgramID;
            other.pm_ProgramID = 0;
        }


        OpenGLShaderProgram() = default;

    private:
        map<string, GLuint> pm_Shaders; //stores references to all shader IDs that have been registered with the OpenGLShaderProgram
        map<string, GLuint> pm_Uniforms; //stores all information relevant to program uniforms

        string pm_ProgramName;

        GLuint pm_ProgramID = 0;

    public:
        OpenGLShaderProgram
            (
                const string& fp_ShaderName, 
                const string& fp_VertexSourceFilePath, 
                const string& fp_FragmentSourceFilePath,
                LogManager* fp_RenderingLogger
            )
        {
            pm_ProgramName = fp_ShaderName;
            pm_ProgramID = glCreateProgram();

            string f_VertexSourceCode, f_FragmentSourceCode;
            bool f_IsVertexShaderValid = false;
            bool f_IsFragmentShaderValid = false;
            
            ///vertex shader
            if (not ReadFileIntoString(fp_VertexSourceFilePath, &f_VertexSourceCode, fp_RenderingLogger))
            {
                fp_RenderingLogger->LogAndPrint("Unable to read vertex shader code into a string", "OpenGLShaderProgram: " + pm_ProgramName, LogManager::LogLevel::Error);
            }
            else if(CreateVertexShader(f_VertexSourceCode, fp_RenderingLogger))
            {
                f_IsVertexShaderValid = true;
            }
            ///fragment shader
            if (not ReadFileIntoString(fp_FragmentSourceFilePath, &f_FragmentSourceCode, fp_RenderingLogger))
            {
                fp_RenderingLogger->LogAndPrint("Unable to read fragment shader code into a string", "OpenGLShaderProgram: " + pm_ProgramName, LogManager::LogLevel::Error);
            }
            else if (CreateFragmentShader(f_FragmentSourceCode, fp_RenderingLogger))
            {
                f_IsFragmentShaderValid = true;
            }

            if(f_IsVertexShaderValid and f_IsFragmentShaderValid)
            {
                Link(fp_RenderingLogger);
            }
            else
            {
                fp_RenderingLogger->LogAndPrint("Shader failed to link due to invalid shader(s)", "OpenGLShaderProgram: " + pm_ProgramName, LogManager::LogLevel::Error);
            }
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

    private:
        //////////////////////////////////////////////
        // Shader Linker 
        //////////////////////////////////////////////

        void 
            Link
            (
                LogManager* fp_RenderingLogger
            )
        {
            for (auto& shader : pm_Shaders)
            {
                glAttachShader(pm_ProgramID, shader.second);
            }

            glLinkProgram(pm_ProgramID);

            fp_RenderingLogger->LogAndPrint("Successfully Linked!", "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Debug);

            GLint success;
            GLchar infoLog[512];

            glGetProgramiv(pm_ProgramID, GL_LINK_STATUS, &success);
            if (not success)
            {
                glGetProgramInfoLog(pm_ProgramID, 512, NULL, infoLog);
                fp_RenderingLogger->LogAndPrint("ERROR::SHADER::PROGRAM::LINKING_FAILED" + static_cast<string>(infoLog), "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Error);
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
                fp_RenderingLogger->LogAndPrint("Shader validation error: " + static_cast<string>(infoLog), "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Error);
            }

            AutoCaptureActiveUniforms(fp_RenderingLogger);

            pm_Shaders.clear(); //don't need the contents anymore since they are stored inside the GL context currently
        }

        void 
            AutoCaptureActiveUniforms
            (
                LogManager* fp_RenderingLogger
            ) //gets uniforms detected by current glContext, then puts them into a map of the form <uniform-name, uniformLocation>
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

                fp_RenderingLogger->LogAndPrint("Uniform #: " + to_string(i) + ", Type(GLenum): " + to_string(type) + ", Name: " + f_temp + ", Location(GLuint): " + to_string(location), "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Debug);
            }
        }

        ///////////////////////////////////////////////
        // Create Shaders
        ///////////////////////////////////////////////

        bool
            CreateVertexShader
            (
                const string& fp_ShaderCode,
                LogManager* fp_RenderingLogger
            )
        {
            int f_VertexShaderID = CreateShader(fp_ShaderCode, GL_VERTEX_SHADER, fp_RenderingLogger);

            if (not f_VertexShaderID)
            {
                return false;
            }

            pm_Shaders.insert({ "VertexShader", f_VertexShaderID });

            return true;
        }

        bool
            CreateFragmentShader
            (
                const string& fp_ShaderCode,
                LogManager* fp_RenderingLogger
            )
        {
            int f_FragmentShaderID = CreateShader(fp_ShaderCode, GL_FRAGMENT_SHADER, fp_RenderingLogger);

            if (not f_FragmentShaderID)
            {
                return false;
            }

            pm_Shaders.insert({ "FragmentShader", f_FragmentShaderID });

            return true;
        }

        int
            CreateShader
            (
                const string& fp_ShaderSourceCode,
                GLuint fp_ShaderType,
                LogManager* fp_RenderingLogger
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
                fp_RenderingLogger->LogAndPrint("Shader compilation error: " + static_cast<string>(infoLog), "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Error);
                return 0; // Or handle the error appropriately
            }

            return f_ShaderID;
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
                string* fp_SourceCode,
                LogManager* fp_RenderingLogger
            )
            const
        {
            // Extract file extension assuming format "filename.ext"
            size_t lastDotIndex = fp_ScriptFilePath.rfind('.');

            if (lastDotIndex == string::npos)
            {
                fp_RenderingLogger->LogAndPrint("No file extension found for GLSL Shader at specified filepath: " + fp_ScriptFilePath, "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Error);
                return false;
            }

            string f_FileExtension = fp_ScriptFilePath.substr(lastDotIndex);

            if (f_FileExtension != ".fs" and f_FileExtension != ".vs" and f_FileExtension != ".glsl")
            {
                fp_RenderingLogger->LogAndPrint("Invalid file extension found when GLSL Shader was expected at specified filepath: " + fp_ScriptFilePath, "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Error);
                return false;
            }

            ifstream f_FileStream(fp_ScriptFilePath);

            if (not f_FileStream)
            {
                fp_RenderingLogger->LogAndPrint("Shader failed to load at file path: " + fp_ScriptFilePath, "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Error);
                return false;
            }

            stringstream f_Buffer;
            f_Buffer << f_FileStream.rdbuf();
            *fp_SourceCode = f_Buffer.str();

            fp_RenderingLogger->LogAndPrint("Shader successfully loaded at file path: " + fp_ScriptFilePath, "OpenGLShaderProgram: " + to_string(pm_ProgramID) + ":" + pm_ProgramName, LogManager::LogLevel::Debug);

            return true;
        }
    };
}



