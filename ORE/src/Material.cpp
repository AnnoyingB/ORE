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
		//if (!CurrentDirectionalLight) {
		//	printf("No direcitional light");
		//	return;
		//}
		//m_shader->SetVec3("lightPositions[0]", CurrentDirectionalLight->direction);
		//m_shader->SetVec3("lightColors[0]", CurrentDirectionalLight->ambient);

		for (int i = 1; i < PointLights.size() + 1; i++) {
			if (i > 32) assert(false && "Max lights is 32 in shader");

			if (i == 1) {
				glm::vec3 pos = glm::vec3(0);
				pos.z = 10 * std::cos(glm::radians(CurrentDirectionalLight->direction.x)) * std::sin(glm::radians(CurrentDirectionalLight->direction.z));
				pos.x = 10 * std::sin(glm::radians(CurrentDirectionalLight->direction.x)) * std::sin(glm::radians(CurrentDirectionalLight->direction.z));
				pos.y = 10 * std::cos(glm::radians(CurrentDirectionalLight->direction.z));
				m_shader->SetVec3("lightPositions[0]", pos);
				m_shader->SetVec3("lightColors[0]", CurrentDirectionalLight->ambient);
			}

			std::string pointLightStr = "lightPositions[" + std::to_string(i) + "]";
			std::string pointLightStr2 = "lightColors[" + std::to_string(i) + "]";
			/*m_shader->SetVec3(pointLightStr + ".position", PointLights[i]->position);
			m_shader->SetFloat(pointLightStr + ".constant", PointLights[i]->constant);
			m_shader->SetFloat(pointLightStr + ".linear", PointLights[i]->linear);
			m_shader->SetFloat(pointLightStr + ".quadratic", PointLights[i]->quadratic);
			m_shader->SetVec3(pointLightStr + ".ambient", PointLights[i]->ambient);
			m_shader->SetVec3(pointLightStr + ".diffuse", PointLights[i]->diffuse);
			m_shader->SetVec3(pointLightStr + ".specular", PointLights[i]->specular);*/
			m_shader->SetVec3(pointLightStr, PointLights[i - 1]->position);
			m_shader->SetVec3(pointLightStr2, PointLights[i - 1]->ambient);
		}
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

		m_shader->SetMat4("normalMatrix", glm::mat3(glm::transpose(glm::inverse(modelMatrix))));
	}
}
