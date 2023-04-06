#pragma once
#include "Buffer.h"

namespace ORE {
	class Framebuffer : public Buffer {
	public:
		Framebuffer(int width = 1280, int height = 1920);
		virtual ~Framebuffer();

		void Create(int width, int height);
		void Bind() override;
		void Unbind() override;
	};
}
