#version 330 core
layout (location = 0) in vec3 position;    // Position variable
layout (location = 1) in vec2 inTexCoord; // TexCoord variable

out vec2 TexCoord;

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

void main()
{

    //gl_Position = projection * view * model * vec4(position, 1.0);
    TexCoord = inTexCoord;
}
