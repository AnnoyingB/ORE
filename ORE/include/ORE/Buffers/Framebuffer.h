#pragma once
#include "Buffer.h"

namespace ORE {
	class Framebuffer : public Buffer {
	private:
		int width;
		int height;
	public:
		Framebuffer(int width = 1280, int height = 1920);
		virtual ~Framebuffer();

		// also used to resize the framebuffer.
		void Create(int width, int height);
		void Bind() override;
		void Unbind() override;

		glm::vec4 ReadPixel(int x, int y);
		int ReadPixelID(int x, int y);
	};
}
