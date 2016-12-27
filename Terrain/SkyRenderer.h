#pragma once
#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"

class SkyRenderer : public Renderer
{
private:
	Shader shader;
	GLuint VAO, VBO;
	GLuint skyCubemap;


	void generate();
public:
	SkyRenderer(GLuint WIDTH, GLuint HEIGHT);
	~SkyRenderer();

	void Render(Camera& camera);
};

