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

#include "OpenGLViewport.h"

#include <fmt/format.h>

namespace PeachCore::OpenGL {

    static const vector<float> VERTICES =
    {
        // Positions      // Texture Coords
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f,  // Top Right
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // Bottom Right
       -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // Bottom Left
       -1.0f,  1.0f, 0.0f,   0.0f, 1.0f   // Top Left
    };

    static const vector<unsigned int> INDICES =
    {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    bool
        Viewport::SetupViewport
        (
            const unsigned int fp_Width,
            const unsigned int fp_Height,
            unique_ptr<ShaderProgram>&& fp_ViewportShader,
            Renderer* fp_Renderer,
            shared_ptr<Logger> fp_RenderingLogger
        )
    {
        pm_CurrentViewportWidth = fp_Width;
        pm_CurrentViewportHeight = fp_Height;

        ////////////////////////////////////////////////
        // Get Reference to Current Renderer
        ////////////////////////////////////////////////

        if (not fp_Renderer)
        {
            //handle error here
            return false;
        }

        pm_Render = fp_Renderer;
        rendering_logger = fp_RenderingLogger;

        ////////////////////////////////////////////////
        // Viewport Shader UwU
        ////////////////////////////////////////////////

        if (not pm_ViewportShader)
        {
            rendering_logger->Error("Passed nullptr viewport shader", "OpenGL::Viewport");
            return false;
        }

        pm_ViewportShader = std::move(fp_ViewportShader);
        PRINT(fmt::format("The program ID for the Viewport Shader is: {}", pm_ViewportShader->GetProgramID()), Colours::Magenta);

        ////////////////////////////////////////////////
        // Generate Buffers
        ////////////////////////////////////////////////

        pm_VAO = pm_Render->Generate2DBuffers(VERTICES, INDICES);
        PRINT(fmt::format("The VAO ID for the Viewport Shader is: {}", pm_VAO), Colours::Magenta);

        ////////////////////////////////////////////////
        // Create Render Texture
        ////////////////////////////////////////////////

        if (not CreateRenderTexture(pm_CurrentViewportWidth, pm_CurrentViewportHeight))
        {
            PRINT_ERROR("Was not able to create render texture");
            return false;
        }

        return true;
    }

    bool
        Viewport::ResizeViewport
        (
            const unsigned int fp_Width,
            const unsigned int fp_Height
        )
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pm_FrameBuffer);

        ////////////////////////////////////////////////
        // Delete Old Render Texture
        ////////////////////////////////////////////////

        glDeleteTextures(1, &pm_RenderTexture);

        ////////////////////////////////////////////////
        // Generate New Render Texture
        ////////////////////////////////////////////////

        glGenTextures(1, &pm_RenderTexture);

        glBindTexture(GL_TEXTURE_2D, pm_RenderTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pm_CurrentViewportWidth, pm_CurrentViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pm_RenderTexture, 0);

        ////////////////////////////////////////////////
        // Delete Old Render Buffer
        ////////////////////////////////////////////////

        glDeleteRenderbuffers(1, &pm_DepthRenderBuffer);

        ////////////////////////////////////////////////
        // Generate New Render Buffer
        ////////////////////////////////////////////////

        glGenRenderbuffers(1, &pm_DepthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, pm_DepthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, pm_CurrentViewportWidth, pm_CurrentViewportHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pm_DepthRenderBuffer);

        ////////////////////////////////////////////////
        // Validate Frame Buffer
        ////////////////////////////////////////////////

        GLenum f_Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (f_Status != GL_FRAMEBUFFER_COMPLETE) // Check if framebuffer is complete
        {
            rendering_logger->Error(fmt::format("Framebuffer is not complete! status: 0x{:X}", f_Status), "OpenGL::Viewport");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return true;
    }

    void
        Viewport::RenderViewport
        (
            const glm::vec2& fp_Position
        )
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pm_FrameBuffer);

        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);  // Red background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        //glEnable(GL_DEPTH_TEST);

        //// Enable scissor test and set the scissor rectangle
        //glEnable(GL_SCISSOR_TEST);
        //glScissor(0, 0, fp_Width, fp_Height); // Set this to the area you want to clear

        glViewport(fp_Position.x, fp_Position.y, pm_CurrentViewportWidth, pm_CurrentViewportHeight);

        //pm_Render->DrawShapePrimitive(*pm_ViewportShader, pm_VAO);
        //glDisable(GL_SCISSOR_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind to default framebuffer
        pm_Render->DrawTexture(*pm_ViewportShader, pm_VAO, pm_RenderTexture);
        //glDisable(GL_DEPTH_TEST);
    }


    bool
        Viewport::CreateRenderTexture
        (
            const unsigned int fp_Width,
            const unsigned int fp_Height
        )
    {
        pm_CurrentViewportWidth = fp_Width;
        pm_CurrentViewportHeight = fp_Height;

        ////////////////////////////////////////////////
        // Generate Frame Buffer
        ////////////////////////////////////////////////

        glGenFramebuffers(1, &pm_FrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, pm_FrameBuffer);

        ////////////////////////////////////////////////
        // Generate Render Texture
        ////////////////////////////////////////////////

        glGenTextures(1, &pm_RenderTexture);

        glBindTexture(GL_TEXTURE_2D, pm_RenderTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pm_CurrentViewportWidth, pm_CurrentViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pm_RenderTexture, 0);

        ////////////////////////////////////////////////
        // Generate Render Buffer
        ////////////////////////////////////////////////

        glGenRenderbuffers(1, &pm_DepthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, pm_DepthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, pm_CurrentViewportWidth, pm_CurrentViewportHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pm_DepthRenderBuffer);

        ////////////////////////////////////////////////
        // Setup Frame Buffer
        ////////////////////////////////////////////////

        GLenum f_Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (f_Status != GL_FRAMEBUFFER_COMPLETE) // Check if framebuffer is complete
        {
            rendering_logger->Error(fmt::format("Framebuffer is not complete! status: 0x{:X}", f_Status), "OpenGL::Viewport");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }
    
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ////////////////////////////////////////////////
        // Unbind Buffers and Reset GL state
        ////////////////////////////////////////////////

        rendering_logger->Debug("Render Texture successfully setup UwU", "OpenGL::Viewport");

        return true;
    }
}//namespace PeachCore::OpenGL


#endif /*PEACH_RENDERER_OPENGL*/