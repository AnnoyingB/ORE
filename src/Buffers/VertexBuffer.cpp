#include "orepch.h"
#include "Buffers/VertexBuffer.h"

namespace ORE {
	VertexBuffer::VertexBuffer() {
		glGenBuffers(1, &bufferID);
		Bind();
		glBufferData(bufferType, sizeof(_vertices.data()), (void*)_vertices.data(), GL_DYNAMIC_DRAW);
		Unbind();
	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &bufferID);
	}
	
	void VertexBuffer::ChangeData(std::vector<Vertex> data) {
		_vertices = data;
		Buffer::ChangeData(data.data());
	}
}
