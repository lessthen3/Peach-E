#version 330 core

#include "bgfx_shader.sh"

varying vec2 v_texcoord;
varying vec4 v_color;

uniform sampler2D s_texture;

void main() 
{
    vec4 texColor = texture2D(s_texture, v_texcoord);
    gl_FragColor = texColor * v_color;
}
