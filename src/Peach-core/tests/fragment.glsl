#version 450 core
in vec2 TexCoord;
out vec4 FragColor;

layout (location = 30) uniform sampler2D texture_sampler;

vec2 m_Temp = vec2(1.0, 2.0);

void main()
{
    m_Temp = TexCoord + vec2(2.0, 3.0);
    FragColor = texture(texture_sampler, TexCoord);
}
