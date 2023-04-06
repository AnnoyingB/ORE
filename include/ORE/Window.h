#pragma once
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

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

		void Create(const char* name, glm::vec2 size, bool fullscreen = false, bool windowed = true);
		bool ShouldClose();
		void Poll();
		void SetSizeCallback(GLFWwindowsizefun callback) { glfwSetWindowSizeCallback(window, callback); }

		operator GLFWwindow* () const {
			return window;
		}
	};
}
