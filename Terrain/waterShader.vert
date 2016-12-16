#version 330 core
layout (location = 0) in vec3 position;
out vec4 ClipSpace;
out vec2 TexCoords;
out vec4 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const float tiling = 6.0;

void main()
{
	Position = model * vec4(position, 1.0f);
	ClipSpace = projection * view * Position;
    gl_Position = ClipSpace;
	TexCoords = vec2(position.x/2.0 + 0.5, position.z/2.0 +0.5) * tiling;
}