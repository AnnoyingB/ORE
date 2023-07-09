//#define ORE
#include "orepch.h"
#include "Includes.h"

using namespace ORE;

int main() {
#ifdef API_VULKAN
	Window window;
	window.Create("ORE", { 1280, 720 });
	VKRenderer::Initialize(window);
	Pipeline pipeline = *VKRenderer::CreatePipeline({ "include\\ORE\\Assets\\Shaders\\Vulkan\\SimpleShaderV.spv", "include\\ORE\\Assets\\Shaders\\Vulkan\\SimpleShaderF.spv" });
	VKMesh mesh = *pipeline.CreateMesh(VKMesh::Triangle);

	while (!window.ShouldClose()) {
		window.Poll();
		VKRenderer::Record();
		VKRenderer::DrawMesh(mesh);
		VKRenderer::Present();
	}
	VKRenderer::Cleanup();
#else
	Window window;
	window.Create("ORE", { 1280, 720 });
	window.InitGlad();

	//MeshCreateInfo MCI{};
	//MCI.vertices = std::vector<ORE::Vertex>();
	//MCI.vertices.push_back({ { -0.5f, -0.5f, 0.0f }, { 1.f, 0.f, 0.f, 1.f }, { -1.f, -1.f, 0.f } });
	//MCI.vertices.push_back({ { 0.5f, -0.5f, 0.0f }, { 0.f, 1.f, 0.f, 1.f }, { 1.f, -1.f, 0.f } });
	//MCI.vertices.push_back({ { 0.0f, 0.5f, 0.0f }, { 0.f, 0.f, 1.f, 1.f }, { 0.f, 1.f, 0.f } });
	//MCI.indices = { 0, 1, 2 };
	/*MeshCreateInfo MCI{};
	MCI = OREML::Loader::Load("include\\ORE\\Assets\\Models\\SkyBox.fbx")[0];
	MCI.shaderPath = ORE::Shader::PBRShader;

	CurrentDirectionalLight = std::make_unique<DirectionalLight>();
	CurrentDirectionalLight->direction = glm::vec3(0, 0, 0);
	CurrentDirectionalLight->diffuse = glm::vec3(1, 0, 1);
	CurrentDirectionalLight->ambient = glm::vec3(1, 0.5f, 0);
	CurrentDirectionalLight->specular = glm::vec3(1, 1, 1);
	if (!CurrentDirectionalLight)
		printf("Current light is nullptr");

	PointLight* pointLight = Renderer::CreatePointLight();
	pointLight->position = glm::vec3(0.5f, 1.5f, 0);
	pointLight->diffuse = glm::vec3(1, 0, 1);
	pointLight->ambient = glm::vec3(1, 0.5f, 0);
	pointLight->specular = glm::vec3(1, 1, 1);

	GLCheckError();
	ORE::Mesh* mesh = ORE::Renderer::CreateMesh(MCI);
	GLCheckError();

	mesh->Material->albedo = glm::vec3(0, 0, 1);
	mesh->Material->metallic = 0.5;
	mesh->Material->roughness = 0.75f;
	mesh->Material->ao = 0.5f;
	mesh->Rotation = glm::vec3(45.f, 0.f, 0.f);
	mesh->Material->Apply();
	mesh->Material->UpdateLighting();

	MCI.shaderPath = ORE::Shader::SkyBoxShader;

	ORE::Mesh* skyboxMesh = ORE::Renderer::CreateMesh(MCI);

	Skybox* skybox = Renderer::CreateSkybox("pine_attic_4k.hdr", skyboxMesh);

	Renderer::CameraMovement = true;

	//Billboard* billboard = Renderer::CreateBillboard("bottom.png");
	//billboard->ModelMatrix = glm::translate(billboard->ModelMatrix, glm::vec3(0, 1, 0));
	Font* font = new Font("C:\\Windows\\Fonts\\Arial.ttf", 48);

	while (!window.ShouldClose()) {
		//GLCheckError();
		window.ClearColor(1, 0, 0, 1);

		ORE::Renderer::RenderMeshes();
		font->RenderText("Hello World!", { 25, 25 }, 24);
		//pointLight->RenderBillboard();

		window.Poll();
	}*/

	ORE::Renderer::Cleanup();
#endif
	return 0;
}