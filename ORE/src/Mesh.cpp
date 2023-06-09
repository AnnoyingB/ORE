#include "orepch.h"
#include "Mesh.h"
#include "glm/gtx/quaternion.hpp"

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
		GLCheckError();

		m_vao->Unbind();
		GLCheckError();

		ModelMatrix = glm::mat4(1.f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1, 0, 0));

		m_vbo->Unbind();
		m_ibo->Unbind();
		GLCheckError();
		//Material->GetConstShader().SetInt("primitiveID", meshInfo.meshID);
		GLCheckError();
	}

	Mesh::~Mesh() {
		delete m_vao;
		delete m_vbo;
		delete m_ibo;

		if (Material)
			delete Material;
	}

	void Mesh::CreateMatrix() {
		auto r = glm::mat4_cast(glm::quat(Rotation));
		auto t = glm::translate(glm::mat4(1.f), Position);
		auto s = glm::scale(glm::mat4(1.f), Scale);
		ModelMatrix = t * r * s;
	}

	void Mesh::Apply(Camera camera) {
		CreateMatrix();
		Material->GetConstShader().SetMat4("projection", camera.GetProjection());
		Material->GetConstShader().SetMat4("view", camera.GetView());
		Material->GetConstShader().SetMat4("model", ModelMatrix);
		Material->GetConstShader().SetMat3("normalMatrix", glm::transpose(glm::inverse(ModelMatrix)));
	}

	void Mesh::Draw(Camera cam) {
		GLCheckError();
		m_vao->Bind();
		Material->GetConstShader().Bind();
		GLCheckError();
		//Apply(cam);
		GLCheckError();
		m_ibo->Bind();
		
		glDrawElements(RenderMode, m_ibo->GetConstIndices().size(), GL_UNSIGNED_INT, 0);
		GLCheckError();

		m_ibo->Unbind();
		m_vao->Unbind();
		GLCheckError();
	}

	void Mesh::SetID(int id) {
		Material->GetConstShader().SetInt("primitiveID", id);
	}
}