#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "../General/ShaderProgram.h"

using namespace std;

namespace PeachCore
{
    class PeachRenderer
    {
    public:
        explicit 
            PeachRenderer
            (
                SDL_Window* fp_CurrentWindow, 
                bool fp_Is3DEnabled = false
            )
        {
            pm_Is3DEnabled = fp_Is3DEnabled;
            pm_CurrentWindow = fp_CurrentWindow;

            if (not InitOpenGL())
            {
                LogManager::RenderingLogger().Fatal("Failed to initialize OpenGL.", "PeachRenderer");
                throw runtime_error("Failed to initialize OpenGL.");
            }
        }

        explicit
            PeachRenderer
            (
                const char* fp_WindowTitle,
                const int fp_WindowWidth,
                const int fp_WindowHeight,
                bool fp_Is3DEnabled = false
            )
        {
            pm_Is3DEnabled = fp_Is3DEnabled;

            CreateSDLWindow(fp_WindowTitle, fp_WindowWidth, fp_WindowHeight);

            if (not InitOpenGL())
            {
                LogManager::RenderingLogger().Fatal("Failed to initialize OpenGL.", "PeachRenderer");
                throw runtime_error("Failed to initialize OpenGL.");
            }
        }

        ~PeachRenderer()
        {
            bgfx::shutdown();
        }

        void 
            DrawTexture(const GLuint& fp_TextureID)
        {
            // Assuming shaderProgram is active and configured
            ShaderProgram f_CurrentShaderProgram = pm_ShaderPrograms.at("Test_Program");
            glUseProgram(f_CurrentShaderProgram.GetProgramID());

            SDL_GL_SwapWindow(pm_CurrentWindow);
        }

        void 
            RenderFrame() 
        {
            bgfx::touch(0);
            bgfx::frame();
            SDL_GL_SwapWindow(pm_CurrentWindow);
        }

        GLuint 
            RegisterTexture(const string& fp_PeachObjectID, const unsigned char* fp_Data, const int fp_Width, const int fp_Height, const int fp_Channels)
        {
            return 0;
        }

        void 
            DeleteTexture(GLuint fp_TextureID)
        {
            //glDeleteBuffers(fp_TextureID);
        }

        bool 
            DeleteShaderProgram(string fp_ShaderProgramName)
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
            LoadShadersFromSource(const string& fp_Name, const string& fp_VertexSource, const string& fp_FragmentSource) 
        {
            ShaderProgram f_Shader;

            glUseProgram(f_Shader.GetProgramID());

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

    private:
        //WIP
        bool 
            InitOpenGL() //SFML handles context creation and settings for us, we can create a core profile and define major/minor versions there (thank god)
        {
            SDL_SysWMinfo wmi;
            SDL_VERSION(&wmi.version);

            if (not SDL_GetWindowWMInfo(pm_CurrentWindow, &wmi)) 
            {
                cerr << "Unable to get window info: " << SDL_GetError() << endl;
                throw runtime_error("Failed to get window manager info.");
            }
            else
            {
                LogManager::RenderingLogger().Debug("SDL window info successfully read", "PeachRenderer");
            }

            bgfx::Init bgfxInit;

            #if defined(_WIN32) || (_WIN64)
                        bgfxInit.platformData.nwh = wmi.info.win.window;  // Windows
            #elif defined(__linux__)
                        bgfxInit.platformData.nwh = (void*)wmi.info.x11.window;  // Linux
            #elif defined(__APPLE__)
                        bgfxInit.platformData.nwh = wmi.info.cocoa.window;  // macOS
            #endif

            bgfxInit.platformData.ndt = NULL;
            bgfxInit.platformData.context = NULL;
            bgfxInit.platformData.backBuffer = NULL;
            bgfxInit.platformData.backBufferDS = NULL;

            bgfxInit.type = bgfx::RendererType::OpenGL; // Use the specified type or auto-select

            bgfxInit.resolution.width = 800;
            bgfxInit.resolution.height = 600;
            bgfxInit.resolution.reset = BGFX_RESET_VSYNC;

            if (not bgfx::init(bgfxInit))
            {
                LogManager::RenderingLogger().Fatal("BGFX failed to initialize. RIP", "PeachRenderer");
                throw runtime_error("BGFX initialization failed");
            }
            else
            {
                LogManager::RenderingLogger().Debug("BGFX initialized properly", "PeachRenderer");
            }

            bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
            bgfx::setViewRect(0, 0, 0, 800, 600);

            bgfx::frame();
            
            return true;
        }

        bool
            CreateSDLWindow
            (
                const char* fp_WindowTitle,
                const int fp_WindowWidth,
                const int fp_WindowHeight
            )
        {
            pm_CurrentWindow =
                SDL_CreateWindow
                (
                    fp_WindowTitle,
                    SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED,
                    fp_WindowWidth, fp_WindowHeight,
                    SDL_WINDOW_SHOWN
                );
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
                GLuint instanceVBO, 
                const vector<float>& instanceData, 
                int attributeIndex, 
                int size, 
                int instanceDataLength, 
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

    private:
        SDL_Window* pm_CurrentWindow;
        bool pm_Is3DEnabled;

        map<string, ShaderProgram> pm_ShaderPrograms; //keeps track of which visual element uses which ShaderProgram
        map< string, tuple<GLuint, GLuint, GLuint, GLuint, GLuint> > MapOfVisualElements; //keeps track of the vao,vbo,ebo id's for each element that is registered
    };
}
