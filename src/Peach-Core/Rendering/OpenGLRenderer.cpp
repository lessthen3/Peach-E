#include "../../../include/Peach-Core/Rendering/OpenGLRenderer.h"

#ifndef __APPLE__

namespace PeachCore {

    void
        Viewport::SetupViewport
        (
            const unsigned int fp_Width,
            const unsigned int fp_Height,
            OpenGLRenderer* fp_Renderer,
            shared_ptr<LogManager> fp_EditorRenderingLogger
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
            return;
        }
        pm_Render = fp_Renderer;

        editor_rendering_logger = fp_EditorRenderingLogger;

        ////////////////////////////////////////////////
        // Generate Buffers
        ////////////////////////////////////////////////

        vector<float> vertices =
        {
            // Positions      // Texture Coords
            1.0f,  1.0f, 0.0f,   1.0f, 1.0f,  // Top Right
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // Bottom Right
           -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // Bottom Left
           -1.0f,  1.0f, 0.0f,   0.0f, 1.0f   // Top Left
        };

        vector<unsigned int> indices =
        {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };

        pm_VAO = pm_Render->Generate2DBuffers(vertices, indices);

        Print("The VAO ID for the Viewport Shader is: " + to_string(pm_VAO), Colours::Magenta);

        ////////////////////////////////////////////////
        // Shaders
        ////////////////////////////////////////////////

        string f_BaseDir = PHYSFS_getWriteDir(); //WARNING: USED ONLY FOR TESTING NEED THIS TO BE IN RESOURCELOADINGMANAGER

        pm_ViewportShader = new OpenGLShaderProgram
        (
            "Viewport Shader",
            f_BaseDir + "/shaders/viewport.vs",
            f_BaseDir + "/shaders/viewport.fs",
            editor_rendering_logger.get()
        );

        Print("The program ID for the Viewport Shader is: " + to_string(pm_ViewportShader->GetProgramID()), Colours::Magenta);

        ////////////////////////////////////////////////
        // Create Render Texture
        ////////////////////////////////////////////////

        if (not CreateRenderTexture(pm_CurrentViewportWidth, pm_CurrentViewportHeight))
        {
            PeachCore::PrintError("Was not able to create render texture");
        }
    }

    void
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void
        Viewport::RenderViewport
        (
            const glm::vec2& fp_Position,
            const unsigned int fp_Width,
            const unsigned int fp_Height
        )
    {
        if (pm_CurrentViewportWidth != fp_Width or pm_CurrentViewportHeight != fp_Height)
        {
            pm_CurrentViewportWidth = fp_Width;
            pm_CurrentViewportHeight = fp_Height;

            ResizeViewport(pm_CurrentViewportWidth, pm_CurrentViewportHeight);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, pm_FrameBuffer);

        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);  // Red background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        //glEnable(GL_DEPTH_TEST);

        if (not pm_Render)
        {
            throw runtime_error("no valid renderer for viewport");
        }

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

        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            editor_rendering_logger->LogAndPrint("Error: Framebuffer is not complete!", "Viewport", PeachCore::LogManager::LogLevel::Error);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }
       
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ////////////////////////////////////////////////
        // Unbind Buffers and Reset GL state
        ////////////////////////////////////////////////

        editor_rendering_logger->LogAndPrint("Render Texture successfully setup UwU", "Viewport", PeachCore::LogManager::LogLevel::Debug);

        return true;
    }
}

#endif