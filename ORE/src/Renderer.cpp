#include "Renderer.h"
#include "orepch.h"

#include "Buffers/Framebuffer.h"

namespace ORE {
	std::vector<Mesh*> Renderer::RenderableMeshes = std::vector<Mesh*>();
	Framebuffer* Renderer::CurrentFramebuffer = nullptr;
	Camera Renderer::CurrentCamera = Camera();
	int Renderer::RenderCalls = 0;

	void Renderer::Init(GLFWwindow* window) {
		CurrentCamera.Position = glm::vec3(0.5f, 0.f, 1.f);
		CurrentCamera.Rotation = glm::vec3(45.f, 0.f, 0.f);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		CurrentCamera.SetAspect( { (float)width, (float)height } );
		CurrentCamera.Fovy = 90;
	}

	Mesh* Renderer::CreateMesh(MeshCreateInfo MCI) {
		Mesh* mesh = new Mesh(MCI);
		GLCheckError();
		RenderableMeshes.push_back(mesh);
		GLCheckError();
		return mesh;
	}

	void Renderer::Cleanup() {
		for (Mesh* mesh : RenderableMeshes)
			delete mesh;
		if (CurrentFramebuffer)
			delete CurrentFramebuffer;
	}

	void Renderer::RenderMeshes() {
		RenderCalls = 0;
		if (CurrentFramebuffer)
			CurrentFramebuffer->Bind();
		for (Mesh* mesh : RenderableMeshes) {
			mesh->GetConstShader().Bind();
			mesh->GetConstShader().SetVec3("viewPos", CurrentCamera.Position);
			mesh->Draw(CurrentCamera);
			RenderCalls++;
		}
		if (CurrentFramebuffer)
			CurrentFramebuffer->Unbind();
	}
}
