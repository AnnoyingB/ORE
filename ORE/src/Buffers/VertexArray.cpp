#include "orepch.h"
#include "Buffers/VertexArray.h"

namespace ORE {
	void VertexArray::CalculateOffsetAndStride() {
		assert(&val != nullptr);
		size_t offset = 0;
		for (VALayoutItem vali : val.Items) {
			vali.Offset = offset;
			offset += vali.GetLayoutSize();
			val.stride += vali.GetLayoutSize();
		}
	}
	
	VertexArray::VertexArray() {
		glGenVertexArrays(1, &bufferID);
	}

	VertexArray::VertexArray(VertexArrayLayout val) {
		this->val = val;
		CalculateOffsetAndStride();
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
		SetData();
	}

	// doesn't work but oh well :p
	void VertexArray::SetData(VertexArrayLayout val) {
		/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)(7 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)(10 * sizeof(float)));
		glEnableVertexAttribArray(3);*/
		this->val = val;
		CalculateOffsetAndStride();
		for (int i = 0; i < val.Items.size(); i++) {
			VALayoutItem vali = val.Items[i];
			switch (vali.LayoutType)
			{
				case Float:
				case Float2:
				case Float3:
				case Float4:
				{
					glBindBuffer(GL_ARRAY_BUFFER, vali.BufferID);
					glEnableVertexAttribArray(i);
					glVertexAttribPointer(i, vali.GetTypeSize(), vali.TypeToGL(), vali.Normalized, val.stride, (void*)vali.Offset);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					break;
				}
				case Int:
				case Bool:
				{
					glBindBuffer(GL_ARRAY_BUFFER, vali.BufferID);
					glEnableVertexAttribArray(i);
					glVertexAttribIPointer(i, vali.GetLayoutSize(), vali.TypeToGL(), val.stride, (const void*)vali.Offset);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					break;
				}
				case Mat3:
				case Mat4:
				{
					glBindBuffer(GL_ARRAY_BUFFER, vali.BufferID);
					uint32_t count = vali.GetTypeSize();
					uint32_t ii = 0;
					for (uint32_t j = i; j < i + count; j++) {
						glEnableVertexAttribArray(j);
						glVertexAttribPointer(j,
							count,
							vali.TypeToGL(),
							vali.Normalized,
							val.stride,
							(const void*)(vali.Offset + sizeof(float) * count * ii));
						glVertexAttribDivisor(j, 1);
						ii++;
					}
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					break;
				}
			}
		}
	}

	void VertexArray::SetData() {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)(7 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(float), (void*)(10 * sizeof(float)));
		glEnableVertexAttribArray(3);
	}
}
