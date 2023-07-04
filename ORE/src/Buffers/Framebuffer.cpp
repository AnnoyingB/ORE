#include "orepch.h"
#include "Buffers/Framebuffer.h"
#include "Renderer.h"

namespace ORE {
	Skybox::Skybox(const std::string& hdrPath, Mesh* skyboxMesh) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_FRAMEBUFFER_SRGB);

		skybox = skyboxMesh;

		FrameBufferCreateInfo fbci;
		fbci.width = 1280;
		fbci.height = 720;
		fbci.internalDepthFormat = GL_DEPTH_COMPONENT24;
		fbci.attatchment = GL_DEPTH_ATTACHMENT;

		skyboxFBO = new Framebuffer(fbci);
		GLCheckError();

		TextureCreateInfo tci;
		tci.repeatS = ClampToEdge;
		tci.repeatT = ClampToEdge;
		tci.filterMin = Linear;
		tci.filterMag = Linear;
		tci.texturePath = hdrPath;
		tci.flip = true;
		tci.loadWithFloat = true;
		tci.textureFormat = GL_RGBA16F;
		//tci.textureFormat = GL_SRGB;
		tci.internalFormat = GL_RGBA;

		Texture* hdrTex = new Texture(tci);

		GLCheckError();

		tci.repeatR = ClampToEdge;
		tci.textureType = CubeMap;
		tci.width = 512;
		tci.height = 512;

		Texture* envTex = new Texture(tci);
		
		tci.width = 32;
		tci.height = 32;

		skyBoxShader = new Shader(Shader::SkyBoxShader);
		irradianceShader = new Shader(Shader::IrradianceShader);
		irradianceMap = new Texture(tci);
		hdrTexture = hdrTex;
		envCubemap = envTex;
		initialized = false;

		GLCheckError();

		//skyBoxShader->Bind();
		//skyBoxShader->SetInt("enviromentMap", 0);
	}

	void Skybox::DrawSkybox() {
		if (!initialized) {
			std::cout << "Setup skybox" << std::endl;
			skyboxFBO->DrawSkybox(*this);
			initialized = true;
		}

		skyBoxShader->Bind();
		skyBoxShader->SetMat4("view", Renderer::CurrentCamera.GetView());
		glActiveTexture(GL_TEXTURE0);
		hdrTexture->Bind();
		envCubemap->Bind();
		skybox->Draw(Renderer::CurrentCamera);
	}

	Framebuffer::Framebuffer(FrameBufferCreateInfo fboi, bool rbo) {
		Create(fboi, rbo);
	}

	void Framebuffer::Create(FrameBufferCreateInfo fboi, bool useRBO) {
		glGenFramebuffers(1, &bufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

		if (!useRBO) {
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboi.width, fboi.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);

			glTexImage2D(
				GL_TEXTURE_2D, 0, fboi.internalDepthFormat, fboi.width, fboi.height, 0,
				fboi.format, fboi.type, NULL
			);

			glFramebufferTexture2D(GL_FRAMEBUFFER, fboi.attatchment, GL_TEXTURE_2D, texID, 0);
		}

		if (useRBO) {
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, fboi.internalDepthFormat, fboi.width, fboi.height);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, fboi.attatchment, GL_RENDERBUFFER, rbo);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Failed to create framebuffer" << std::endl;
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		this->width = fboi.width;
		this->height = fboi.height;
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
	
	void Framebuffer::DrawSkybox(Skybox& skybox) {
		GLCheckError();

		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		// convert HDR equirectangular environment map to cubemap equivalent
		skybox.skyBoxShader->Bind();
		skybox.skyBoxShader->SetMat4("projection", Renderer::CurrentCamera.GetProjection());
		glActiveTexture(GL_TEXTURE0);
		skybox.hdrTexture->Bind();

		glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
		Bind();
		for (unsigned int i = 0; i < 6; ++i)
		{
			skybox.skyBoxShader->SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *skybox.envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			skybox.skybox->Draw(Renderer::CurrentCamera);
		}
		Unbind();

		Bind();
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		skybox.irradianceShader->Bind();
		skybox.irradianceShader->SetInt("enviromentMap", 0);
		skybox.irradianceShader->SetMat4("projection", Renderer::CurrentCamera.GetProjection());
		glActiveTexture(GL_TEXTURE0);
		skybox.envCubemap->Bind();

		glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
		Bind();
		for (unsigned int i = 0; i < 6; ++i)
		{
			skybox.irradianceShader->SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *skybox.irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			skybox.skybox->Draw(Renderer::CurrentCamera);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Unbind();
	}

	Framebuffer::~Framebuffer() {
		glDeleteFramebuffers(1, &bufferID);
	}
}