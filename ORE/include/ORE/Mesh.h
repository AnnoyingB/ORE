#pragma once
#include "Buffers/VertexArray.h"
#include "Buffers/IndexBuffer.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"
#include "glm/mat4x4.hpp"
#include "Debugging.h"

#include <iostream>

namespace ORE {
	struct MeshCreateInfo {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::string shaderPath;
		int meshID = -1.0f; // Use some kind of ECS to use this for clicking on an item to select it
	};

	class Mesh {
	protected:
		VertexArray* m_vao;
		VertexBuffer* m_vbo;
		IndexBuffer* m_ibo;
	public:
		Mesh() = default;
		Mesh(MeshCreateInfo);

		virtual ~Mesh();

		Material* Material;
		glm::mat4 ModelMatrix;
		glm::vec3 Position = glm::vec3(0);
		glm::vec3 Scale = glm::vec3(1);
		glm::vec3 Rotation = glm::vec3(0);

		GLenum RenderMode = GL_TRIANGLES;

		// Hopefully accurate
		bool operator==(const Mesh& rhs) {
			if (rhs.GetConstVBO().GetVertices() == this->GetVBO().GetVertices())
				return true;
			return false;
		}
		
		// Hopefully works c:
		Mesh operator+(const Mesh& rhs)
		{
			VertexBuffer lhsVBO = this->GetVBO();
			std::vector<Vertex> lhsVertices = lhsVBO.GetVertices();
			std::vector<Vertex> rhsVertices = rhs.GetConstVBO().GetVertices();
			
			size_t size = lhsVertices.size() > rhsVertices.size() ? lhsVertices.size() : rhsVertices.size();
			Mesh smallestMesh = lhsVertices.size() == size ? *this : rhs;
			Mesh biggestMesh = &smallestMesh == this ? rhs : *this;

			// Try to simplify the mesh.
			bool changed = false;
			for (int i = 0;
				i < smallestMesh.GetConstVBO().GetConstVertices().size();i++
				) {
				Vertex& vert = smallestMesh.GetVBO().GetVertices()[i];
				Vertex& vert2 = biggestMesh.GetVBO().GetVertices()[i];
				if (vert == vert2) {
					changed = true;
					this->GetIBO().GetIndices().erase(GetIBO().GetIndices().begin() + i);
					this->GetVBO().GetVertices().erase(GetVBO().GetVertices().begin() + i);
				}
			}

			// If not changed just merge them.
			if (!changed) {
				for (int i = 0; i < rhs.GetConstVBO().GetConstVertices().size(); i++) {
					this->GetIBO().GetIndices().push_back(rhs.GetConstIBO().GetConstIndices()[i]);
					this->GetVBO().GetVertices().push_back(rhs.GetConstVBO().GetConstVertices()[i]);
				}
			}

			// If it has been changed and the size isn't the samee, merge em.
			if (this->GetVBO().GetConstVertices().size() < rhs.GetConstVBO().GetConstVertices().size() && changed) {
				for (int i = this->GetVBO().GetVertices().size();
					i < rhs.GetConstVBO().GetConstVertices().size();
					i++) {
					this->GetVBO().AddVertex(biggestMesh.GetVBO().GetVertices()[i]);
					this->GetIBO().GetIndices().push_back(biggestMesh.GetIBO().GetIndices()[i]);
				}
			}

			return *this;
		}

		VertexBuffer& GetConstVBO() const { return const_cast<VertexBuffer& const>(*m_vbo); }
		VertexBuffer& GetVBO() { return *m_vbo; }
		IndexBuffer& GetConstIBO() const { return const_cast<IndexBuffer& const>(*m_ibo); };
		IndexBuffer& GetIBO() { return *m_ibo; }
		Shader& GetConstShader() const { return this->Material->GetConstShader(); }
		Shader& GetShader() { return this->Material->GetShader(); }

		virtual void Draw(Camera cam);

		void CreateMatrix();
		void Apply(Camera camera);
		void SetID(int id);
	};
}
