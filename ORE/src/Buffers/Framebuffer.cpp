#include "orepch.h"
#include "Buffers/Framebuffer.h"

namespace ORE {
	Framebuffer::Framebuffer(int width, int height) {
		Create(width, height);
	}

	void Framebuffer::Create(int width, int height) {
		glGenFramebuffers(1, &bufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
			GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
		);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Failed to create framebuffer" << std::endl;
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		this->width = width;
		this->height = height;
	}

	void Framebuffer::Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
		glViewport(0, 0, width, height);
	}

	void Framebuffer::Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glm::vec4 Framebuffer::ReadPixel(int x, int y) {
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		glm::vec4 colData;
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &colData[0]);
		glReadBuffer(GL_NONE);
		return colData;
	}

	int Framebuffer::ReadPixelID(int x, int y) {
		glReadBuffer(GL_COLOR_ATTACHMENT1);

		int id;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &id);
		glReadBuffer(GL_NONE);
		return id;
	}

	Framebuffer::~Framebuffer() {
		glDeleteFramebuffers(1, &bufferID);
	}
}