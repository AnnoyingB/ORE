#pragma once
#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"
#include "Shader.h"
#include "Camera.h"

namespace ORE {
	class Material {
	private:
		Shader* m_shader;
	public:
		glm::vec3 albedo;
		float metallic;
		float roughness;
		float ao;

		Material() = default;
		Material(std::string);

		virtual ~Material();

		Shader& GetConstShader() const { return const_cast<Shader& const>(*m_shader); }
		Shader& GetShader() { return *m_shader; }

		void UpdateLighting();
		void Apply();
		void SetMatrices(const Camera& cam, glm::mat4 modelMatrix);
	};
}
