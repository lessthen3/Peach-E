#pragma once

#include <bgfx/bgfx.h>
#include "nuklear.h"

#include <memory>

using namespace std;

void 
    RenderNuklearCommands(nk_context* fp_NuklearContext) 
{
    const nk_command* cmd;
    const nk_buffer* vbuf;
    const nk_buffer* ebuf;

    ////// Setup for rendering
    ////bgfx::setViewRect(0, 0, 0, windowWidth, windowHeight);
    ////bgfx::setViewMode(0, bgfx::ViewMode::Sequential);

    // Get draw command
    struct nk_buffer vbuf, ebuf;

    nk_foreach(cmd, fp_NuklearContext)
    {
        //// Bind texture
        //bgfx::TextureHandle th = { (uint16_t)cmd->texture.id };
        //bgfx::setTexture(0, s_textureUniform, th);

        //// Setup vertex and index buffers
        //bgfx::TransientVertexBuffer tvb;
        //bgfx::TransientIndexBuffer tib;

        //bgfx::allocTransientVertexBuffer(&tvb, cmd->vertex_count, s_layout);
        //bgfx::allocTransientIndexBuffer(&tib, cmd->elem_count);

        //// Copy vertex data
        //memcpy(tvb.data, cmd->vertex_ptr, cmd->vertex_count * sizeof(Vertex));
        //memcpy(tib.data, cmd->element_ptr, cmd->elem_count * sizeof(uint16_t));

        //// Set state and submit
        //bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
        //bgfx::setVertexBuffer(0, &tvb);
        //bgfx::setIndexBuffer(&tib);
        //bgfx::submit(0, s_program);
    }
}
