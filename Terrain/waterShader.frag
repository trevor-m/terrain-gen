#version 330 core
in vec4 Position;
in vec4 ClipSpace;

out vec4 color;

struct DirectionalLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform float waterHeight;
//uniform DirectionalLight sun;
//uniform vec3 viewPos;

const float waterFade = 1.0;

void main()
{
	// lighting
	/*//ambient
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
	// vec4(ambient + diffuse + specular, 1.0f);



	// normalized device space
	vec2 ndc = (ClipSpace.xy/ClipSpace.w)/2.0 + 0.5;

	vec4 reflectColor = texture(reflectionTexture, vec2(ndc.x , -ndc.y));
	vec4 refractColor = texture(refractionTexture, ndc);

	color = mix(reflectColor, refractColor, 0.5);
	//color = vec4(0,0,1,1);
}