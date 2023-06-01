#include "Material.h"
#include "orepch.h"

namespace ORE {
	Material::Material(std::string shaderPath) {
		m_shader = new Shader(shaderPath);
		GLCheckError();

		albedo = glm::vec3(0);
		metallic = 0.f;
		roughness = 0.f;
		ao = 0.f;
		GLCheckError();

		m_shader->Bind();
		Apply();
	}

	Material::~Material() {
		delete m_shader;
	}

	void Material::UpdateLighting() {
		if (!CurrentDirectionalLight) {
			printf("No direcitional light");
			return;
		}
		m_shader->SetVec3("lightPositions[0]", CurrentDirectionalLight->direction);
		m_shader->SetVec3("lightColors[0]", CurrentDirectionalLight->ambient);

		/*for (int i = 0; i < PointLights.size(); i++) {
			if (i > 32) assert(false && "Max lights is 32 in shader");

			std::string pointLightStr = "pointLights[" + std::to_string(i) + "]";
			m_shader->SetVec3(pointLightStr + ".position", PointLights[i]->position);
			m_shader->SetFloat(pointLightStr + ".constant", PointLights[i]->constant);
			m_shader->SetFloat(pointLightStr + ".linear", PointLights[i]->linear);
			m_shader->SetFloat(pointLightStr + ".quadratic", PointLights[i]->quadratic);
			m_shader->SetVec3(pointLightStr + ".ambient", PointLights[i]->ambient);
			m_shader->SetVec3(pointLightStr + ".diffuse", PointLights[i]->diffuse);
			m_shader->SetVec3(pointLightStr + ".specular", PointLights[i]->specular);
		}*/
	}

	void Material::Apply() {
		m_shader->SetVec3("albedo", albedo);
		m_shader->SetFloat("metallic", metallic);
		m_shader->SetFloat("roughness", roughness);
		m_shader->SetFloat("ao", ao);
	}

	void Material::SetMatrices(const Camera& cam, glm::mat4 modelMatrix) {
		m_shader->SetMat4("projection", cam.GetProjection());
		m_shader->SetMat4("view", cam.GetView());
		m_shader->SetMat4("model", modelMatrix);
	}
}
