#include "orepch.h"
#include "Buffers/Framebuffer.h"
#include "Renderer.h"

namespace ORE {
	Skybox::Skybox(const std::string& hdrPath, Mesh* skyboxMesh) {
		skybox = skyboxMesh;

		/*FrameBufferCreateInfo fbci;
		fbci.width = 512;
		fbci.height = 512;
		fbci.internalDepthFormat = GL_DEPTH_COMPONENT24;
		fbci.attatchment = GL_DEPTH_ATTACHMENT;

		skyboxFBO = new Framebuffer(fbci);
		GLCheckError();*/

		TextureCreateInfo tci;
		tci.repeatS = ClampToEdge;
		tci.repeatT = ClampToEdge;
		tci.filterMin = Linear;
		tci.filterMag = Linear;
		//tci.texturePath = hdrPath;
		//tci.flip = true;

		//Texture* hdrTex = new Texture(tci);

		//GLCheckError();

		tci.repeatR = ClampToEdge;
		tci.textureType = CubeMap;
		//tci.textureFormat = GL_RGB;
		//tci.internalFormat = GL_RGB;

		Texture* envTex = new Texture(tci);

		skyBoxShader = new Shader(Shader::SkyBoxShader);
		//hdrTexture = hdrTex;
		envCubemap = envTex;

		GLCheckError();

		//skyBoxShader->Bind();
		//skyBoxShader->SetInt("enviromentMap", 0);
	}

	void Skybox::DrawSkybox() {
		glDepthMask(GL_FALSE);

		GLCheckError();
		Shader& skyBoxShader = skybox->GetShader();
		GLCheckError();
		glm::mat4 view = glm::mat4(glm::mat3(Renderer::CurrentCamera.GetView()));
		skyBoxShader.SetMat4("view", view);
		GLCheckError();
		skyBoxShader.SetMat4("projection", Renderer::CurrentCamera.GetProjection());
		GLCheckError();
		glActiveTexture(GL_TEXTURE0);
		envCubemap->Bind();
		GLCheckError();

		skybox->Draw(Renderer::CurrentCamera); // renders a 1x1x1 cube
		glDepthMask(GL_TRUE);
	}

	Framebuffer::Framebuffer(FrameBufferCreateInfo fboi, bool rbo) {
		Create(fboi, rbo);
	}

	void Framebuffer::Create(FrameBufferCreateInfo fboi, bool useRBO) {
		glGenFramebuffers(1, &bufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

		if (!useRBO) {
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboi.width, fboi.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

			glTexImage2D(
				GL_TEXTURE_2D, 0, fboi.internalDepthFormat, fboi.width, fboi.height, 0,
				fboi.format, fboi.type, NULL
			);

			glFramebufferTexture2D(GL_FRAMEBUFFER, fboi.attatchment, GL_TEXTURE_2D, texture, 0);
		}

		if (useRBO) {
			unsigned int rbo;
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
	
	void Framebuffer::SetupCubeMap(Skybox& skybox) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
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
		Shader& cubemapShader = skybox.skybox->GetShader();
		cubemapShader.Bind();
		cubemapShader.SetInt("equirectangularMap", 0);
		cubemapShader.SetMat4("projection", glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));
		glActiveTexture(GL_TEXTURE0);
		skybox.hdrTexture->Bind();
		GLCheckError();

		glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
		Bind();
		for (unsigned int i = 0; i < 6; ++i)
		{
			cubemapShader.Bind();
			cubemapShader.SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *skybox.envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		Unbind();
		GLCheckError();
	}

	Framebuffer::~Framebuffer() {
		glDeleteFramebuffers(1, &bufferID);
	}
}