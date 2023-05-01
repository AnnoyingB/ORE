#pragma once
#include "Mesh.h"
#include "GLFW/glfw3.h"

namespace ORE {
	class Framebuffer;

	class Renderer {
	public:
		static std::vector<Mesh*> RenderableMeshes;
		static std::vector<Framebuffer*> Framebuffers;
		static Camera CurrentCamera;

		enum RT {
			None = 0,
			RT3D = 1,
			RT2D = 2,
		} static RenderType;

		static void Init(GLFWwindow* window);
		static Mesh* CreateMesh(MeshCreateInfo MCI);
		static void Cleanup();
		static void RenderMeshes();
	};
}
