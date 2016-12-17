#version 330 core
in vec4 Position;
in vec4 ClipSpace;
in vec2 TexCoords;

out vec4 color;

struct DirectionalLight {
    vec3 direction;
  
   // vec3 ambient;
    //vec3 diffuse;
    vec3 specular;
};

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float waterHeight;
uniform DirectionalLight sun;
uniform vec3 viewPos;
uniform float waveMoveFactor;

const float waterFade = 1.0;

const float waveStrength = 0.04;

const float shininess = 132.0;
const float reflectivity = 0.8;

void main()
{
	// normalized device space
	vec2 ndc = (ClipSpace.xy/ClipSpace.w)/2.0 + 0.5;
	vec2 reflectTexCoords = vec2(ndc.x , -ndc.y);
	vec2 refractTexCoords = ndc;

	float near = 0.1;
	float far = 1000.0;
	float depth = texture(depthMap, refractTexCoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	float waterDepth = floorDistance - waterDistance;

	//get RG values of dudv map
	vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoords.x + waveMoveFactor, TexCoords.y)).rg*0.1;
	distortedTexCoords = TexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+waveMoveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth/2.0, 0.0, 1.0);

	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);
	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColor = texture(refractionTexture, refractTexCoords);

	vec4 normalColor = texture(normalMap, distortedTexCoords);
	vec3 norm = normalize(vec3(normalColor.r * 2.0 - 1.0, normalColor.g*2.0, normalColor.b * 2.0 - 1.0));
	vec3 viewDir = normalize(viewPos - vec3(Position));
	vec3 reflectDir = reflect(-sun.direction, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = sun.specular * spec * reflectivity * clamp(waterDepth/5.0, 0.0, 1.0); 


	float refractiveFactor = clamp(pow(dot(viewDir, norm), 0.5), 0.0, 1.0); //change this to make more seethrough/reflective

	color = mix(reflectColor, refractColor, refractiveFactor);
	color = mix(color, vec4(0,0.5,0.8,1), 0.2) + vec4(specular, 0.0);
	color.a = clamp(waterDepth/2.0, 0.0, 1.0);
}