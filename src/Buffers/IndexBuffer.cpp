#include "orepch.h"
#include "Buffers/IndexBuffer.h"

namespace ORE {
	IndexBuffer::IndexBuffer() {
		glGenBuffers(1, &bufferID);
		Bind();
		glBufferData(bufferType, sizeof(_indices.data()), _indices.data(), GL_DYNAMIC_DRAW);
		Unbind();
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &bufferID);
	}

	void IndexBuffer::ChangeData(std::vector<int> data) {
		_indices = data;
		Buffer::ChangeData(data.data());
	}
}
