#pragma once
#include "Lighting/DirectionalLight.h"
#include "Shader.h"

namespace ORE {
	class Material {
	private:
		Shader* m_shader;
	public:
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient;
		float shininess;

		Material() = default;
		Material(std::string);

		virtual ~Material();

		Shader& GetConstShader() const { return const_cast<Shader& const>(*m_shader); }
		Shader& GetShader() { return *m_shader; }

		void UpdateLighting();
		void Apply();
	};
}
