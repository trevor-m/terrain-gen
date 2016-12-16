#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "TerrainRenderer.h"
#include "WaterRenderer.h"


// callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// movement
void do_movement();

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

	// terrain params
	GLfloat waterHeight = 0.0f;

	// generate terrain
	TerrainRenderer terrain(WIDTH, HEIGHT);
	WaterRenderer water(WIDTH, HEIGHT, 200, 200, waterHeight);


	
	// water
	//GLuint reflectionFBO, refractionFBO;
	//glGenFramebuffers(1, &reflectionFBO);
	//glGenFramebuffers(1, &refractionFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);

	

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

		
		terrain.PrepareRender(waterHeight);
		terrain.Render(camera);
		water.Render(camera);

		// swap buffers
		glfwSwapBuffers(window);

	}

	// exit
	glfwTerminate();
	return 0;
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