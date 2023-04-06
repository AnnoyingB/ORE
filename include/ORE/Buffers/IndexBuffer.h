#pragma once
#include "Buffer.h"

namespace ORE {
	class IndexBuffer : public Buffer
	{
	private:
		std::vector<unsigned int> _indices;
		unsigned int bufferType = GL_ELEMENT_ARRAY_BUFFER;
	public:
		IndexBuffer();
		virtual ~IndexBuffer();

		// More specific type, does the same thing
		void ChangeData(std::vector<int> data);
		std::vector<unsigned int> GetConstIndices() const { return _indices; }
		std::vector<unsigned int>& GetIndices() { return _indices; }
	};
}
