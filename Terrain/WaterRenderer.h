#pragma once
#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"

#define REFLECTION_WIDTH 320
#define REFLECTION_HEIGHT 180
#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 720

class WaterRenderer : public Renderer
{
private:
	Shader shader;
	GLuint VBO, VAO;
	GLuint reflectionFBO, refractionFBO;
	GLuint reflectionTexture, reflectionDepthTexture, refractionTexture, refractionDepthTexture;
	GLuint dudvMap, normalMap;

	GLfloat waveSpeed;
	GLfloat waveMoveFactor;

	GLfloat vertices[6*3];
	int cols, rows;

	//GLuint dudvMap, normalMap;
	void generate(GLfloat waterHeight);
public:
	WaterRenderer(GLuint WIDTH, GLuint HEIGHT, int cols, int rows, GLfloat waterHeight);
	~WaterRenderer();

	//void PrepareRender();
	void Render(Camera& camera, GLfloat deltaTime);

	void BindReflectionBuffer();
	void BindRefractionBuffer();
	void UnbindBuffer();
};

