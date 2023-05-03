#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace ORE {
	class Camera {
	public:
		// Perspective Variables
		float Fovy, Aspect;
		// Ortho Variables
		float Width, Height;
		float Far = 100.0f, Near = 0.1f;
		bool Ortho = false;

		glm::vec3 Position;
		glm::vec3 Rotation;

		Camera() = default;

		void SetAspect(glm::vec2 size);
		glm::mat4 GetProjection();
		glm::mat4 GetView(glm::vec3 upVector = glm::vec3(0, 1, 0));

		// Screen -> World
		// World -> Screen
	};
}