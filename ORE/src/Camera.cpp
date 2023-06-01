#include "Camera.h"
#include "orepch.h"

namespace ORE {
	void Camera::SetAspect(glm::vec2 size) {
		Aspect = size.x / size.y;
	}

	glm::mat4 Camera::GetProjection() const {
		return Ortho ?
			glm::ortho(Position.x, Width, Height, Position.y, Near, Far)
			: glm::perspective(glm::radians(Fovy), Aspect, Near, Far);
	}

	glm::mat4 Camera::GetView(glm::vec3 upVector) const {
		glm::vec3 cameraFront = GetForwardVec();

		glm::mat4 camera = glm::lookAt(Position,
			Position + cameraFront,
			upVector);

		//return glm::inverse(camera);
		return camera;
	}

	glm::vec3 Camera::GetForwardVec() const {
		glm::vec3 direction;
		direction.x = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
		direction.y = sin(glm::radians(Rotation.y));
		direction.z = sin(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
		glm::vec3 cameraFront = glm::normalize(direction);
		return cameraFront;
	}
}