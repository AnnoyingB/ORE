#include "orepch.h"
#include "Window.h"

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	ORE::Window window;
	window.Create("ORE", { 1280, 720 });
	window.InitGlad();
	window.SetSizeCallback(WindowSizeCallback);

	while (!window.ShouldClose()) {
		window.ClearColor(1, 0, 0, 1);

		// Render

		window.Poll();
	}
	return 0;
}