#pragma once
#ifndef SHADERS_COMPILED
#include "glslang/SPIRV/GlslangToSpv.h"
#else
#include <string>
#include <vector>
#endif

#include "volk/volk.h"

namespace ORE {
	struct GraphicsPipelineInfo {
		VkExtent2D swapchainExtent = {0, 0};
		std::string_view vertex, fragment, geometry = "";

		GraphicsPipelineInfo(std::string_view v, std::string_view f, std::string_view g = "")
			: vertex(v), fragment(f), geometry(g) { }
	};

	class GraphicsPipeline {
	private:
		VkShaderModule m_VertexModule = VK_NULL_HANDLE, m_FragmentModule = VK_NULL_HANDLE, m_GeometryModule = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkDevice* m_Device;

		VkPipelineShaderStageCreateInfo* m_ShaderStages;

		std::vector<char> ReadFile(std::string_view);
		void CreateShaderModule(std::vector<char>, VkShaderModule*);
	public:
		static inline bool Initialized = false;

		GraphicsPipeline(VkRenderPass);
		virtual ~GraphicsPipeline();

		void Create(GraphicsPipelineInfo);

		operator VkPipeline() {
			return m_Pipeline;
		}
	};
}