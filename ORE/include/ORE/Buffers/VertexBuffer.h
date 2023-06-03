#pragma once
#include "Buffer.h"
#include "Debugging.h"

namespace ORE {
	struct Vertex {
		glm::vec3 position;
		glm::vec4 colour; // RGBA
		glm::vec3 normal;
		glm::vec3 texCoord;

		Vertex() = default;
		Vertex(glm::vec3 pos, glm::vec4 col, glm::vec3 norm, glm::vec3 tex = glm::vec3(0.f))
			: position(pos), colour(col), normal(norm), texCoord(tex) {}

		bool operator==(const Vertex& vert) const {
			if (this->position == vert.position &&
				this->colour == vert.colour &&
				this->normal == vert.normal)
				return true;
			return false;
		}
	};

	class VertexBuffer : public Buffer
	{
	private:
		std::vector<Vertex> _vertices;
	public:
		VertexBuffer() { glGenBuffers(1, &bufferID); bufferType = GL_ARRAY_BUFFER; }
		VertexBuffer(std::vector<Vertex> vertices);
		virtual ~VertexBuffer();

		// More specific type, does the same thing
		void ChangeData(std::vector<Vertex> data = std::vector<Vertex>());
		std::vector<Vertex> GetConstVertices() const { return _vertices; }
		std::vector<Vertex>& GetVertices() { return _vertices; }
		void AddVertex(Vertex value) { _vertices.push_back(value); }

		operator unsigned int() { return bufferID; }
	};
}
