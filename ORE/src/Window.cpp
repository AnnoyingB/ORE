#include "orepch.h"
#include "Window.h"
#ifndef API_VULKAN
#include "Renderer.h"
#endif

namespace ORE {
	void ErrorCallback(int code, const char* description) {
		std::cout << "Code: " << code << " " << description << std::endl;
	}

#ifndef API_VULKAN
	void DebugMessageCallback([[maybe_unused]] GLenum source, GLenum type, [[maybe_unused]] GLuint id, [[maybe_unused]] GLenum severity, [[maybe_unused]] GLsizei messageLength, const GLchar* message, const void*)
	{
		if (type == GL_DEBUG_TYPE_ERROR)
		{
			printf("GL: DEBUG CALLBACK type = %i, source = %i, severity = error, message = %s\n", type, source, message);
		}
	}

	void WindowSizeCallback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		Renderer::CurrentCamera.Width = width;
		Renderer::CurrentCamera.Height = height;
	}
#endif

	Window::~Window() {
		if (!init) return;
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool Window::Create(const char* name, glm::vec2 size, bool fullscreen, bool windowed, bool vulkan) {
		if (!init) {
			assert(glfwInit() && "Failed to init GLFW!");
			init = true;
		}
		
		glfwSetErrorCallback(ErrorCallback);

		if (vulkan)
			if (!glfwVulkanSupported())
				abort();

		if (!vulkan) {
			// GLFW OpenGL version 4.5
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		}
		else {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		if (fullscreen && windowed) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			window = glfwCreateWindow(mode->width, mode->height, name, monitor, NULL);
			assert(window && "Failed to create GLFW window");
			
			if(!vulkan) glfwMakeContextCurrent(window);
			return true;
		}

		window = glfwCreateWindow(size.x, size.y, name, fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
		assert(window && "Failed to create GLFW window");
		
		if(!vulkan) glfwMakeContextCurrent(window);
		return true;
	}

#ifndef API_VULKAN
	bool Window::InitGlad() {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugMessageCallback, nullptr);

		SetSizeCallback(WindowSizeCallback);

		Renderer::Init(window);

		return true;
	}
#endif

	bool Window::ShouldClose() {
		return glfwWindowShouldClose(window);
	}

#ifndef API_VULKAN
	void Window::ClearColor(float r, float g, float b, float a) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(r, g, b, a);
	}
#endif

	void Window::Poll() {
		glfwPollEvents();
#ifdef API_OPENGL
		glfwSwapBuffers(window);
#endif
	}

	glm::vec2 Window::GetWindowSize() {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		return { width, height };
	}

	void Window::SetCursor(std::string_view cursorPath, int xhot, int yhot)
	{
		int width, height, numComponents;
		unsigned char* imageData = stbi_load(cursorPath.data(), &width, &height, &numComponents, 4);

		if (imageData == NULL) {
			std::cout << "Failed to load image for cursor" << std::endl;
			return;
		}

		GLFWimage* image = new GLFWimage();
		image->width = width;
		image->height = height;
		image->pixels = imageData;

		stbi_image_free(imageData);

		GLFWcursor* cursor = glfwCreateCursor(image, xhot, yhot);
		glfwSetCursor(window, cursor);
	}
}
