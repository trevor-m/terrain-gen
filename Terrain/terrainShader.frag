#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;

void main()
{             
    color = vec4(1.0f);//texture(texture1, TexCoords);
}