#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec2 TexCoords;
out vec4 Position;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 clipPlane;

void main()
{
	Position = model * vec4(position, 1.0f);
	gl_ClipDistance[0] = dot(Position, clipPlane);

    gl_Position = projection * view * Position;
    TexCoords = texCoords;
	Normal = normal; //mat3(transpose(inverse(model))) * normal;
}