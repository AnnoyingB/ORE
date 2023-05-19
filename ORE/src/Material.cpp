#include "Material.h"
#include "orepch.h"

namespace ORE {
	Material::Material(std::string shaderPath) {
		m_shader = new Shader(shaderPath);
		GLCheckError();

		diffuse = glm::vec3(0);
		ambient = glm::vec3(0);
		specular = glm::vec3(0);
		shininess = 0.f;

		Apply();
	}

	Material::~Material() {
		delete m_shader;
	}

	void Material::UpdateLighting() {
		m_shader->SetVec3("dirLight.direction", CurrentDirectionalLight->direction);
		m_shader->SetVec3("dirLight.diffuse", CurrentDirectionalLight->diffuse);
		m_shader->SetVec3("dirLight.ambient", CurrentDirectionalLight->ambient);
		m_shader->SetVec3("dirLight.specular", CurrentDirectionalLight->specular);
	}

	void Material::Apply() {
		m_shader->SetVec3("material.diffuse", diffuse);
		m_shader->SetVec3("material.ambient", ambient);
		m_shader->SetVec3("material.specular", specular);
		m_shader->SetFloat("material.shininess", shininess);
	}
}
