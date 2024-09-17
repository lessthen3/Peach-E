#pragma once

#include "../Managers/LogManager.h"

#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

namespace PeachCore {

    class ShaderProgram 
    {
    public:
        ShaderProgram()
        {
        }

        ~ShaderProgram()
        {
            CleanUp();
        }

        ////////////////////////////////////////////////////////////
        //SHADER CREATOR FOR FRAGMENT AND VERTEX SHADERS
        ////////////////////////////////////////////////////////////

        void CreateVertexShader(const string& fp_ShaderCode)
        {
            //pm_Shaders.insert({"VertexShader", CreateShader(fp_ShaderCode, GL_VERTEX_SHADER) });
        }

        void CreateFragmentShader(const string& fp_ShaderCode)
        {
            //pm_Shaders.insert({"FragmentShader", CreateShader(fp_ShaderCode, GL_FRAGMENT_SHADER) });
        }

        int 
            CreateShader
            (
                string fp_ShaderSourceCode, 
                int fp_ShaderType
            ) //creates, compiles and attaches desired shader type to current shaderprogram
            const
        {
            //glUseProgram(pm_ProgramID);
            //int f_ShaderID = glCreateShader(fp_ShaderType);

            //if (f_ShaderID == 0) 
            //    {return 0;}

            //const char* f_Cstr = fp_ShaderSourceCode.c_str(); //idk why cpp makes me do this in two lines but whatever

            //glShaderSource(f_ShaderID, 1, &f_Cstr, NULL);
            //glCompileShader(f_ShaderID);

            //int success;
            //GLchar infoLog[512];

            //// After glCompileShader(f_ShaderID);
            //glGetShaderiv(f_ShaderID, GL_COMPILE_STATUS, &success);
            //if (!success)
            //{
            //    glGetShaderInfoLog(f_ShaderID, 512, NULL, infoLog);
            //    cerr << "Shader compilation error: " << infoLog << endl;
            //    return 0; // Or handle the error appropriately
           // }

            //glAttachShader(pm_ProgramID, f_ShaderID);

            //return f_ShaderID;
            return 0;
        }

        ///////////////////////////////////////////////
        // Generic Uniform Setters 
        ///////////////////////////////////////////////

        void SetUniformMat4(const string& fp_UniformName, glm::mat4 matrix) 
        {
            //glUniformMatrix4fv(pm_Uniforms.at(fp_UniformName), 1, GL_FALSE, glm::value_ptr(matrix));
        }

        void SetUniform1i(const string& fp_UniformName, const int value) { //retrieves uniform and its associated int and sets it
            //glUniform1i(pm_Uniforms[fp_UniformName], value);
        }

        void SetUniform1f(const string& fp_UniformName, const float value) {
            //glUniform1f(pm_Uniforms[fp_UniformName], value);
        }

        void SetUniform3f(const string& fp_UniformName, glm::vec3 value) {
            //glUniform3f(pm_Uniforms[fp_UniformName], value.x, value.y, value.z);
        }

        void SetUniform4f(const string& fp_UniformName, glm::vec4 value) {
            //glUniform4f(pm_Uniforms[fp_UniformName], value.x, value.y, value.z, value.w);
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

        void Link()   
        {
            AutoCaptureActiveUniforms(); //Does what it says on the tin. please dont ask how many times i cried while writing this class, please just dont'.
        }

        void AutoCaptureActiveUniforms() //gets uniforms detected by current glContext, then puts them into a map of the form <uniform-name, uniformLocation>
        {
        }

       //////////////////////////////////////////////
       // Shader Cleanup
       //////////////////////////////////////////////

        void Bind() 
            const
        {
        }

        void Unbind() 
            const
        {
        }

        void CleanUp() 
            const
        {
        }

        //////////////////////////////////////////////
        // load/compilation functions
        //////////////////////////////////////////////

        //loads and compiles one shader, also attaches it to the current ShaderProgram
        bool 
            LoadAndCompileShader
            (
                const string& kind, 
                const string& filePath
            ) 
        {
            //// Load the file into a string
            //ifstream shaderFile(filePath);

            //if (!shaderFile)
            //{
            //    LogManager::RenderingLogger().Warn("Failed to open shader file: " + filePath, "ShaderProgram");
            //    return false;
            //}
            //string source((istreambuf_iterator<char>(shaderFile)), istreambuf_iterator<char>());
           
            //if (kind == "VertexShader")
            //{
            //    CreateVertexShader(source);
            //}

            //else if (kind == "FragmentShader")
            //{
            //    CreateFragmentShader(source);
            //}
            //else
            //{
            //    LogManager::RenderingLogger().Warn("Failed to associate loaded shader with pre-defined shader types", "ShaderProgram");
            //    return false;
            //}

            //// Compile the source code
            //return true;
        }

        //compiles multiple shaders
        bool 
            LoadAndCompileShaders
            (
                const map<string, string>& fp_ShadersToLoadAndCompile
            ) //takes a map of "Shader Type" : "Shader Paths"
        {
           // glUseProgram(pm_ProgramID);
            //for (auto& shader : fp_ShadersToLoadAndCompile)
            //{
            //    LoadAndCompileShader(shader.first, shader.second); //loads shader type from shader path in map
            //}

            //if (false) {
            //    cerr << "Failed to compile shaders." << endl;
            //    return false;
            //}


            //return true;
        }

        //BROKEN DO NOT USE, WIP
        void PrintShaderProgramDebugVerbose()
            const
        {
        }

        void PrintShaderProgramUniformList()
        {
            for (auto& uniform : pm_Uniforms)
            {
                cout << "Uniform Name: " << uniform.first << "\n";
                cout << "Uniform Location: " << uniform.second << "\n";
            }
        }

        int GetUniformLocation(const string& fp_UniformName)
        {
            return pm_Uniforms.at(fp_UniformName);
        }

        string GetProgramName() const {
            return pm_ProgramName;
        }

        int GetProgramID() 
            const
        {
            return pm_ProgramID;
        }
    
private:
        map<string, int> pm_Shaders; //stores references to all shader IDs that have been registered with the ShaderProgram
        map<string, int> pm_Uniforms; //stores all information relevant to program uniforms

        string pm_ProgramName;

        int pm_ProgramID = 0;
    };

}



