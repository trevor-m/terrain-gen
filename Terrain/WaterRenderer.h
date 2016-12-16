#pragma once
#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"

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

	void Render(Camera& camera, GLfloat deltaTime);

	void BindReflectionBuffer();
	void BindRefractionBuffer();
	void UnbindBuffer();
};

