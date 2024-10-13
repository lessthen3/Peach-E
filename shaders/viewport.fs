#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D Test_Texture;

void main()
{
    FragColor = texture(Test_Texture, TexCoord);
} 