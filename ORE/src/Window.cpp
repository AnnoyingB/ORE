#include "orepch.h"
#include "Window.h"

namespace ORE {
	void ErrorCallback(int code, const char* description) {
		std::cout << "Code: " << code << " " << description << std::endl;
	}

	Window::~Window() {
		if (!init) return;
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool Window::Create(const char* name, glm::vec2 size, bool fullscreen, bool windowed) {
		if (!init)
			assert(glfwInit() && "Failed to init GLFW!");
		
		glfwSetErrorCallback(ErrorCallback);

		// GLFW OpenGL version 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		if (fullscreen && windowed) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			window = glfwCreateWindow(mode->width, mode->height, name, monitor, NULL);
			assert(window && "Failed to create GLFW window");
			
			glfwMakeContextCurrent(window);
			return true;
		}

		window = glfwCreateWindow(size.x, size.y, name, fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
		assert(window && "Failed to create GLFW window");
		
		glfwMakeContextCurrent(window);
		return true;
	}

	bool Window::InitGlad() {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}
		return true;
	}

	bool Window::ShouldClose() {
		return glfwWindowShouldClose(window);
	}

	void Window::ClearColor(float r, float g, float b, float a) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(r, g, b, a);
	}

	void Window::Poll() {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	void Window::SetCursor(std::string_view cursorPath, int xhot, int yhot)
	{
		int width, height, numComponents;
		unsigned char* imageData = stbi_load(cursorPath.data(), &width, &height, &numComponents, 4);

		if (imageData == NULL) {
			std::cout << "Failed to load image for cursor" << std::endl;
			return;
		}

		GLFWimage* image;
		image->width = width;
		image->height = height;
		image->pixels = imageData;

		stbi_image_free(imageData);

		GLFWcursor* cursor = glfwCreateCursor(image, xhot, yhot);
		glfwSetCursor(window, cursor);
	}
}
