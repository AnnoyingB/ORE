#include "orepch.h"
#include "Buffers/VertexArray.h"

namespace ORE {
	VertexArray::VertexArray() {
		glGenVertexArrays(1, &bufferID);
	}
	
	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &bufferID);
	}

	void VertexArray::Bind() {
		glBindVertexArray(bufferID);
	}

	void VertexArray::Unbind() {
		glBindVertexArray(0);
	}

	void VertexArray::ChangeData(void*) {
		// You used ChangeData instead of SetData didn't you
		// :/
		assert(true);
	}

	void VertexArray::SetData() {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
}
