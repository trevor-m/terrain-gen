#pragma once
#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "PerlinNoise.h"

class TerrainRenderer : public Renderer
{
private:
	Shader shader;
	GLuint VBO, VAO;
	GLuint grassTexture, sandTexture;

	GLfloat* terrainVertices;
	int bufferSize;
	static const int cols = 200, rows = 200;

	void generate();
public:
	TerrainRenderer(GLuint WIDTH, GLuint HEIGHT);
	~TerrainRenderer();

	void Render(Camera& camera, glm::vec4 clipPlane, GLfloat waterHeight);
};

