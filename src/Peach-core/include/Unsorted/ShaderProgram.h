#pragma once


#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include <Windows.h>

#include <GL/glew.h>
#include <GL/GL.h>
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

        void CreateVertexShader(std::string fp_ShaderCode)
        {
            pm_Shaders.insert({"VertexShader", CreateShader(fp_ShaderCode, GL_VERTEX_SHADER) });
        }

        void CreateFragmentShader(std::string fp_ShaderCode)
        {
            pm_Shaders.insert({"FragmentShader", CreateShader(fp_ShaderCode, GL_FRAGMENT_SHADER) });
        }

        GLuint CreateShader(std::string fp_ShaderSourceCode, GLuint fp_ShaderType) //creates, compiles and attaches desired shader type to current shaderprogram
            const
        {
            GLuint f_ShaderID = glCreateShader(fp_ShaderType);

            if (f_ShaderID == 0) 
                {return 0;}

            const char* f_Cstr = fp_ShaderSourceCode.c_str(); //idk why cpp makes me do this in two lines but whatever

            glShaderSource(f_ShaderID, 1, &f_Cstr, NULL);
            glCompileShader(f_ShaderID);

            //if (glGetShaderi(shaderId, GL_COMPILE_STATUS) == 0) {
            //    throw new Exception("Error compiling Shader code: " + glGetShaderInfoLog(shaderId, 1024));
            //}

            glAttachShader(pm_ProgramID, f_ShaderID);

            return f_ShaderID;
        }

        ///////////////////////////////////////////////
        //GENERIC UNIFORM SETTERS AND CREATOR
        ///////////////////////////////////////////////

        void CreateUniform(std::string fp_UniformName) 
        {
            int uniformLocation = glGetUniformLocation(pm_ProgramID, (fp_UniformName).c_str());
            //if (uniformLocation < 0) { throw new Exception("Could not find uniform: " + uniformName); }
            pm_Uniforms.emplace(fp_UniformName, uniformLocation);
        }

        void SetUniform(const char* uniformName, glm::mat4 matrix) {
            glUniformMatrix4fv(pm_Uniforms[uniformName], 1, GL_FALSE, glm::value_ptr(matrix)); //allocates a floatbuffer and fills it simultaneously  
        }

        void SetUniform(const std::string uniformName, const int value) { //retrieves uniform and its associated GLuint and sets it
            glUniform1i(pm_Uniforms[uniformName], value);
        }

        void SetUniform(std::string uniformName, const float value) {
            glUniform1f(pm_Uniforms[uniformName], value);
        }

        void SetUniform(const std::string uniformName, glm::vec3 value) {
            glUniform3f(pm_Uniforms[uniformName], value.x, value.y, value.z);
        }

        void SetUniform(const std::string uniformName, glm::vec4 value) {
            glUniform4f(pm_Uniforms[uniformName], value.x, value.y, value.z, value.w);
        }

        //////////////////////////////////////////////
        //FOG UNIFORM CREATOR AND SETTER
        //////////////////////////////////////////////

        void createFogUniform(std::string uniformName) 
        {
            CreateUniform(uniformName + ".activeFog");
            CreateUniform(uniformName + ".colour");
            CreateUniform(uniformName + ".density");
        }

        //void SetUniform(std::string uniformName, Fog fog) {
        //    SetUniform(uniformName + ".activeFog", fog.isActive() ? 1 : 0);
        //    SetUniform(uniformName + ".colour", fog.getColour());
        //    SetUniform(uniformName + ".density", fog.getDensity());
        //}

        //////////////////////////////////////////////
        //MATERIAL UNIFORM CREATOR AND SETTER
        //////////////////////////////////////////////

        void CreateMaterialUniforms(std::string uniformName) 
        {
            CreateUniform(uniformName + ".ambient");
            CreateUniform(uniformName + ".diffuse");
            CreateUniform(uniformName + ".specular");
            CreateUniform(uniformName + ".hasTexture");
            CreateUniform(uniformName + ".reflectance");
        }

        //void SetMaterialUniforms(std::string uniformName, Material material) {
        //    SetUniform(uniformName + ".ambient", material.getAmbientColour());
        //    SetUniform(uniformName + ".diffuse", material.getDiffuseColour());
        //    SetUniform(uniformName + ".specular", material.getSpecularColour());
        //    SetUniform(uniformName + ".hasTexture", material.isTextured() ? 1 : 0);
        //    SetUniform(uniformName + ".reflectance", material.getReflectance());
        //}

        //////////////////////////////////////////////
        //SHADER LINKER AND CLEANUP
        //////////////////////////////////////////////

        void Link()   
        {
            glLinkProgram(pm_ProgramID);

            //if (glGetProgrami(programId, GL_LINK_STATUS) == 0) {
            //    throw new Exception("Error linking Shader code: " + glGetProgramInfoLog(programId, 1024));
            //}

            if (pm_Shaders["VertexShader"] != 0) {
                glDetachShader(pm_ProgramID, pm_Shaders["VertexShader"]);
            }

            if (pm_Shaders["FragmentShader"] != 0) {
                glDetachShader(pm_ProgramID, pm_Shaders["FragmentShader"]);
            }

            glValidateProgram(pm_ProgramID);

            //if (glGetProgrami(programId, GL_VALIDATE_STATUS) == 0) {
            //    throw new Exception("Warning validating Shader code: " + glGetProgramInfoLog(programId, 1024));
            //}
        }

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

        bool LoadShader(const std::string& type, const std::string& filePath) {
        
            std::vector<uint8_t> buffer;
            if (!LoadFile(filePath, buffer)) {
                std::cerr << "Failed to load shader file: " << filePath << std::endl;
                return false;
            }


            if (false) {
                std::cerr << "Failed to create " << type << " shader from: " << filePath << std::endl;
                return false;
            }
            return true;
        }

        bool FinalizeProgram() {
            if (!pm_Shaders.count("VertexShader") || !pm_Shaders.count("FragmentShader")) {
                std::cerr << "Vertex and Fragment shaders must be loaded to create a program." << std::endl;
                return false;
            }

            
            // Optionally attach other shaders here if needed, like Geometry or Compute shaders
            return false;
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
                std::cerr << "Failed to open shader file: " << filePath << std::endl;
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

            // Compile the source code
            return true;
        }

        //compiles multiple shaders
        bool LoadAndCompileShaders(const std::map<std::string, std::string>& fp_ShadersToLoadAndCompile) //takes a map of "Shader Type" : "Shader Paths"
        {

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

        bool LoadFile(const std::string& filename, std::vector<uint8_t>& data) {
            std::ifstream file(filename, std::ios::binary | std::ios::ate);
            if (!file) {
                return false;
            }
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            data.resize(size);
            return file.read((char*)data.data(), size).good();
        }
    };

}



