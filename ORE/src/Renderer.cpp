#include "Renderer.h"
#include "orepch.h"

namespace ORE {
	std::vector<Mesh*> Renderer::RenderableMeshes = std::vector<Mesh*>();
	std::vector<Framebuffer*> Renderer::Framebuffers = std::vector<Framebuffer*>();
	Camera Renderer::CurrentCamera = Camera();

	void Renderer::Init(GLFWwindow* window) {
		CurrentCamera.Position = glm::vec3(0.5f, 0.f, 1.f);
		CurrentCamera.Rotation = glm::vec3(45.f, 0.f, 0.f);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		CurrentCamera.SetAspect((float)width, (float)height);
		CurrentCamera.Fovy = 90;
	}

	Mesh* Renderer::CreateMesh(MeshCreateInfo MCI) {
		Mesh* mesh = new Mesh(MCI);
		RenderableMeshes.push_back(mesh);
		return mesh;
	}

	void Renderer::Cleanup() {
		for (Mesh* mesh : RenderableMeshes)
			delete mesh;
		for (Framebuffer* fbo : Framebuffers)
			delete fbo;
	}

	void Renderer::RenderMeshes() {
		for (Mesh* mesh : RenderableMeshes)
			mesh->Draw(CurrentCamera);
	}
}
