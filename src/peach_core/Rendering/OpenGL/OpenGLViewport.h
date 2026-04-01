/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#ifdef PEACH_RENDERER_OPENGL

#include "OpenGLRenderer.h"

namespace PeachCore::OpenGL {

    //////////////////////////////////////////////
    // Viewport Struct
    //////////////////////////////////////////////

    struct Viewport
    {
        Viewport() = default;

        void
            SetupViewport
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height,
                Renderer* fp_Renderer,
                shared_ptr<Logger> fp_EditorRenderingLogger
            );

        void
            ResizeViewport
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height
            );

        void
            RenderViewport
            (
                const glm::vec2& fp_Position,
                const unsigned int fp_Width,
                const unsigned int fp_Height
            );

    private:
        GLuint pm_RenderTexture = -1; //i initialize these to -1 so that i know they've been unitialized ig idk
        GLuint pm_FrameBuffer = -1;
        GLuint pm_DepthRenderBuffer = -1;

        GLuint pm_VAO = -1;

        ShaderProgram* pm_ViewportShader = nullptr;

        unsigned int pm_CurrentViewportHeight = 0;
        unsigned int pm_CurrentViewportWidth = 0;

        //vector<SDL_Event> pm_CurrentPolledEvents;

        Renderer* pm_Render = nullptr;

        shared_ptr<Logger> editor_rendering_logger = nullptr;

        bool
            CreateRenderTexture
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height
            );
    };
}//namespace PeachCore::OpenGL

#endif /*PEACH_RENDERER_OPENGL*/