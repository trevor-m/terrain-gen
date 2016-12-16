#version 330 core
in vec4 Position;
in vec4 ClipSpace;
in vec2 TexCoords;

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
uniform vec3 viewPos;
uniform float waveMoveFactor;

const float waterFade = 1.0;
const float waveStrength = 0.02;

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

	vec3 norm = vec3(0, 1, 0);
	vec3 viewDir = normalize(viewPos - vec3(Position));
	float refractiveFactor = pow(dot(viewDir, norm), 0.8); //change this to make more seethrough/reflective

	// normalized device space
	vec2 ndc = (ClipSpace.xy/ClipSpace.w)/2.0 + 0.5;
	vec2 reflectTexCoords = vec2(ndc.x , -ndc.y);
	vec2 refractTexCoords = ndc;

	//get RG values of dudv map
	vec2 distortion1 = (texture(dudvMap, vec2(TexCoords.x + waveMoveFactor, TexCoords.y)).rg * 2.0 - 1.0) * waveStrength;
	vec2 distortion2 = (texture(dudvMap, vec2(-TexCoords.x + waveMoveFactor, TexCoords.y + waveMoveFactor)).rg * 2.0 - 1.0) * waveStrength;
	vec2 totalDistortion = distortion1 + distortion2;

	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);
	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColor = texture(refractionTexture, refractTexCoords);

	color = mix(reflectColor, refractColor, refractiveFactor);
	color = mix(color, vec4(0,0.3,0.5,1), 0.2);
}