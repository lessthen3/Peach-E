#pragma once

#include "nuklear.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "imgui/imgui_impl_bgfx.h"
#include "imgui/imgui_impl_sdl2.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "../General/ShaderProgram.h"

#include "../2D/PeachCamera2D.h"

using namespace std;

namespace PeachCore
{
    class PeachRenderer
    {
    private:
        SDL_Window* pm_CurrentWindow;
        bool pm_Is3DEnabled;

        vector<unique_ptr<PeachCamera2D>> pm_ListOfScenePeachCameras2D; //only the renderer cares about cameras

        map<string, ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which ShaderProgram

    public:

        ~PeachRenderer()
        {
            if (pm_CurrentWindow) //RenderingManager handles bookeeping and creation, after though each PeachRenderer takes exclusive control over its SDL window
            {
                SDL_DestroyWindow(pm_CurrentWindow);
                pm_CurrentWindow = nullptr;
            }

            pm_ListOfScenePeachCameras2D.clear();
        }

        explicit 
            PeachRenderer //peach renderer is never supposed to create an sdl window, it only manages closing it
            (
                SDL_Window* fp_CurrentWindow, 
                const bool fp_Is3DEnabled = false
            )
        {
            pm_Is3DEnabled = fp_Is3DEnabled;
            pm_CurrentWindow = fp_CurrentWindow;
        }

        void 
            DrawTexture(const int& fp_TextureID)
        {
            // Assuming shaderProgram is active and configured
            ShaderProgram f_CurrentShaderProgram = pm_ShaderPrograms.at("Test_Program");

        }

        int 
            RegisterTexture
            (
                const string& fp_PeachObjectID, 
                const unsigned char* fp_Data, 
                const uint32_t fp_Width,
                const uint32_t fp_Height,
                const uint32_t fp_Channels
            )
        {
            return 0;
        }

        void 
            DeleteTexture(const uint32_t fp_TextureID)
        {
            //glDeleteBuffers(fp_TextureID);
        }

        bool 
            DeleteShaderProgram(const string& fp_ShaderProgramName)
        {
            try //idk lazy way of dealing with repeated deletes of a shader program
            {
               // glDeleteProgram(pm_ShaderPrograms.at(fp_ShaderProgramName).GetProgramID());
                pm_ShaderPrograms.erase(fp_ShaderProgramName);
                return true;
            }
            catch (const exception& ex)
            {
                LogManager::RenderingLogger().Warn("An error occurred: " + string(ex.what()), "PeachRenderer"); //this might not work LOL
                return false;
            }
        }

        void 
            LoadShadersFromSource
            (
                const string& fp_Name, 
                const string& fp_VertexSource, 
                const string& fp_FragmentSource
            ) 
        {
            ShaderProgram f_Shader;

           // glUseProgram(f_Shader.GetProgramID());

            cout << "program id in openglrenderer " << f_Shader.GetProgramID() << "\n";

            f_Shader.LoadAndCompileShaders({{"VertexShader", fp_VertexSource}, {"FragmentShader", fp_FragmentSource}});
            f_Shader.Link();

            pm_ShaderPrograms.insert({fp_Name, f_Shader});
        }

        ShaderProgram& 
            GetShaderProgram(const string& fp_Name) 
        {
            return pm_ShaderPrograms.at(fp_Name);
        }

        ////////////////////////////////////////////////
        //// Texture OpenGL Setup
        ////////////////////////////////////////////////

        void 
            SetupMeshDataArrays
            (
                const string& fp_PeachObjectID,
                const vector<float>& fp_Vertices, 
                const vector<uint32_t>& fp_Indices,
                const vector<float>& fp_TexCoords,
                const vector<float>& fp_NormalCoords
            )    
        {

        }

        void 
            SetupInstancedArray
            (
                uint32_t instanceVBO,
                const vector<float>& instanceData, 
                uint32_t attributeIndex,
                uint32_t size,
                uint32_t instanceDataLength,
                int offset
            )
            const
        {

        }

        // Example usage: suppose you want to pass a model matrix per instance
        // You would call setupInstancedArray four times, one for each row of the matrix, because glVertexAttribPointer can only handle up to 4 float components at a time.

        void
            GenerateUniformBufferObject() //used for uniforms that are shared across multiple shaders
        {

        }
    };
}
