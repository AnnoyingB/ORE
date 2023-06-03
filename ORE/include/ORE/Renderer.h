#pragma once
#include "Buffers/Framebuffer.h"
#include "Objects/Billboard.h"
#include "Mesh.h"
#include "GLFW/glfw3.h"

namespace ORE {
	class Renderer {
	private:
		static bool enabledStickyKeys;
		static glm::vec<2, double, glm::defaultp> oldMousePos;
	public:
		static std::vector<Mesh*> RenderableMeshes;
		static Framebuffer* CurrentFramebuffer;
		static Skybox* CurrentSkybox;
		static Camera CurrentCamera;
		static int RenderCalls;
		static GLFWwindow* CurrentWindow;
		static bool CameraMovement; // WASDEQ
		static float CameraSpeed;
		static float CameraSensitivity;

		enum RT {
			None = 0,
			RT3D = 1,
			RT2D = 2,
		} static RenderType;

		static void Init(GLFWwindow* window);
		static Mesh* CreateMesh(MeshCreateInfo MCI);
		static PointLight* CreatePointLight();
		static Skybox* CreateSkybox(const std::string& hdr, Mesh* skyboxMesh);
		static Billboard* CreateBillboard(const std::string& texPath, bool flip = false,  bool render = true);
		static void Cleanup();
		static void RenderMeshes();
	};
}
