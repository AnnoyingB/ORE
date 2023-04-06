#include "orepch.h"
#include "glad/glad.h"
#include "Window.h"

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	ORE::Window window;
	window.Create("ORE", { 1280, 720 });

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	window.SetSizeCallback(WindowSizeCallback);

	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1, 0, 0, 1);

		window.Poll();
	}
	return 0;
}