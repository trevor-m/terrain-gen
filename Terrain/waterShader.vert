#version 330 core
layout (location = 0) in vec3 position;
out vec4 ClipSpace;
out vec4 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	Position = model * vec4(position, 1.0f);
	ClipSpace = projection * view * Position;
    gl_Position = ClipSpace;
}