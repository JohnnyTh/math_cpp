#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    FragColor = vec4(texColor.r, texColor.r, texColor.r, 1.0);
}