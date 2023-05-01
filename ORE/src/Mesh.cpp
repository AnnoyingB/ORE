#include "orepch.h"
#include "Mesh.h"

namespace ORE {
	Mesh::Mesh(MeshCreateInfo meshInfo) {
		Material = new ORE::Material(meshInfo.shaderPath);
		GLCheckError();

		m_vao = new VertexArray();
		GLCheckError();
		m_vao->Bind();

		m_vbo = new VertexBuffer(meshInfo.vertices);
		GLCheckError();
		m_vbo->Bind();

		m_ibo = new IndexBuffer(meshInfo.indices);
		GLCheckError();
		m_ibo->Bind();

		m_vao->SetData();

		m_ibo->Unbind();
		m_vao->Unbind();
		GLCheckError();

		ModelMatrix = glm::mat4(1.f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1, 0, 0));

		Material->GetConstShader().SetInt("primitiveID", meshInfo.meshID);
	}

	Mesh::~Mesh() {
		delete m_vao;
		delete m_vbo;
		delete m_ibo;

		if (Material)
			delete Material;
	}

	void Mesh::Apply(Camera camera) {
		Material->GetConstShader().SetMat4("projection", camera.GetProjection());
		Material->GetConstShader().SetMat4("view", camera.GetView());
		Material->GetConstShader().SetMat4("model", ModelMatrix);
	}

	void Mesh::Draw(Camera cam) {
		GLCheckError();
		m_vao->Bind();
		Material->GetConstShader().Bind();
		GLCheckError();
		Apply(cam);
		GLCheckError();
		
		std::vector<unsigned int> indices = GetIBO().GetIndices();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

		m_vao->Unbind();
		GLCheckError();
	}

	void Mesh::SetID(int id) {
		Material->GetConstShader().SetInt("primitiveID", id);
	}
}