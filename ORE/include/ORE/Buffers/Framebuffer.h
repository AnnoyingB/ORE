#pragma once
#include "Mesh.h"
#include "Buffer.h"

namespace ORE {
	class Framebuffer;

	struct Skybox {
		//Framebuffer* skyboxFBO;
		Mesh* skybox;
		Shader* skyBoxShader;
		Texture *hdrTexture, *envCubemap;

		Skybox(const std::string& hdrPath, Mesh* skyboxMesh);
		void DrawSkybox();
	};

	struct FrameBufferCreateInfo {
		int width, height;
		GLenum internalDepthFormat = GL_DEPTH24_STENCIL8, attatchment = GL_DEPTH_STENCIL_ATTACHMENT;
		GLenum format = GL_DEPTH_STENCIL, type = GL_UNSIGNED_INT_24_8; // only used for the textrure
	};

	class Framebuffer : public Buffer {
	private:
		int width;
		int height;
	public:
		Framebuffer(FrameBufferCreateInfo fboi = { 1280, 1920 }, bool rbo = true);
		virtual ~Framebuffer();

		// also used to resize the framebuffer.
		void Create(FrameBufferCreateInfo, bool rbo = true);
		void Bind() override;
		void Unbind() override;

		glm::vec4 ReadPixel(int x, int y);
		int ReadPixelID(int x, int y);

		// Only if you are using a cube map
		void SetupCubeMap(Skybox&);
	};
}
