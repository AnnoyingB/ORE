//#define ORE
#define STB_IMAGE_IMPLEMENTATION
#include "orepch.h"
#include "Window.h"
#include "Renderer.h"

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	ORE::Window window;
	window.Create("ORE", { 1280, 720 });
	window.InitGlad();
	window.SetSizeCallback(WindowSizeCallback);

	ORE::Renderer::Init(window);

	ORE::MeshCreateInfo MCI {};
	MCI.vertices = std::vector<ORE::Vertex>();
	MCI.vertices.push_back({ { -0.5f, -0.5f, 0.0f }, { 1.f, 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f } });
	MCI.vertices.push_back({ { 0.5f, -0.5f, 0.0f }, { 0.f, 1.f, 0.f, 1.f }, { 0.f, 0.f, 0.f } });
	MCI.vertices.push_back({ { 0.0f, 0.5f, 0.0f }, { 0.f, 0.f, 1.f, 1.f }, { 0.f, 0.f, 0.f } });
	MCI.indices = { 0, 1, 2 };
	MCI.shaderPath = ORE::Shader::OREShaders + "SimpleShader";

	ORE::Mesh* mesh = ORE::Renderer::CreateMesh(MCI);

	while (!window.ShouldClose()) {
		window.ClearColor(1, 0, 0, 1);

		ORE::Renderer::RenderMeshes();

		window.Poll();
	}

	ORE::Renderer::Cleanup();
	return 0;
}