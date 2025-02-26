#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord;

out vec2 TexCoord;

// uniform mat4 transform;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    TexCoord = tex_coord;
}