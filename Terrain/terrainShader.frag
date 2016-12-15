#version 330 core
in vec2 TexCoords;
in vec4 Position;

out vec4 color;

uniform sampler2D grassTexture;
uniform sampler2D sandTexture;
uniform float waterHeight;

const float waterFade = 1.0;

void main()
{
	if (Position.y < waterHeight - waterFade)
		color = texture(sandTexture, TexCoords);
	else if (Position.y < waterHeight)
		color = mix(texture(grassTexture, TexCoords), texture(sandTexture, TexCoords), waterHeight - Position.y);
	else
		color = texture(grassTexture, TexCoords);
}