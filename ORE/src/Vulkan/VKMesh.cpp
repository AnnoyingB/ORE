#include "orepch.h"
#include "Vulkan/VKMesh.h"
#include "Vulkan/Renderer.h"

namespace ORE {
	inline std::vector<Vertex> VKMesh::Triangle = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	void VKMesh::Create(VKMeshInfo info) {
		m_Pipeline = info.pipeline;
		m_SwapchainExtent = VKRenderer::GetExtent();
		m_Index = info.index;
		m_VerticesSize = info.verticesSize;
	}

	void VKMesh::Render(VkCommandBuffer& buffer) {
		m_Pipeline->Bind(buffer, m_Index);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float>(m_SwapchainExtent->width);
		viewport.height = static_cast<float>(m_SwapchainExtent->height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(buffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = *m_SwapchainExtent;
		vkCmdSetScissor(buffer, 0, 1, &scissor);

		vkCmdDraw(buffer, m_Pipeline->GetVerticesSize(), 1, 0, 0);
	}
}