#include<GL/glew.h>     // use this before GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"
#include "Shader.h"
#include "Config.h"
#include "CubeMap.h"

GLuint loadBMP_custom(const char* imagepath);

Conf config; //************************************************************************

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void saveConfig();

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		saveConfig();
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void saveConfig()
{
	config.cameraPosition = { camera.Position.x, camera.Position.y, camera.Position.z };
	config.cameraYaw = camera.Yaw;
	config.cameraPitch = camera.Pitch;
	config.cameraZoom = camera.Zoom;

	config.lightPos = { 2.0f, 0.0f, 0.0f };
	config.lightAmbDiffSpec = { { 0.8f, 0.8f, 0.8f }, { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } };

	config.matAmbDiffSpec = { { 1.0f, 0.5f, 0.31f }, { 1.0f, 0.5f, 0.31f }, { 0.5f, 0.5f, 0.5f } };
	config.matShininess = 32.0f;
	config.save();
}

int main()
{
	GLFWwindow* window;

	// Initialize the library 
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context 
	window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);
	// Make the window's context current 
	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//create_faces_data();

	glewInit();

	config.load();

	camera.Position = glm::vec3(config.cameraPosition[0], config.cameraPosition[1], config.cameraPosition[2]);
	camera.Yaw = config.cameraYaw;
	camera.Pitch = config.cameraPitch;
	camera.Zoom = config.cameraZoom;
	firstMouse = true;
	camera.refresh();

	CubeMap cubeMap(10.0f);

	glActiveTexture(0);

	// Loop until the user closes the window 
	while (!glfwWindowShouldClose(window))
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glDepthMask(GL_FALSE);
		cubeMap.draw(camera);

		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glUseProgram(0);
	glfwTerminate();
	return 0;
}
