#include "TerrainRenderer.h"



TerrainRenderer::TerrainRenderer(GLuint WIDTH, GLuint HEIGHT)
	: shader("terrainShader.vert", "terrainShader.frag"), Renderer(WIDTH, HEIGHT)
{

	// load textures
	grassTexture = loadTexture("grass.jpg");
	sandTexture = loadTexture("sand.jpg");

	generate();
}


TerrainRenderer::~TerrainRenderer()
{
	// deallocate
	delete[] terrainVertices;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &grassTexture);
	glDeleteTextures(1, &sandTexture);
}

void TerrainRenderer::generate() {
	PerlinNoise noise(237);

	double dist = 20.0;
	double heightScale = 20;
	double heightOffset = -9;
	bufferSize = cols*rows * 8 * 2;
	terrainVertices = new GLfloat[bufferSize];
	int i = 0;
	for (int z = 0; z < cols; z++) {
		for (int x = 0; x < rows; x++) {
			// position
			terrainVertices[i++] = x;
			terrainVertices[i++] = heightScale*noise.noise(x / dist, z / dist, 0.8) + heightOffset;
			terrainVertices[i++] = z;
			// texture
			terrainVertices[i++] = (x % 2 == 0) ? 0.0f : 1.0f;
			terrainVertices[i++] = 1.0f;
			// normal
			terrainVertices[i++] = 0.0f;
			terrainVertices[i++] = 0.0f;
			terrainVertices[i++] = 0.0f;

			// position
			terrainVertices[i++] = x;
			terrainVertices[i++] = heightScale*noise.noise(x / dist, (z + 1) / dist, 0.8) + heightOffset;
			terrainVertices[i++] = z + 1;
			// texture
			terrainVertices[i++] = (x % 2 == 0) ? 0.0f : 1.0f;
			terrainVertices[i++] = 0.0f;
			// normal
			terrainVertices[i++] = 0.0f;
			terrainVertices[i++] = 0.0f;
			terrainVertices[i++] = 0.0f;
		}
	}

	// calculate normals
	double step = 0.1;
	glm::vec3 normals[cols][rows];
	for (int z = 0; z < cols; z++) {
		for (int x = 0; x < rows; x++) {
			double y = heightScale*noise.noise(x / dist, z / dist, 0.8) + heightOffset;
			//double dxdy = (x - (x - 0.1)) / (y - noise.noise((x - 0.1) / dist, z, 0.8));
			//double dzdy = (z - (z - 0.1)) / (y - noise.noise(x, (z - 0.1) / dist, 0.8));
			double newY = heightScale*noise.noise((x - step) / dist, z / dist, 0.8) + heightOffset;
			glm::vec3 dxdy = glm::vec3(step, newY - y, 0);
			newY = heightScale*noise.noise(x / dist, (z - step) / dist, 0.8) + heightOffset;
			glm::vec3 dzdy = glm::vec3(0, newY - y, step);

			normals[z][x] = glm::normalize(glm::cross(dzdy, dxdy));
		}
	}
	//apply normals
	i = 0;
	for (int z = 0; z < cols - 1; z++) {
		for (int x = 0; x < rows; x++) {
			i += 5;
			terrainVertices[i++] = normals[z][x].x;
			terrainVertices[i++] = normals[z][x].y;
			terrainVertices[i++] = normals[z][x].z;
			i += 5;
			terrainVertices[i++] = normals[z + 1][x].x;
			terrainVertices[i++] = normals[z + 1][x].y;
			terrainVertices[i++] = normals[z + 1][x].z;
		}
	}

	// create buffers for data
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	// set VAO and VBO data
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(GLfloat), terrainVertices, GL_STATIC_DRAW);
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TerrainRenderer::PrepareRender(GLfloat waterHeight) {
	// set shader 'constants'
	shader.Use();
	glUniform1f(glGetUniformLocation(shader.Program, "waterHeight"), waterHeight);
	glUniform3f(glGetUniformLocation(shader.Program, "sun.direction"), -0.2f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(shader.Program, "sun.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shader.Program, "sun.diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shader.Program, "sun.specular"), 0.5f, 0.5f, 0.5f);
}

void TerrainRenderer::Render(Camera& camera) {
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
	model = glm::translate(model, glm::vec3(-cols / 2, 0.0f, -rows / 2));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glUniform1i(glGetUniformLocation(shader.Program, "grassTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sandTexture);
	glUniform1i(glGetUniformLocation(shader.Program, "sandTexture"), 1);

	// draw
	for (int i = 0; i < cols; i++) {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, i * rows * 2, rows * 2);
		glBindVertexArray(0);
	}
}