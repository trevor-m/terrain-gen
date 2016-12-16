#pragma once
#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"

class WaterRenderer : public Renderer
{
private:
	Shader shader;
	GLuint VBO, VAO;

	GLfloat vertices[6*3];

	//GLuint dudvMap, normalMap;
	void generate(int cols, int rows, GLfloat waterHeight);
public:
	WaterRenderer(GLuint WIDTH, GLuint HEIGHT, int cols, int rows, GLfloat waterHeight);
	~WaterRenderer();

	void Render(Camera& camera);
};

