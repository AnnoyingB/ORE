#include "orepch.h"
#include "Mesh.h"

namespace ORE {
	void Mesh::Draw() {
		m_vao.Bind();
		m_shader.Bind();
		
		std::vector<unsigned int> indices = GetIBO().GetIndices();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

		m_vao.Unbind();
		GLCheckError();
	}
}