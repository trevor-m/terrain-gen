#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <SOIL.h>
#include "Shader.h"
#include "Camera.h"
#include "PerlinNoise.h"

// callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// movement
void do_movement();

// utility
GLuint loadTexture(GLchar* path);

// window dimensions
const GLuint WIDTH = 1800, HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 3.0f));
// input
bool keys[1024];
bool firstMouse = true;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// terrain params
const int cols = 200, rows = 200;
double dist = 20.0;
double heightScale = 20;
double heightOffset = -8;
GLfloat waterHeight = 0.0f;


int main() {
	// initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Terrain Gen", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// turn on GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// tell OpenGL size of rendering window
	glViewport(0, 0, WIDTH, HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	// register callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// disable cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// load shaders
	Shader terrainShader("terrainShader.vert", "terrainShader.frag");

	// load textures
	GLuint grassTexture = loadTexture("grass.jpg");
	GLuint sandTexture = loadTexture("sand.jpg");

	// generate terrain
	PerlinNoise noise(237);
	int bufferSize = cols*rows * 8 * 2;
	GLfloat* terrainVertices = new GLfloat[bufferSize];
	int i = 0;
	for (int z = 0; z < cols; z++) {
		for (int x = 0; x < rows; x++) {
			// position
			terrainVertices[i++] = x;
			terrainVertices[i++] = heightScale*noise.noise(x/dist, z/dist, 0.8) + heightOffset;
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
			terrainVertices[i++] = heightScale*noise.noise(x/ dist, (z+1)/ dist, 0.8) + heightOffset;
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
			glm::vec3 dxdy = glm::vec3(step, newY-y, 0);
			newY = heightScale*noise.noise(x / dist, (z - step) / dist, 0.8) + heightOffset;
			glm::vec3 dzdy = glm::vec3(0, newY-y, step);

			normals[z][x] = glm::normalize(glm::cross(dzdy, dxdy));
		}
	}
	//apply normals
	i = 0;
	for (int z = 0; z < cols-1; z++) {
		for (int x = 0; x < rows; x++) {
			i += 5;
			terrainVertices[i++] = normals[z][x].x;
			terrainVertices[i++] = normals[z][x].y;
			terrainVertices[i++] = normals[z][x].z;
			i += 5;
			terrainVertices[i++] = normals[z+1][x].x;
			terrainVertices[i++] = normals[z+1][x].y;
			terrainVertices[i++] = normals[z+1][x].z;
		}
	}

	// create buffers for data
	GLuint VBO, VAO;
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

	// set shader 'constants'
	terrainShader.Use();
	glUniform1f(glGetUniformLocation(terrainShader.Program, "waterHeight"), waterHeight);
	glUniform3f(glGetUniformLocation(terrainShader.Program, "sun.direction"), -0.2f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(terrainShader.Program, "sun.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(terrainShader.Program, "sun.diffuse"), 0.8f, 0.8f, 0.8f);
	//glUniform3f(glGetUniformLocation(lightingShader.Program, "sun.specular"), 0.5f, 0.5f, 0.5f);

	// game loop
	while (!glfwWindowShouldClose(window)) {
		// get deltatime
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// check and call events
		glfwPollEvents();
		do_movement();

		// render
		glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// use shader
		terrainShader.Use();
		// view
		glm::mat4 view;
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(terrainShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		// projection
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.Zoom), WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(terrainShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(-cols/2, 0.0f, -rows/2));
		glUniformMatrix4fv(glGetUniformLocation(terrainShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glUniform1i(glGetUniformLocation(terrainShader.Program, "grassTexture"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, sandTexture);
		glUniform1i(glGetUniformLocation(terrainShader.Program, "sandTexture"), 1);

		// draw
		for (int i = 0; i < cols; i++) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLE_STRIP, i * rows * 2, rows*2);
			glBindVertexArray(0);
		}

		// swap buffers
		glfwSwapBuffers(window);

	}
	
	// deallocate
	delete[] terrainVertices;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &grassTexture);
	glDeleteTextures(1, &sandTexture);

	// exit
	glfwTerminate();
	return 0;
}

GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;

}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera.ProcessMouseMovement(xoffset, yoffset);
}