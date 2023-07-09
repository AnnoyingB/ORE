#include "orepch.h"
#include "Vulkan/GraphicsPipeline.h"
#include <fstream>

#include "Vulkan/Renderer.h"
#include "Vulkan/VKMesh.h"
#include "vma/vk_mem_alloc.h"

namespace ORE {
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_VIEWPORT
	};

	GraphicsPipeline::GraphicsPipeline(VkRenderPass renderpass) {
		m_Device = VKRenderer::GetLogicalDevice();
		m_ShaderStages = {};
		m_RenderPass = renderpass;
	}

	std::vector<char> GraphicsPipeline::ReadFile(std::string_view filePath) {
		std::vector<char> buffer;
		std::ifstream file(filePath.data(), std::ios::ate | std::ios::binary);
		if (!file.is_open())
			return {};

		size_t fileSize = (size_t)file.tellg();
		buffer.resize(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	void GraphicsPipeline::CreateShaderModule(std::vector<char> data, VkShaderModule* module) {
		if (data.empty()) {
			std::cout << "Failed to load shader code!" << std::endl;
			abort();
			return;
		}

		VkShaderModuleCreateInfo shaderModuleInfo{};
		shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleInfo.codeSize = data.size();
		shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());

		if (vkCreateShaderModule(*m_Device, &shaderModuleInfo, nullptr, module) != VK_SUCCESS) {
			std::cout << "Failed to create shader module!" << std::endl;
			return;
		}
	}

	GraphicsPipeline::~GraphicsPipeline() {
		if (!m_VertexBuffers.empty())
			for(AllocatedBuffer buffer : m_VertexBuffers)
				vmaDestroyBuffer(*m_Allocator, buffer.buffer, buffer.allocation);
		if (m_Pipeline) vkDestroyPipeline(*m_Device, m_Pipeline, nullptr);
		if (m_PipelineLayout) vkDestroyPipelineLayout(*m_Device, m_PipelineLayout, nullptr);
		if(m_VertexModule) vkDestroyShaderModule(*m_Device, m_VertexModule, nullptr);
		if(m_FragmentModule) vkDestroyShaderModule(*m_Device, m_FragmentModule, nullptr);
		if(m_GeometryModule) vkDestroyShaderModule(*m_Device, m_GeometryModule, nullptr);
	}

	void GraphicsPipeline::Create(GraphicsPipelineInfo info)
	{
		const bool const hasGeometry = !info.geometry.empty();

		std::vector<char> vertBuff, fragBuff, geomBuff;
#ifndef SHADERS_COMPILED
		// Compile Shaders
		{
			if (!Initialized) {
				glslang::InitializeProcess();
				Initialized = true;
			}
			// Vertex
			{
				auto glsl = ReadFile(info.vert);

				glslang::TShader vertex(EShLangVertex);
				const char* shaderStrings[1] = { glsl.data() };
				vertex.setStrings(shaderStrings, 1);

				const EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
				if (vertex.parse(nullptr, 100, false, messages)) {
					std::cout << "Failed to compile glsl to spv!" << std::endl;
					return;
				}
			}
		}
#else
		{
			vertBuff = ReadFile(info.vertex);
			fragBuff = ReadFile(info.fragment);
			if(hasGeometry)
				geomBuff = ReadFile(info.geometry);
		}
#endif

		// Create vertex module
		CreateShaderModule(vertBuff, &m_VertexModule);
		// Create fragment module
		CreateShaderModule(fragBuff, &m_FragmentModule);
		// Create geometry module
		if(hasGeometry)
			CreateShaderModule(geomBuff, &m_GeometryModule);
		m_Allocator = VKRenderer::GetAllocator();
		{
			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = m_VertexModule;
			vertShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = m_FragmentModule;
			fragShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo geomShaderStageInfo{};
			if (hasGeometry) {
				geomShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				geomShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				geomShaderStageInfo.module = m_GeometryModule;
				geomShaderStageInfo.pName = "main";
			}
			size_t shaders = 2;
			if (hasGeometry) shaders++;
			m_ShaderStages = (VkPipelineShaderStageCreateInfo*)_malloca(shaders * sizeof(VkPipelineShaderStageCreateInfo));
			m_ShaderStages[0] = vertShaderStageInfo;
			m_ShaderStages[1] = fragShaderStageInfo;
			if (hasGeometry) m_ShaderStages[2] = geomShaderStageInfo;

			std::cout << "Created shader modules\n";
		}

		// Pipeline layout
		{
			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

			if (vkCreatePipelineLayout(*m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
				std::cout << "Failed to create pipeline layout!" << std::endl;
				abort();
				return;
			}

			std::cout << "Created pipeline layout!\n";
		}

		// Create pipeline
		{
			VkVertexInputBindingDescription bindingDesc = Vertex::GetBindingDescription();
			std::array<VkVertexInputAttributeDescription, 2> attributeDesc = Vertex::GetAttributeDescriptions();

			VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
			dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicStateInfo.pDynamicStates = dynamicStates.data();

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDesc.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDesc.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
			inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport{};
			viewport.x = 0;
			viewport.y = 0;
			viewport.width = (float)info.swapchainExtent.width;
			viewport.height = (float)info.swapchainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor{};
			scissor.extent = info.swapchainExtent;
			scissor.offset = { 0, 0 };

			VkPipelineViewportStateCreateInfo viewportStateInfo{};
			viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportStateInfo.viewportCount = 1;
			viewportStateInfo.pViewports = &viewport;
			viewportStateInfo.scissorCount = 1;
			viewportStateInfo.pScissors = &scissor;

			VkPipelineRasterizationStateCreateInfo rasterizerStateInfo{};
			rasterizerStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizerStateInfo.depthClampEnable = VK_FALSE;
			rasterizerStateInfo.rasterizerDiscardEnable = VK_FALSE;
			rasterizerStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizerStateInfo.lineWidth = 1.0f;
			rasterizerStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizerStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizerStateInfo.depthBiasEnable = VK_FALSE;

			VkPipelineMultisampleStateCreateInfo multisampleStateInfo{};
			multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleStateInfo.sampleShadingEnable = VK_FALSE;
			multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

			// TODO: Depth stencil

			VkPipelineColorBlendAttachmentState blendingAttachmentState{};
			blendingAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			blendingAttachmentState.blendEnable = VK_FALSE;

			VkPipelineColorBlendStateCreateInfo blendingStateInfo{};
			blendingStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			blendingStateInfo.logicOpEnable = VK_FALSE;
			blendingStateInfo.attachmentCount = 1;
			blendingStateInfo.pAttachments = &blendingAttachmentState;

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

			pipelineInfo.renderPass = m_RenderPass;
			pipelineInfo.stageCount = 2 + hasGeometry;
			pipelineInfo.pStages = m_ShaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pColorBlendState = &blendingStateInfo;
			pipelineInfo.pRasterizationState = &rasterizerStateInfo;
			pipelineInfo.pDynamicState = &dynamicStateInfo;
			pipelineInfo.pViewportState = &viewportStateInfo;
			pipelineInfo.pMultisampleState = &multisampleStateInfo;
			pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

			pipelineInfo.layout = m_PipelineLayout;
			pipelineInfo.renderPass = m_RenderPass;
			pipelineInfo.subpass = 0;
			
			if (vkCreateGraphicsPipelines(*m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS) {
				std::cout << "Failed to create graphics pipeline!" << std::endl;
				abort();
				return;
			}

			_freea(m_ShaderStages); // don't need it anymore

			std::cout << "Created pipeline!\n";
		}
	}

	void GraphicsPipeline::Bind(VkCommandBuffer cmdBuffer, uint32_t index) {
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		//std::vector<VkBuffer> buffers(m_VertexBuffers.size());
		//for (AllocatedBuffer buffer : m_VertexBuffers)
		//	buffers.push_back(buffer.buffer);
		VkBuffer buffers[] = { m_VertexBuffers[index].buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, buffers, offsets);
	}

	VKMesh* GraphicsPipeline::CreateMesh(std::vector<Vertex> vertices) {
		uint32_t index = m_VertexBuffers.size();
		m_VertexBuffers.push_back(AllocatedBuffer());

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = vertices.size() * sizeof(Vertex);
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		vmaCreateBuffer(*m_Allocator, &bufferInfo, &allocInfo, &m_VertexBuffers[index].buffer, &m_VertexBuffers[index].allocation, nullptr);
		void* data = nullptr;
		vmaMapMemory(*m_Allocator, m_VertexBuffers[index].allocation, &data);
		memcpy(data, vertices.data(), (size_t)bufferInfo.size);
		vmaUnmapMemory(*m_Allocator, m_VertexBuffers[index].allocation);
		m_VerticesSize = vertices.size();

		VKMesh* mesh = new VKMesh();
		mesh->Create({this, vertices.size(), index});
		return mesh;
	}

	uint32_t GraphicsPipeline::GetVerticesSize() {
		return m_VerticesSize;
	}
}