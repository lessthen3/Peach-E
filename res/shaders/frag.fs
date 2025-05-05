#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform vec4 colourUniform = vec4(1.0f, 0.5f, 0.2f, 1.0f);
uniform sampler2D Test_Texture;

void main()
{
    FragColor = texture(Test_Texture, TexCoord) + colourUniform;
} 