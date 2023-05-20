//#define ORE
#define STB_IMAGE_IMPLEMENTATION
#include "orepch.h"
#include "Window.h"
#include "Renderer.h"
//#include "OREML/Loader.h"

using namespace ORE;

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	Window window;
	window.Create("ORE", { 1280, 720 });
	window.InitGlad();
	window.SetSizeCallback(WindowSizeCallback);

	Renderer::Init(window);

	MeshCreateInfo MCI {};
	MCI.vertices = std::vector<ORE::Vertex>();
	MCI.vertices.push_back({ { -0.5f, -0.5f, 0.0f }, { 1.f, 0.f, 0.f, 1.f }, { -1.f, -1.f, 0.f } });
	MCI.vertices.push_back({ { 0.5f, -0.5f, 0.0f }, { 0.f, 1.f, 0.f, 1.f }, { 1.f, -1.f, 0.f } });
	MCI.vertices.push_back({ { 0.0f, 0.5f, 0.0f }, { 0.f, 0.f, 1.f, 1.f }, { 0.f, 1.f, 0.f } });
	MCI.indices = { 0, 1, 2 };
	//ORE::MeshCreateInfo MCI{};
	//MCI = OREML::Loader::Load("include\\ORE\\Models\\untitled.fbx")[0];
	MCI.shaderPath = ORE::Shader::OREShaders + "SimpleShader";

	CurrentDirectionalLight = std::make_unique<DirectionalLight>();
	CurrentDirectionalLight->direction = glm::vec3(0, 0, 45);
	CurrentDirectionalLight->diffuse = glm::vec3(1, 0, 0);
	CurrentDirectionalLight->ambient = glm::vec3(1, 0.5f, 0);
	CurrentDirectionalLight->specular = glm::vec3(1, 1, 1);
	if (!CurrentDirectionalLight)
		printf("Current light is nullptr");

	GLCheckError();
	ORE::Mesh* mesh = ORE::Renderer::CreateMesh(MCI);
	GLCheckError();

	mesh->Material->diffuse = glm::vec3(1, 0, 0);
	mesh->Material->ambient = glm::vec3(1, 0, 1);
	mesh->Material->specular = glm::vec3(1, 0, 1);
	mesh->Material->shininess = 0.5f;
	mesh->ModelMatrix = glm::rotate(mesh->ModelMatrix, 45.f, glm::vec3(0, 0, 1));
	mesh->GetConstShader().Bind();
	mesh->Material->Apply();
	mesh->Material->UpdateLighting();

	while (!window.ShouldClose()) {
		//GLCheckError();
		window.ClearColor(1, 0, 0, 1);

		ORE::Renderer::RenderMeshes();

		window.Poll();
	}

	ORE::Renderer::Cleanup();
	return 0;
}