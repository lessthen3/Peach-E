#version 330 core

// BGFX uses a varying.def.sc file to define varyings.
// Include this file to use predefined varyings.
#include "bgfx_shader.sh"

// Attributes
attribute vec2 a_position;
attribute vec2 a_texcoord0;
attribute vec4 a_color0;

// Output to fragment shader
varying vec2 v_texcoord;
varying vec4 v_color;

uniform mat4 u_proj;

void main() 
{
    gl_Position = u_proj * vec4(a_position, 0.0, 1.0);
    v_texcoord = a_texcoord0;
    v_color = a_color0 / 255.0;  // Normalize the color as BGFX passes color as uint8
}
