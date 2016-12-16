#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(GLuint WIDTH, GLuint HEIGHT, int cols, int rows, GLfloat waterHeight)
	: shader("waterShader.vert", "waterShader.frag"), Renderer(WIDTH, HEIGHT)
{
	generate(cols, rows, waterHeight);
}

WaterRenderer::~WaterRenderer()
{
}

void WaterRenderer::generate(int cols, int rows, GLfloat waterHeight)
{
	vertices[0] = -rows / 2;
	vertices[1] = waterHeight;
	vertices[2] = -cols / 2;

	vertices[3] = rows / 2;
	vertices[4] = waterHeight;
	vertices[5] = -cols / 2;

	vertices[6] = -rows / 2;
	vertices[7] = waterHeight;
	vertices[8] = cols / 2;

	vertices[9] = rows / 2;
	vertices[10] = waterHeight;
	vertices[11] = cols / 2;

	vertices[12] = -rows / 2;
	vertices[13] = waterHeight;
	vertices[14] = cols / 2;

	vertices[15] = rows / 2;
	vertices[16] = waterHeight;
	vertices[17] = -cols / 2;

	// create buffers for data
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	// set VAO and VBO data
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void WaterRenderer::Render(Camera& camera) {
	// use shader
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// view matrix
	glm::mat4 view;
	view = camera.GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	// projection matrix
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(camera.Zoom), WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// model matrix
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// bind textures
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glUniform1i(glGetUniformLocation(shader.Program, "grassTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sandTexture);
	glUniform1i(glGetUniformLocation(shader.Program, "sandTexture"), 1);*/

	// draw
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}