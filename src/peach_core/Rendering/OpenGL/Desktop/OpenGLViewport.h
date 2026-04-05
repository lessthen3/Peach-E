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

#include "OpenGLRenderer.h"

namespace PeachCore::OpenGL {

    //////////////////////////////////////////////
    // Viewport Struct
    //////////////////////////////////////////////

    struct Viewport
    {
        Viewport() = default;

    private:
        GLuint pm_RenderTexture = 0; //i initialize these to -1 so that i know they've been unitialized ig idk
        GLuint pm_FrameBuffer = 0;
        GLuint pm_DepthRenderBuffer = 0;

        GLuint pm_VAO = 0;

        unique_ptr<ShaderProgram> pm_ViewportShader = nullptr;

        unsigned int pm_CurrentViewportHeight = 0;
        unsigned int pm_CurrentViewportWidth = 0;

        Renderer* pm_Render = nullptr;

        shared_ptr<Logger> rendering_logger = nullptr;
        
    public:
        [[nodiscard]] bool
            SetupViewport
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height,
                unique_ptr<ShaderProgram>&& fp_ViewportShader,
                Renderer* fp_Renderer,
                shared_ptr<Logger> fp_EditorRenderingLogger
            );

        [[nodiscard]] bool
            ResizeViewport
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height
            );

        void
            RenderViewport
            (
                const glm::vec2& fp_Position
            );

    private:
        [[nodiscard]] bool
            CreateRenderTexture
            (
                const unsigned int fp_Width,
                const unsigned int fp_Height
            );
    };
}//namespace PeachCore::OpenGL

#endif /*PEACH_RENDERER_OPENGL*/