#pragma once
#include <assert.h>
#include "VertexBuffer.h"

namespace ORE {
	class VertexArray : public Buffer
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		void Bind() override;
		void Unbind() override;
		// Do not use ChangeData, use SetData.
		void ChangeData(void*) override;
		/*
		  Default layout:
			0 = Position
			1 = Color
			2 = Normal

		  Bind the VAO first and then any other buffers to use as data.
		*/
		void SetData();
	};
}
