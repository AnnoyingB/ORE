#include "orepch.h"
#include "Buffers/Buffer.h"

namespace ORE {
	void Buffer::Bind() {
		glBindBuffer(bufferType, bufferID);
	}

	void Buffer::Unbind() {
		glBindBuffer(bufferType, 0);
	}

	void Buffer::ChangeData(void* data) {
		Bind();
		glBufferSubData(bufferType, 0, sizeof(data), data);
		Unbind();
	}
}
