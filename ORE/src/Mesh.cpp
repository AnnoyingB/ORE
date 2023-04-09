#include "orepch.h"
#include "Mesh.h"

namespace ORE {
	Mesh::Mesh(MeshCreateInfo meshInfo) {
		m_shader = Shader(meshInfo.shaderPath);

		m_vao = VertexArray();
		m_vao.Bind();

		m_vbo = VertexBuffer(meshInfo.vertices);
		m_vbo.Bind();

		m_ibo = IndexBuffer(meshInfo.indices);
		m_ibo.Bind();

		m_vao.SetData();

		m_ibo.Unbind();
		m_vao.Unbind();
	}

	void Mesh::Draw() {
		m_vao.Bind();
		m_shader.Bind();
		
		std::vector<unsigned int> indices = GetIBO().GetIndices();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

		m_vao.Unbind();
		GLCheckError();
	}
}