#include "orepch.h"
#include "Buffers/VertexBuffer.h"

namespace ORE {
	VertexBuffer::VertexBuffer(std::vector<Vertex> vertices) {
		bufferType = GL_ARRAY_BUFFER;

		/*std::vector<float> vertexPositions;

		for (Vertex vert : vertices) {
			vertexPositions.push_back(vert.position.x);
			vertexPositions.push_back(vert.position.y);
			vertexPositions.push_back(vert.position.z);
		}*/

		GLCheckError();
		_vertices = vertices;

		glGenBuffers(1, &bufferID);
		GLCheckError();
		Bind();
		GLCheckError();
		glBufferData(bufferType, _vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
		GLCheckError();
		Unbind();
		GLCheckError();
	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &bufferID);
	}
	
	void VertexBuffer::ChangeData(std::vector<Vertex> data) {
		if(!data.empty())
			_vertices = data;
		Buffer::ChangeData(_vertices.data());
	}
}
