#version 330 core
in vec4 Position;

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
uniform vec3 viewPos;

const float waterFade = 1.0;

void main()
{
	// mix between sand and grass
	/*vec4 currentColor;
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
	//specular
	vec3 viewDir = normalize(viewPos - vec3(Position));
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
	vec3 specular = sun.specular * spec * vec3(currentColor);  */

	color = vec4(0,0,1,1);// vec4(ambient + diffuse + specular, 1.0f);
}