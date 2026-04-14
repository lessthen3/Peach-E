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
#ifdef PEACH_RENDERER_OPENGL

#include "OpenGLShaderProgram.h"

#include <fmt/format.h>

#include <sstream>

namespace PeachCore::OpenGL {


    ShaderProgram::~ShaderProgram() //cleaning up should be kosher since OpenGL runs single threaded anyways owo
    {
        if (pm_ProgramID != 0) //delete program if it has been set only
        {
            CleanUp();
        }

        PRINT(fmt::format("Destroyed program ID: {} for shader program named: {} ", pm_ProgramID, pm_ProgramName), Colours::BrightMagenta);
    }


    ShaderProgram::ShaderProgram
    (
        const string& fp_ShaderName,
        const string& fp_VertexSourceFilePath,
        const string& fp_FragmentSourceFilePath,
        Logger* logger
    )
    {
        pm_ProgramName = fp_ShaderName;
        pm_ProgramID = glCreateProgram();

        string f_VertexSourceCode, f_FragmentSourceCode;
        bool f_IsVertexShaderValid = false;
        bool f_IsFragmentShaderValid = false;

        ///vertex shader
        if (not ReadFileIntoString(fp_VertexSourceFilePath, f_VertexSourceCode, logger))
        {
            logger->Error("Unable to read vertex shader code into a string", "OpenGL::ShaderProgram: " + pm_ProgramName);
        }
        else if (CreateVertexShader(f_VertexSourceCode, logger))
        {
            f_IsVertexShaderValid = true;
        }
        ///fragment shader
        if (not ReadFileIntoString(fp_FragmentSourceFilePath, f_FragmentSourceCode, logger))
        {
            logger->Error("Unable to read fragment shader code into a string", "OpenGL::ShaderProgram: " + pm_ProgramName);
        }
        else if (CreateFragmentShader(f_FragmentSourceCode, logger))
        {
            f_IsFragmentShaderValid = true;
        }

        if (f_IsVertexShaderValid and f_IsFragmentShaderValid)
        {
            Link(logger);
        }
        else
        {
            logger->Error("Shader failed to link due to invalid shader(s)", "OpenGL::ShaderProgram: " + pm_ProgramName);
        }
    }

    //////////////////////////////////////////////
    // Fog Uniform Creator Setters
    //////////////////////////////////////////////

    void
        ShaderProgram::SetUniform
        (
            const string& fp_UniformName,
            const Fog3D& fp_Fog
        )
    {
        SetUniform(fp_UniformName + ".activeFog", fp_Fog.IsActive() ? 1 : 0);
        SetUniform(fp_UniformName + ".colour", fp_Fog.GetColour());
        SetUniform(fp_UniformName + ".density", fp_Fog.GetDensity());
    }

    //////////////////////////////////////////////
    // Material Uniform Setters
    //////////////////////////////////////////////

    void
        ShaderProgram::SetMaterialUniforms
        (
            const string& fp_UniformName,
            const PeachMaterial& fp_Material
        )
    {
        SetUniform(fp_UniformName + ".ambient", fp_Material.GetAmbientColour());
        SetUniform(fp_UniformName + ".diffuse", fp_Material.GetDiffuseColour());
        SetUniform(fp_UniformName + ".specular", fp_Material.GetSpecularColour());
        SetUniform(fp_UniformName + ".hasTexture", fp_Material.IsTextured() ? 1 : 0);
        SetUniform(fp_UniformName + ".reflectance", fp_Material.GetReflectance());
    }

    //////////////////////////////////////////////
    // Texture Uniform Setter
    //////////////////////////////////////////////

    void
        ShaderProgram::SetTexture
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
        ShaderProgram::Link
        (
            Logger* logger
        )
    {
        for (auto& shader : pm_Shaders)
        {
            glAttachShader(pm_ProgramID, shader.second);
        }

        glLinkProgram(pm_ProgramID);

        logger->Debug
        (
            "Successfully Linked!", 
            fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
        );

        GLint f_Success;
        GLchar f_InfoLog[512];

        glGetProgramiv(pm_ProgramID, GL_LINK_STATUS, &f_Success);
        if (not f_Success)
        {
            glGetProgramInfoLog(pm_ProgramID, 512, NULL, f_InfoLog);
            logger->Error
            (
                fmt::format("ERROR::SHADER::PROGRAM::LINKING_FAILED: {}", f_InfoLog),
                fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
            );
        }

        for (auto& shader : pm_Shaders)
        {
            glDetachShader(pm_ProgramID, shader.second);
            glDeleteShader(shader.second);  // Delete the shader as it's no longer needed
        }

        glValidateProgram(pm_ProgramID);

        glGetProgramiv(pm_ProgramID, GL_VALIDATE_STATUS, &f_Success);
        if (not f_Success)
        {
            glGetProgramInfoLog(pm_ProgramID, 512, NULL, f_InfoLog);
            logger->Error
            (
                fmt::format("Shader validation error: {}", f_InfoLog), 
                fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
            );
        }

        AutoCaptureActiveUniforms(logger);

        pm_Shaders.clear(); //don't need the contents anymore since they are stored inside the GL context currently
    }

    void
        ShaderProgram::AutoCaptureActiveUniforms
        (
            Logger* fp_RenderingLogger
        ) //gets uniforms detected by current glContext, then puts them into a map of the form <uniform-name, uniformLocation>
    {
        int f_Total = -1;
        glGetProgramiv(pm_ProgramID, GL_ACTIVE_UNIFORMS, &f_Total);

        for (int lv_Index = 0; lv_Index < f_Total; ++lv_Index)
        {
            int name_len = -1, num = -1;
            GLenum type = GL_ZERO;
            char name[100];

            glGetActiveUniform(pm_ProgramID, GLuint(lv_Index), sizeof(name) - 1, &name_len, &num, &type, name);

            name[name_len] = 0;
            GLuint location = glGetUniformLocation(pm_ProgramID, name);

            const char* f_temp = name;

            pm_Uniforms.insert({ f_temp, location });

            fp_RenderingLogger->Debug
            (
                fmt::format("Uniform #: {}, Type(GLenum): {}, Name: {}, Location(GLuint): {}", lv_Index, type, f_temp, location),
                fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
            );
        }
    }

    ///////////////////////////////////////////////
    // Create Shaders
    ///////////////////////////////////////////////

    bool
        ShaderProgram::CreateVertexShader
        (
            const string& fp_ShaderCode,
            Logger* fp_RenderingLogger
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
        ShaderProgram::CreateFragmentShader
        (
            const string& fp_ShaderCode,
            Logger* fp_RenderingLogger
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
        ShaderProgram::CreateShader //creates, compiles and attaches desired shader type to current shaderprogram
        (
            const string& fp_ShaderSourceCode,
            GLuint fp_ShaderType,
            Logger* fp_RenderingLogger
        ) 
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
            fp_RenderingLogger->Error
            (
                fmt::format("Shader compilation error: {}", infoLog), 
                fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
            );
            return 0; // Or handle the error appropriately
        }

        return f_ShaderID;
    }

    //////////////////////////////////////////////
    // Utility Functions
    //////////////////////////////////////////////

    bool
        ShaderProgram::ReadFileIntoString
        (
            const string& fp_ScriptFilePath,
            string& fp_SourceCode,
            Logger* fp_RenderingLogger
        )
        const
    {
        size_t f_LastDotIndex = fp_ScriptFilePath.rfind('.');  // Extract file extension assuming fmt::format "filename.ext"

        if (f_LastDotIndex == string::npos)
        {
            fp_RenderingLogger->Error
            (
                fmt::format("No file extension found for GLSL Shader at specified filepath: {}", fp_ScriptFilePath),
                fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
            );
            return false;
        }

        string f_FileExtension = fp_ScriptFilePath.substr(f_LastDotIndex);

        if (f_FileExtension != ".fs" and f_FileExtension != ".vs" and f_FileExtension != ".glsl" and f_FileExtension != ".frag" and f_FileExtension != ".vert")
        {
            fp_RenderingLogger->Error
            (
                fmt::format("Invalid file extension found when GLSL Shader was expected at specified filepath: {}", fp_ScriptFilePath), 
                fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
            );
            return false;
        }

        ifstream f_FileStream(fp_ScriptFilePath);

        if (not f_FileStream)
        {
            fp_RenderingLogger->Error
            (
                fmt::format("Shader failed to load at file path: {}", fp_ScriptFilePath),
                fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
            );
            return false;
        }

        stringstream f_Buffer;
        f_Buffer << f_FileStream.rdbuf();
        fp_SourceCode = f_Buffer.str();

        fp_RenderingLogger->Debug
        (
            fmt::format("Shader successfully loaded at file path: {}", fp_ScriptFilePath), 
            fmt::format("OpenGL::ShaderProgram: {}:{}", pm_ProgramName, pm_ProgramID)
        );

        return true;
    }

}//namespace PeachCore::OpenGL

#endif /*PEACH_RENDERER_OPENGL*/