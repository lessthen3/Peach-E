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

///PeachCore
#include "Utils/Logger.h"
#include "Rendering/Fog.h"
#include "Rendering/PeachMaterial.h"

///External
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace PeachCore::OpenGL {

    class ShaderProgram 
    {
    private:
        unordered_map<string, GLuint> pm_Shaders; //stores references to all shader IDs that have been registered with the OpenGL::ShaderProgram
        unordered_map<string, GLuint> pm_Uniforms; //stores all information relevant to program uniforms

        string pm_ProgramName;

        GLuint pm_ProgramID = 0;

    public:
        ~ShaderProgram(); //cleaning up should be kosher since OpenGL runs single threaded anyways owo
        ShaderProgram() = default;

        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;
        ShaderProgram(ShaderProgram&& other) noexcept = default; // Implement this

    public:
        ShaderProgram
        (
            const string& fp_ShaderName,
            const string& fp_VertexSourceFilePath,
            const string& fp_FragmentSourceFilePath,
            Logger* logger
        );

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

        void
            SetUniform
            (
                const string& fp_UniformName,
                const Fog3D& fp_Fog
            );

        //////////////////////////////////////////////
        // Material Uniform Setters
        //////////////////////////////////////////////

        void
            SetMaterialUniforms
            (
                const string& fp_UniformName,
                const PeachMaterial& fp_Material
            );

        //////////////////////////////////////////////
        // Texture Uniform Setter
        //////////////////////////////////////////////

        void
            SetTexture
            (
                const string& fp_UniformName,
                int fp_TextureID,
                int fp_TextureUnit
            );

#ifdef PEACH_DEBUG
        void
            PrintShaderProgramUniformList()
        {
            for (auto& lv_Uniform : pm_Uniforms)
            {
                cout << "Uniform Name: " << lv_Uniform.first << "\n";
                cout << "Uniform Location: " << lv_Uniform.second << "\n";
            }
        }
#endif
        
        int
            GetUniformLocation(const string& fp_UniformName)
        {
            return pm_Uniforms.at(fp_UniformName);
        }

        [[nodiscard]] string
            GetProgramName()
            const noexcept
        {
            return pm_ProgramName;
        }

        [[nodiscard]] GLuint
            GetProgramID()
            const noexcept
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
                Logger* fp_RenderingLogger
            );

        void
            AutoCaptureActiveUniforms //gets uniforms detected by current glContext, then puts them into a map of the form <uniform-name, uniformLocation>
            (
                Logger* fp_RenderingLogger
            );

        ///////////////////////////////////////////////
        // Create Shaders
        ///////////////////////////////////////////////

        bool
            CreateVertexShader
            (
                const string& fp_ShaderCode,
                Logger* fp_RenderingLogger
            );

        bool
            CreateFragmentShader
            (
                const string& fp_ShaderCode,
                Logger* fp_RenderingLogger
            );

        int
            CreateShader //creates, compiles and attaches desired shader type to current shaderprogram
            (
                const string& fp_ShaderSourceCode,
                GLuint fp_ShaderType,
                Logger* fp_RenderingLogger
            ) 
            const;

       //////////////////////////////////////////////
       // Shader Cleanup
       //////////////////////////////////////////////

        void 
            CleanUp()
        {
            glDeleteProgram(pm_ProgramID);
            pm_ProgramID = 0; //reset val uwu
        }

       //////////////////////////////////////////////
       // Utility Functions
       //////////////////////////////////////////////

        bool
            ReadFileIntoString
            (
                const string& fp_ScriptFilePath,
                string& fp_SourceCode,
                Logger* fp_RenderingLogger
            )
            const;
    };
}//namespace PeachCore::OpenGL

#endif /*PEACH_RENDERER_OPENGL*/