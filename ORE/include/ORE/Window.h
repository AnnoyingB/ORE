#pragma once
#ifndef API_VULKAN
#include "glad/glad.h"
#endif
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "stb_image.h"

namespace ORE {
	// Wrapper for making windows with GLFW
	// If you need the glfw window, just cast it.
	class Window
	{
	private:
		GLFWwindow* window;
		bool init = false;
	public:
		virtual ~Window();

		bool Create(const char* name, glm::vec2 size, bool fullscreen = false, bool windowed = true, bool vulkan = false);
#ifndef API_VULKAN
		bool InitGlad();
#endif
		bool ShouldClose();
#ifndef API_VULKAN
		void ClearColor(float r, float g, float b, float a);
#endif
		void Poll();
		glm::vec2 GetWindowSize();
		void SetSizeCallback(GLFWwindowsizefun callback) { glfwSetWindowSizeCallback(window, callback); }

		void SetCursor(std::string_view cursorPath, int xhot, int yhot);

		operator GLFWwindow* () const {
			return window;
		}
	};
}
