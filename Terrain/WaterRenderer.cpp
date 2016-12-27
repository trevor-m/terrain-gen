#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(GLuint WIDTH, GLuint HEIGHT, int cols, int rows, GLfloat waterHeight)
	: shader("waterShader.vert", "waterShader.frag"), Renderer(WIDTH, HEIGHT)
{
	waveSpeed = 0.03f;
	waveMoveFactor = 0;
	dudvMap = loadTexture("waterDUDV.png");
	normalMap = loadTexture("waterNormaL.png");

	//create framebuffers
	glGenFramebuffers(1, &reflectionFBO);
	glGenFramebuffers(1, &refractionFBO);

	//set up reflection
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//color
	glGenTextures(1, &reflectionTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);
	//depth
	glGenTextures(1, &reflectionDepthTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, reflectionDepthTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Reflection Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//set up refraction
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//color
	glGenTextures(1, &refractionTexture);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);
	//depth
	glGenTextures(1, &refractionDepthTexture);
	glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Refraction Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	generate(waterHeight);
	this->cols = cols;
	this->rows = rows;
}

WaterRenderer::~WaterRenderer()
{
	glDeleteFramebuffers(1, &reflectionFBO);
	glDeleteFramebuffers(1, &refractionFBO);
	glDeleteTextures(1, &reflectionTexture);
	glDeleteTextures(1, &reflectionDepthTexture);
	glDeleteTextures(1, &refractionTexture);
	glDeleteTextures(1, &refractionDepthTexture);
	glDeleteTextures(1, &normalMap);
	glDeleteTextures(1, &dudvMap);
}

void WaterRenderer::generate(GLfloat waterHeight)
{
	int i = 0;
	vertices[i++] = -1.0f;
	vertices[i++] = waterHeight;
	vertices[i++] = -1.0f;

	vertices[i++] = -1.0f;
	vertices[i++] = waterHeight;
	vertices[i++] = 1.0f;

	vertices[i++] = 1.0f;
	vertices[i++] = waterHeight;
	vertices[i++] = -1.0f;

	vertices[i++] = 1.0f;
	vertices[i++] = waterHeight;
	vertices[i++] = -1.0f;

	vertices[i++] = -1.0f;
	vertices[i++] = waterHeight;
	vertices[i++] = 1.0f;

	vertices[i++] = 1.0f;
	vertices[i++] = waterHeight;
	vertices[i++] = 1.0f;

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

void WaterRenderer::Render(Camera& camera, GLfloat deltaTime)
{
	// use shader
	shader.Use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform3f(glGetUniformLocation(shader.Program, "sun.direction"), -0.2f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(shader.Program, "sun.specular"), 0.5f, 0.5f, 0.5f);

	waveMoveFactor += waveSpeed * deltaTime;
	waveMoveFactor = std::fmod(waveMoveFactor, 1.0);
	glUniform1f(glGetUniformLocation(shader.Program, "waveMoveFactor"), waveMoveFactor);
	glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// view matrix
	glm::mat4 view;
	view = camera.GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	// projection matrix
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(camera.Zoom), WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// model matrix
	glm::mat4 model;
	//model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(rows/2, 0.0f, cols/2));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glUniform1i(glGetUniformLocation(shader.Program, "reflectionTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glUniform1i(glGetUniformLocation(shader.Program, "refractionTexture"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	glUniform1i(glGetUniformLocation(shader.Program, "dudvMap"), 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glUniform1i(glGetUniformLocation(shader.Program, "normalMap"), 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
	glUniform1i(glGetUniformLocation(shader.Program, "depthMap"), 4);

	// draw
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glDisable(GL_BLEND);
}

void WaterRenderer::BindReflectionBuffer()
{
	//glBindTexture(GL_TEXTURE_2D, 0); //?
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glViewport(0, 0, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}

void WaterRenderer::BindRefractionBuffer()
{
	//glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glViewport(0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterRenderer::UnbindBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}