#pragma once
//#define VK_USE_PLATFORM_WIN32_KHR
#include "volk/volk.h"
#include <optional>
#include <string>

#define NOMINMAX
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Vulkan/GraphicsPipeline.h"

#define VK_CHECK(x)                               \
if(x != VK_SUCCESS) {                             \
	std::cout << "Failed to run " << #x << std::endl; \
	abort();                                      \
	return;                                       \
}                                                 \

#ifndef FRAMES_IN_FLIGHT
#define FRAMES_IN_FLIGHT 2
#endif

namespace ORE {
	typedef GraphicsPipeline& Pipeline;

	// Frames in flight: Use #define FRAMES_IN_FLIGHT <number of frames>
	class VKRenderer {
	private:
		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool Complete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
		};

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		static GLFWwindow* m_Window;

		// Object creation objects
		static VkInstance m_Instance;
		static VkPhysicalDevice m_PhysicalDevice;
		static VkDevice m_Device;
		static QueueFamilyIndices m_Indices;
		static VkQueue m_Queue;

		// Swapchain/pipeline objects
		static VkSurfaceKHR m_Surface;
		static VkSwapchainKHR m_Swapchain;
		static VkFormat m_SwapchainFormat;
		static VkExtent2D m_SwapchainExtent;
		static VkRenderPass m_RenderPass;
		static std::vector<VkImage> m_SwapchainImages;
		static std::vector<VkImageView> m_SwapchainImageViews;
		static std::vector<VkFramebuffer> m_SwapchainFramebuffers;

		// Presentation/rendering objects
		static VkCommandPool m_CommandPool;
		static std::vector<VkCommandBuffer> m_CommandBuffers;
		static std::vector<VkSemaphore> m_ImagesAvailable;
		static std::vector<VkSemaphore> m_FramesFinished;
		static std::vector<VkFence> m_InFlights;
		static uint32_t imageIndex;

		static std::vector<GraphicsPipeline*> m_Pipelines;
		static uint32_t m_CurrentFrame;

		static bool CheckDeviceExtensions(const VkPhysicalDevice&);
		static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice&);
		static VkPhysicalDevice TestDevicesSuitability(const std::vector<VkPhysicalDevice>&);

		static SwapChainSupportDetails QuerySwapchainSupport(const VkPhysicalDevice&);
		static VkSurfaceFormatKHR ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>&);
		static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>&);
		static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR&, GLFWwindow*);

		static void RecreateSwapchain();
	public:
		static void Initialize(GLFWwindow* window, const std::string& name = "ORE", bool validation = true);
		static void Cleanup();

		static void Record(VkClearValue clear = { {{1.f, 0.f, 1.f, 1.f}} });
		static void Draw(Pipeline);
		static void Present();

		// Store the return value as Pipeline
		static GraphicsPipeline* CreatePipeline(GraphicsPipelineInfo);

		static VkDevice* GetLogicalDevice();
		static VkExtent2D* GetExtent();
		static VkRenderPass* GetRenderPass();
	};
}
