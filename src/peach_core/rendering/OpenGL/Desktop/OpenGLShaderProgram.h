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
#include "utils/Logger.h"
#include "rendering/Fog.h"
#include "rendering/PeachMaterial.h"

///External
#include <GL/glew.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>

///STL
#include <unordered_map>

namespace PeachCore::OpenGL {

    constexpr size_t NUMBER_OF_OPENGL_SHADER_TYPES = 6;

    constexpr size_t VERTEX_SHADER_INDEX = 0;
    constexpr size_t FRAGMENT_SHADER_INDEX = 1;
    constexpr size_t COMPUTE_SHADER_INDEX = 2;
    constexpr size_t GEOMETRY_SHADER_INDEX = 3;
    constexpr size_t TESS_CONTROL_SHADER_INDEX = 4;
    constexpr size_t TESS_EVAL_SHADER_INDEX = 5;

    class ShaderProgram 
    {
    private:
        unordered_map<string, GLint> pm_Uniforms; //stores all information relevant to program uniforms

        string pm_ProgramName;

        GLuint pm_ProgramID = 0;

    public:
        ~ShaderProgram(); //cleaning up should be kosher since OpenGL runs single threaded anyways owo
        ShaderProgram() = default;

        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;
        ShaderProgram(ShaderProgram&& other) noexcept = delete; 

    public:
        ShaderProgram
        (
            const string& fp_ShaderName,
            const string& fp_VertexSource,
            const string& fp_FragmentSource,
            Logger* logger
        );

        ///////////////////////////////////////////////
        // Generic Uniform Setters 
        ///////////////////////////////////////////////

        /// Matrices 

        void 
            SetUniform(const string& fp_UniformName, const mat4s& fp_Matrix) 
        {
            glUniformMatrix4fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        void
            SetUniform(const string& fp_UniformName, const mat3s& fp_Matrix)
        {
            glUniformMatrix3fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        void
            SetUniform(const string& fp_UniformName, const mat2s& fp_Matrix)
        {
            glUniformMatrix2fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        void
            SetUniform(const string& fp_UniformName, const mat2x3s& fp_Matrix)
        {
            glUniformMatrix2x3fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        void
            SetUniform(const string& fp_UniformName, const mat3x2s& fp_Matrix)
        {
            glUniformMatrix3x2fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        void
            SetUniform(const string& fp_UniformName, const mat2x4s& fp_Matrix)
        {
            glUniformMatrix2x4fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        void
            SetUniform(const string& fp_UniformName, const mat4x2s& fp_Matrix)
        {
            glUniformMatrix4x2fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        void
            SetUniform(const string& fp_UniformName, const mat3x4s& fp_Matrix)
        {
            glUniformMatrix3x4fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        void
            SetUniform(const string& fp_UniformName, const mat4x3s& fp_Matrix)
        {
            glUniformMatrix4x3fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, fp_Matrix.raw[0]);
        }

        /// int vecs 

        void 
            SetUniform(const string& fp_UniformName, const int fp_Value)
        {
            glUniform1i(pm_Uniforms.at(fp_UniformName), fp_Value);
        }

        void
            SetUniform(const string& fp_UniformName, const ivec2s& fp_Value)
        {
            glUniform2i(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y);
        }

        void
            SetUniform(const string& fp_UniformName, const ivec3s& fp_Value)
        {
            glUniform3i(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y, fp_Value.z);
        }

        void
            SetUniform(const string& fp_UniformName, const ivec4s& fp_Value)
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
            SetUniform(const string& fp_UniformName, const vec2s& fp_Value)
        {
            glUniform2f(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y);
        }

        void 
            SetUniform(const string& fp_UniformName, const vec3s& fp_Value) 
        {
            glUniform3f(pm_Uniforms.at(fp_UniformName), fp_Value.x, fp_Value.y, fp_Value.z);
        }

        void 
            SetUniform(const string& fp_UniformName, const vec4s& fp_Value) 
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

        // void
        //     SetTexture
        //     (
        //         const string& fp_UniformName,
        //         int fp_TextureID,
        //         int fp_TextureUnit
        //     );

#ifdef PEACH_DEBUG //XXX: don't really need this now but better to catch calls to it in rel owo
        void
            PrintShaderProgramUniformList()
        {
            for (auto& lv_Uniform : pm_Uniforms)
            {
                PEACH_PRINT_FMT(PEACH_COL_BRIGHT_GREEN, "Uniform Name: {}, Uniform Location: {}", lv_Uniform.first, lv_Uniform.second);
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
                const vector<GLint>& fp_ShaderIDs,
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