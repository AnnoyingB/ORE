#pragma once
#include "Shader.h"

namespace ORE {
	class Material {
	private:
		Shader* m_shader;
	public:
		Material() = default;
		Material(std::string);

		virtual ~Material();

		Shader& GetConstShader() const { return const_cast<Shader& const>(*m_shader); }
		Shader& GetShader() { return *m_shader; }
	};
}
