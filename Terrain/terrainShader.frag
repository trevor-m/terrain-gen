#version 330 core
in vec2 TexCoords;
in vec4 Position;
in vec3 Normal;

out vec4 color;

struct DirectionalLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D grassTexture;
uniform sampler2D sandTexture;
uniform float waterHeight;
uniform DirectionalLight sun;

const float waterFade = 1.0;

void main()
{
	// mix between sand and grass
	vec4 currentColor;
	if (Position.y < waterHeight - waterFade)
		currentColor = texture(sandTexture, TexCoords);
	else if (Position.y < waterHeight)
		currentColor = mix(texture(grassTexture, TexCoords), texture(sandTexture, TexCoords), waterHeight - Position.y);
	else
		currentColor = texture(grassTexture, TexCoords);

	// lighting
	//ambient
	vec3 ambient = sun.ambient * vec3(currentColor);
	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-sun.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = sun.diffuse * diff * vec3(currentColor);

	color = vec4(ambient + diffuse, 1.0f);
}