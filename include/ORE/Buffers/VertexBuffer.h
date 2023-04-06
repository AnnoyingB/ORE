#pragma once
#include "Buffer.h"

namespace ORE {
	struct Vertex {
		glm::vec3 position;
		glm::vec4 colour; // RGBA
		glm::vec3 normal;

		bool operator==(const Vertex& vert) {
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
		unsigned int bufferType = GL_ARRAY_BUFFER;
	public:
		VertexBuffer();
		virtual ~VertexBuffer();

		// More specific type, does the same thing
		void ChangeData(std::vector<Vertex> data);
		std::vector<Vertex> GetConstVertices() const { return _vertices; }
		std::vector<Vertex>& GetVertices() { return _vertices; }
		void AddVertex(Vertex value) { _vertices.push_back(value); }
	};
}
