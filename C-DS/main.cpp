#include "App.h"

App *app;

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void window_refresh_callback(GLFWwindow* window)
{
	app->update();
	app->render();
	glFinish();
}

int main() {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		exit(1);
	app = new App("C-DS");

	glfwSetWindowRefreshCallback(app->getWindow(), window_refresh_callback);
	glfwSetFramebufferSizeCallback(app->getWindow(), framebuffer_size_callback);
	
	app->run();

	return 0;
}