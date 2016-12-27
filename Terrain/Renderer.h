#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <vector>

class Renderer
{
protected:
	GLuint WIDTH, HEIGHT;

	GLuint loadTexture(GLchar* path);
	GLuint loadCubemap(std::vector<const GLchar*> faces);
public:
	Renderer(GLuint WIDTH, GLuint HEIGHT);
	~Renderer();

	//virtual void PrepareRender() = 0;
	//virtual void Render() = 0;
};

