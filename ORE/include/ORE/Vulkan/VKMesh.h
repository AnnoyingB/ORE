#pragma once
#include "Vulkan/GraphicsPipeline.h"
#include "glm/glm.hpp"

#include <array>

namespace ORE {
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription description{};
			description.binding = 0;
			description.stride = sizeof(Vertex);
			description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return description;
		}

		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);
			return attributeDescriptions;
		}
	};

	struct VKMeshInfo {
		GraphicsPipeline* pipeline;
		size_t verticesSize;
		uint32_t index;
	};

	class VKMesh {
	private:
		GraphicsPipeline* m_Pipeline;
		VkExtent2D* m_SwapchainExtent;
		size_t m_VerticesSize;
		uint32_t m_Index;
	public:
		static std::vector<Vertex> Triangle;

		void Create(VKMeshInfo);
		void Render(VkCommandBuffer&);
	};
}