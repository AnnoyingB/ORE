#pragma once
#include <assert.h>
#include "VertexBuffer.h"

namespace ORE {
	enum VALayoutType {
		Int = 0,
		Bool,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4
	};

	struct VALayoutItem {
		VALayoutType LayoutType;
		size_t Offset;
		bool Normalized;
		GLuint BufferID;

		uint32_t GetLayoutSize() {
			switch (LayoutType) {
				case Int:   return sizeof(int);
				case Bool:  return sizeof(bool);
				case Float: return sizeof(float);
				case Float2: return sizeof(float) * 2;
				case Float3: return sizeof(float) * 3;
				case Float4: return 4 * 4;
				case Mat3:  return 4 * 3 * 3;
				case Mat4:  return 4 * 4 * 4;
			}
		}

		uint32_t GetTypeSize() {
			switch (LayoutType) {
				case Int: return 1;
				case Bool: return 1;
				case Float: return 1;
				case Float2: return 2;
				case Float3: return 3;
				case Mat3: return 3;
				case Float4: return 4;
				case Mat4: return 4;
			}
		}

		GLenum TypeToGL() {
			switch (LayoutType) {
			case Int: return GL_INT; break;
			case Bool: return GL_BOOL; break;
				case Float:
				case Float2:
				case Float3:
				case Float4:
				case Mat4:
				case Mat3:
					return GL_FLOAT;
			}
		}
	};

	struct VertexArrayLayout {
		size_t stride;
		std::vector<VALayoutItem> Items;
	};

	class VertexArray : public Buffer
	{
	private:
		VertexArrayLayout val;

		void CalculateOffsetAndStride();
	public:
		VertexArray();
		VertexArray(VertexArrayLayout val);
		virtual ~VertexArray();

		void Bind() override;
		void Unbind() override;
		void ChangeData(void*) override;
		/*
		  Default layout in all shaders:
			0 = Position
			1 = Color
			2 = Normal

		  Bind the VAO.
		*/
		virtual void SetData();
		virtual void SetData(VertexArrayLayout);
	};
}
