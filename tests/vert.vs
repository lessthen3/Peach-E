#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 norms;

out vec2 TexCoord;

uniform mat4 transform;

void main()
{
    gl_Position = vec4(position, 1.0) * transform;
    TexCoord = tex_coord;
}