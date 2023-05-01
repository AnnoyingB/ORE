#include "Material.h"
#include "orepch.h"

namespace ORE {
	Material::Material(std::string shaderPath) {
		m_shader = new Shader(shaderPath);
		GLCheckError();
	}

	Material::~Material() {
		delete m_shader;
	}
}
