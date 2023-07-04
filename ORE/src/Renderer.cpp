#include "Renderer.h"
#include "orepch.h"

#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"

namespace ORE {
	std::vector<Mesh*> Renderer::RenderableMeshes = std::vector<Mesh*>();
	Framebuffer* Renderer::CurrentFramebuffer = nullptr;
	Skybox* Renderer::CurrentSkybox = nullptr;
	Camera Renderer::CurrentCamera = Camera();
	GLFWwindow* Renderer::CurrentWindow = nullptr;
	int Renderer::RenderCalls = 0;
	bool Renderer::CameraMovement = false;
	bool Renderer::enabledStickyKeys = false;
	glm::vec<2, double, glm::defaultp> Renderer::oldMousePos = glm::vec<2, double, glm::defaultp>(0);
	float Renderer::CameraSpeed = .05f;
	float Renderer::CameraSensitivity = .1f;
	glm::vec2 Renderer::UISize = glm::vec2(1280, 720);

	void Renderer::Init(GLFWwindow* window) {
		CurrentCamera.Position = glm::vec3(0.f, 0.f, 5.f);
		CurrentCamera.Rotation = glm::vec3(0.f, 0.f, 0.f);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		CurrentCamera.SetAspect( { (float)width, (float)height } );
		CurrentCamera.Fovy = 90;

		CurrentWindow = window;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	Mesh* Renderer::CreateMesh(MeshCreateInfo MCI) {
		Mesh* mesh = new Mesh(MCI);
		GLCheckError();
		if(MCI.shaderPath != Shader::SkyBoxShader)
			RenderableMeshes.push_back(mesh);
		GLCheckError();
		return mesh;
	}

	PointLight* Renderer::CreatePointLight() {
		PointLight* pointLight = new PointLight();
		PointLights.push_back(pointLight);
		return pointLight;
	}

	Skybox* Renderer::CreateSkybox(const std::string& hdr, Mesh* skyboxMesh) {
		Skybox* skybox = new Skybox(hdr, skyboxMesh);
		if (!CurrentSkybox)
			CurrentSkybox = skybox;
		return skybox;
	}

	Billboard* Renderer::CreateBillboard(const std::string& texPath, bool flip, bool render) {
		MeshCreateInfo mci;
		mci.vertices.push_back({ glm::vec3(SquareVertices[0], 0), { 1.f, 1.f, 1.f, 1.f }, {0.f, 0.f, 1.f}, {0, 1, 0} });
		mci.vertices.push_back({ glm::vec3(SquareVertices[1], 0), { 1.f, 1.f, 1.f, 1.f }, {0.f, 0.f, 1.f}, {1, 1, 0} });
		mci.vertices.push_back({ glm::vec3(SquareVertices[2], 0), { 1.f, 1.f, 1.f, 1.f }, {0.f, 0.f, 1.f}, {1, 0, 0} });
		mci.vertices.push_back({ glm::vec3(SquareVertices[3], 0), { 1.f, 1.f, 1.f, 1.f }, {0.f, 0.f, 1.f}, {0, 0, 0} });
		mci.indices = { 0, 1, 2, 2, 3, 0 };
		mci.shaderPath = Shader::BillboardShader;

		Billboard* billboard = new Billboard(texPath, flip, mci);
		if(render)
			RenderableMeshes.push_back(billboard);
		return billboard;
	}

	void Renderer::Cleanup() {
		for (Mesh* mesh : RenderableMeshes)
			delete mesh;
		if (CurrentFramebuffer)
			delete CurrentFramebuffer;

		for (PointLight* light : PointLights)
			delete light;
	}

	void Renderer::RenderMeshes() {
		RenderCalls = 0;

		if (CameraMovement) {
			glm::vec<2, double, glm::defaultp> mousePos;
			glfwGetCursorPos(CurrentWindow, &mousePos.x, &mousePos.y);

			if (!enabledStickyKeys) {
				glfwSetInputMode(CurrentWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
				glfwSetInputMode(CurrentWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				enabledStickyKeys = true;
			}

			CurrentCamera.Rotation += glm::vec3(mousePos.x - oldMousePos.x, oldMousePos.y - mousePos.y, 0) * CameraSensitivity;

			glm::vec3 CameraForward = CurrentCamera.GetForwardVec();
			if (glfwGetKey(CurrentWindow, GLFW_KEY_W))
				CurrentCamera.Position += CameraForward * CameraSpeed;
			if (glfwGetKey(CurrentWindow, GLFW_KEY_S))
				CurrentCamera.Position -= CameraForward * CameraSpeed;
			if (glfwGetKey(CurrentWindow, GLFW_KEY_A))
				CurrentCamera.Position -= glm::normalize(glm::cross(CameraForward, glm::vec3(0, 1, 0))) * CameraSpeed;
			if (glfwGetKey(CurrentWindow, GLFW_KEY_D))
				CurrentCamera.Position += glm::normalize(glm::cross(CameraForward, glm::vec3(0, 1, 0))) * CameraSpeed;
			if (glfwGetKey(CurrentWindow, GLFW_KEY_Q))
				CurrentCamera.Position.y -= CameraSpeed;
			if (glfwGetKey(CurrentWindow, GLFW_KEY_E))
				CurrentCamera.Position.y += CameraSpeed;
			if (glfwGetKey(CurrentWindow, GLFW_KEY_ESCAPE))
				glfwSetWindowShouldClose(CurrentWindow, GLFW_TRUE);

			oldMousePos = mousePos;
		}
		else if (!CameraMovement && enabledStickyKeys) {
			glfwSetInputMode(CurrentWindow, GLFW_STICKY_KEYS, GLFW_FALSE);
			glfwSetInputMode(CurrentWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			enabledStickyKeys = false;
		}

		if (CurrentFramebuffer)
			CurrentFramebuffer->Bind();
		if (CurrentSkybox)
			CurrentSkybox->irradianceMap->Bind();
		for (Mesh* mesh : RenderableMeshes) {
			mesh->GetConstShader().Bind();
			mesh->GetConstShader().SetVec3("camPos", CurrentCamera.Position);
			mesh->Apply(CurrentCamera);
			mesh->Draw(CurrentCamera);
			RenderCalls++;
		}
		if (CurrentFramebuffer)
			CurrentFramebuffer->Unbind();

		if (CurrentSkybox) {
			CurrentSkybox->DrawSkybox();
			RenderCalls++;
		}
	}
}
