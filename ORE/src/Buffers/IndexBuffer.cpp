#include "orepch.h"
#include "Buffers/IndexBuffer.h"

namespace ORE {
	IndexBuffer::IndexBuffer(std::vector<unsigned int> indices) {
		bufferType = GL_ELEMENT_ARRAY_BUFFER;
		_indices = indices;

		glGenBuffers(1, &bufferID);
		Bind();
		glBufferData(bufferType, _indices.size() * sizeof(unsigned int), _indices.data(), GL_DYNAMIC_DRAW);
		Unbind();
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &bufferID);
	}

	void IndexBuffer::ChangeData(std::vector<unsigned int> data) {
		_indices = data;
		Buffer::ChangeData(data.data());
	}
}
