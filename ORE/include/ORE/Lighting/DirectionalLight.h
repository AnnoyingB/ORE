#pragma once
#include "glm/glm.hpp"

#include <memory>

namespace ORE {
	// Set the current light, otherwise there will be no directional light.
	struct DirectionalLight {
		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;

		glm::vec3 direction;
	};

	inline std::unique_ptr<DirectionalLight> CurrentDirectionalLight = nullptr;
}