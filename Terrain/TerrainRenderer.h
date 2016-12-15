#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include "Shader.h"
#include "Camera.h"
#include "PerlinNoise.h"

class TerrainRenderer
{
private:
	GLuint WIDTH, HEIGHT;

	Shader shader;
	GLuint VBO, VAO;
	GLuint grassTexture, sandTexture;

	GLfloat* terrainVertices;
	int bufferSize;
	static const int cols = 200, rows = 200;

	GLuint loadTexture(GLchar* path);
	void generate();
public:
	TerrainRenderer(GLuint WIDTH, GLuint HEIGHT);
	~TerrainRenderer();

	void PrepareRender(GLfloat waterHeight);
	void Render(Camera& camera);
};

