#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"

#include <vector>

namespace ORE {
	class Buffer
	{
	protected:
		unsigned int bufferID;
		unsigned int bufferType;
	public:
		virtual void Bind();
		virtual void Unbind();
		virtual void ChangeData(void* data);

		operator unsigned int() const {
			return bufferID;
		}
	};
}
