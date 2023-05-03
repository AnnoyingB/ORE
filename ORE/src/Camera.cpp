#include "Camera.h"
#include "orepch.h"

namespace ORE {
	void Camera::SetAspect(glm::vec2 size) {
		Aspect = size.x / size.y;
	}

	glm::mat4 Camera::GetProjection() {
		return Ortho ?
			glm::ortho(Position.x, Width, Height, Position.y, Near, Far)
			: glm::perspective(glm::radians(Fovy), Aspect, Near, Far);
	}

	glm::mat4 Camera::GetView(glm::vec3 upVector) {
		glm::mat4 camera = glm::lookAt(Position,
			Position + glm::vec3(0.f, 0.f, -1.f),
			upVector);

		//return glm::inverse(camera);
		return camera;
	}
}