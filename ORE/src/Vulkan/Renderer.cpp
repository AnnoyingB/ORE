#include "orepch.h"
#include "Vulkan/Renderer.h"
#include <algorithm>
#include <limits>
#include <set>
#include <fstream>

#ifndef NO_NSIGHT
#include "nsight/GFSDK_Aftermath_GpuCrashDump.h"
#define ENABLE_DUMP GFSDK_Aftermath_EnableGpuCrashDumps(GFSDK_Aftermath_Version_API, GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_Vulkan, GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_None, CrashDumpGPU, CrashDumpShader, CrashDumpDescription, CrashDumpResolve, nullptr);
#define DISABLE_DUMP GFSDK_Aftermath_DisableGpuCrashDumps()
#else
#define ENABLE_DUMP
#define DISABLE_DUMP
#endif

namespace ORE {
	inline GLFWwindow* VKRenderer::m_Window = nullptr;
	
	inline VkInstance VKRenderer::m_Instance = {};
	inline VkPhysicalDevice VKRenderer::m_PhysicalDevice = {};
	inline VkDevice VKRenderer::m_Device = {};
	inline VKRenderer::QueueFamilyIndices VKRenderer::m_Indices = {};
	inline VkQueue VKRenderer::m_Queue = {};

	inline VkSurfaceKHR VKRenderer::m_Surface = {};
	inline VkSwapchainKHR VKRenderer::m_Swapchain = {};
	inline VkFormat VKRenderer::m_SwapchainFormat = {};
	inline VkExtent2D VKRenderer::m_SwapchainExtent = {};
	inline VkRenderPass VKRenderer::m_RenderPass = {};
	inline std::vector<VkImage> VKRenderer::m_SwapchainImages = {};
	inline std::vector<VkImageView> VKRenderer::m_SwapchainImageViews = {};
	inline std::vector<VkFramebuffer> VKRenderer::m_SwapchainFramebuffers = {};

	inline VkCommandPool VKRenderer::m_CommandPool = {};
	inline std::vector<VkCommandBuffer> VKRenderer::m_CommandBuffers = {};
	inline std::vector<VkSemaphore> VKRenderer::m_ImagesAvailable = {};
	inline std::vector<VkSemaphore> VKRenderer::m_FramesFinished = {};
	inline std::vector<VkFence> VKRenderer::m_InFlights = {};
	inline uint32_t VKRenderer::imageIndex = 0;

	inline std::vector<GraphicsPipeline*> VKRenderer::m_Pipelines = {};
	inline uint32_t VKRenderer::m_CurrentFrame = {};
	inline bool VKRenderer::m_ResizedThisFrame = false;
	inline VmaAllocator VKRenderer::m_Allocator = nullptr;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> extensions = {
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	enum class ScoringSystem
	{
		DiscreteGPU = 100,
		NVIDIA = 10,
		GeometryShader = 25,
		LogicOP = 100,
	};

	bool VKRenderer::CheckDeviceExtensions(const VkPhysicalDevice& device) {
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> props(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, props.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : props) {
			requiredExtensions.erase(extension.extensionName);
		}

		return !requiredExtensions.empty();
	}

	VKRenderer::QueueFamilyIndices VKRenderer::FindQueueFamilies(const VkPhysicalDevice& device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.Complete())
				break;

			i++;
		}

		return indices;
	}

	VkPhysicalDevice VKRenderer::TestDevicesSuitability(const std::vector<VkPhysicalDevice>& devices) {
		std::vector<int> scores(devices.size());
		for (int i = 0; i < devices.size(); i++) {
			QueueFamilyIndices indices = FindQueueFamilies(devices[i]);
			if (!indices.Complete() || CheckDeviceExtensions(devices[i]))
				break;

			bool swapchainSupport = false;
			{
				SwapChainSupportDetails details = QuerySwapchainSupport(devices[i]);
				swapchainSupport = !details.formats.empty() && !details.presentModes.empty();
			}
			if (!swapchainSupport)
				break;

			VkPhysicalDeviceProperties props{};
			VkPhysicalDeviceFeatures features{};
			vkGetPhysicalDeviceProperties(devices[i], &props);
			vkGetPhysicalDeviceFeatures(devices[i], &features);

			if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				scores[i] += (int)ScoringSystem::DiscreteGPU;
			if (props.vendorID == VK_DRIVER_ID_NVIDIA_PROPRIETARY)
				scores[i] += (int)ScoringSystem::NVIDIA;
			if (features.geometryShader)
				scores[i] += (int)ScoringSystem::GeometryShader;
			if (features.logicOp)
				scores[i] += (int)ScoringSystem::LogicOP;
		}
		int highest = 0;
		int highestIdx = 0;
		for (int i = 0; i < scores.size(); i++) {
			if (scores[i] > highest) {
				highest = scores[i];
				highestIdx = i;
			}
		}

		if (highest == 0) {
			throw std::runtime_error("No suitable GPU found!");
		}

		return devices[highestIdx];
	}

	VKRenderer::SwapChainSupportDetails VKRenderer::QuerySwapchainSupport(const VkPhysicalDevice& device) {
		VKRenderer::SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
		}

		uint32_t presentCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentCount, nullptr);
		if (presentCount != 0) {
			details.presentModes.resize(presentCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentCount, details.presentModes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR VKRenderer::ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
		for (const VkSurfaceFormatKHR& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return format;
		}

		return formats[0];
	}

	VkPresentModeKHR VKRenderer::ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes) {
		for (const VkPresentModeKHR& mode : modes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) // triple buffering
				return mode;
		}

		return VK_PRESENT_MODE_FIFO_KHR; // only guaranteed one
	}

	VkExtent2D VKRenderer::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D extent{
			std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
		return extent;
	}

	void VKRenderer::RecreateSwapchain() {
		vkDeviceWaitIdle(m_Device);

		for (VkFramebuffer framebuffer : m_SwapchainFramebuffers)
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		for (VkImageView view : m_SwapchainImageViews)
			vkDestroyImageView(m_Device, view, nullptr);
		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);

		// Create swapchain
		{
			SwapChainSupportDetails details = QuerySwapchainSupport(m_PhysicalDevice);

			VkSurfaceFormatKHR format = ChooseSwapchainFormat(details.formats);
			VkPresentModeKHR presentMode = ChoosePresentMode(details.presentModes);
			VkExtent2D extent = ChooseExtent(details.capabilities, m_Window);

			uint32_t imageCount = details.capabilities.minImageCount + 1;
			if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
				imageCount = details.capabilities.maxImageCount;

			VkSwapchainCreateInfoKHR swapchainInfo{};
			swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainInfo.surface = m_Surface;

			swapchainInfo.minImageCount = imageCount;
			swapchainInfo.imageFormat = format.format;
			swapchainInfo.imageColorSpace = format.colorSpace;
			swapchainInfo.imageExtent = extent;
			swapchainInfo.presentMode = presentMode;
			swapchainInfo.imageArrayLayers = 1;
			swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchainInfo.oldSwapchain = m_Swapchain;

			m_SwapchainFormat = format.format;
			m_SwapchainExtent = extent;

			if (m_Indices.graphicsFamily != m_Indices.presentFamily) {
				uint32_t familyIndices[] = { m_Indices.graphicsFamily.value(), m_Indices.presentFamily.value() };

				swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapchainInfo.queueFamilyIndexCount = 2;
				swapchainInfo.pQueueFamilyIndices = familyIndices;
			}
			else
				swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

			swapchainInfo.preTransform = details.capabilities.currentTransform;
			swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchainInfo.clipped = VK_TRUE;
			swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

			VK_CHECK(vkCreateSwapchainKHR(m_Device, &swapchainInfo, nullptr, &m_Swapchain));

			std::cout << "Created KHR Swapchain!\n";
		}

		// Get images and create image views
		{
			uint32_t swapchainImageCount;
			vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);
			m_SwapchainImages.resize(swapchainImageCount);
			vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, m_SwapchainImages.data());

			m_SwapchainImageViews.resize(swapchainImageCount);
			for (int i = 0; i < swapchainImageCount; i++) {
				VkImageViewCreateInfo imageViewInfo{};
				imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewInfo.image = m_SwapchainImages[i];

				imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewInfo.format = m_SwapchainFormat;

				imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

				imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageViewInfo.subresourceRange.baseMipLevel = 0;
				imageViewInfo.subresourceRange.levelCount = 1;
				imageViewInfo.subresourceRange.baseArrayLayer = 0;
				imageViewInfo.subresourceRange.layerCount = 1;

				VK_CHECK(vkCreateImageView(m_Device, &imageViewInfo, nullptr, &m_SwapchainImageViews[i]));
			}

			std::cout << "Created image views\n";
		}
		
		// Create swapchain framebuffers
		{
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.width = m_SwapchainExtent.width;
			framebufferInfo.height = m_SwapchainExtent.height;
			framebufferInfo.layers = 1;

			m_SwapchainFramebuffers.resize(m_SwapchainImageViews.size());
			for (int i = 0; i < m_SwapchainFramebuffers.size(); i++) {
				VkImageView attachments[] = {
					m_SwapchainImageViews[i]
				};

				framebufferInfo.attachmentCount = static_cast<uint32_t>(sizeof(attachments) / sizeof(attachments[0]));
				framebufferInfo.pAttachments = attachments;

				VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]));
			}
		}
	}

	void CrashDumpGPU(const void* gpuCrashDump, uint32_t dumpSize, void* userData) {
		std::cout << "gpu\n";
		std::ofstream file("DumpGPU.gpudbg", std::ios::binary);
		if (!file.is_open()) {
			abort();
			return;
		}
		file.write(reinterpret_cast<const char*>(gpuCrashDump), dumpSize);
		file.close();
	}

	void CrashDumpShader(const void* gpuCrashDump, uint32_t dumpSize, void* userData) {
		std::cout << "shader\n";
		std::ofstream file("DumpShader.nvdbg", std::ios::binary);
		if (!file.is_open()) {
			abort();
			return;
		}
		file.write(reinterpret_cast<const char*>(gpuCrashDump), dumpSize);
		file.close();
	}


	void CrashDumpDescription(PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription randomPtr, void* userData) {
		std::cout << "idk\n";
	}


	void CrashDumpResolve(const void* marker, void* userData, void** resolvedMarker, uint32_t* markerSize) {
		std::cout << "marker\n";
		std::ofstream file("DumpMarker.nvdbg", std::ios::binary);
		if (!file.is_open()) {
			abort();
			return;
		}
		file.write(reinterpret_cast<const char*>(marker), *markerSize);
		file.close();
	}
	
	void VKRenderer::Initialize(GLFWwindow* window, const std::string& name, bool validation) {
		{
			// Initialize volk
			if (volkInitialize() != VK_SUCCESS) {
				std::cout << "Failed to initialize volk!" << std::endl;
				abort();
				return;
			}

			// Create instance
			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pEngineName = "ORE";
			appInfo.pApplicationName = name.c_str();
			appInfo.apiVersion = VK_VERSION_1_3;
			appInfo.engineVersion = 1;

			VkInstanceCreateInfo instanceInfo{};
			instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceInfo.pApplicationInfo = &appInfo;
			instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			instanceInfo.ppEnabledExtensionNames = extensions.data();
			if (validation) {
				instanceInfo.ppEnabledLayerNames = validationLayers.data();
				instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			}

			VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &m_Instance));

			volkLoadInstance(m_Instance);

			std::cout << "Created Vulkan Instance\n";

			// load swapchain function
			//vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(m_Instance, "vkCreateWin32SurfaceKHR");
		}
		ENABLE_DUMP

		m_Window = window;

		// Create surface
		{
			VkWin32SurfaceCreateInfoKHR surfaceInfo{};
			surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surfaceInfo.hwnd = glfwGetWin32Window(window);
			surfaceInfo.hinstance = GetModuleHandle(nullptr);

			VK_CHECK(vkCreateWin32SurfaceKHR(m_Instance, &surfaceInfo, nullptr, &m_Surface));

			std::cout << "Created surface KHR\n";
		}

		{
			// Find physical device
			uint32_t deviceCount;
			vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

			if (deviceCount == 0) {
				std::cout << "No devices support vulkan" << std::endl;
				abort();
				return;
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

			if (devices.size() > 0)
				m_PhysicalDevice = TestDevicesSuitability(devices);
			else
				m_PhysicalDevice = devices[0];

			std::cout << "Found Physical Device\n";
		}

		{
			// Create virtual device & queue
			m_Indices = FindQueueFamilies(m_PhysicalDevice);

			std::vector<VkDeviceQueueCreateInfo> deviceQueueInfos;
			std::set<uint32_t> uniqueQueueFamilies = { m_Indices.graphicsFamily.value(), m_Indices.presentFamily.value() };

			float priority = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies) {
				VkDeviceQueueCreateInfo deviceQueueInfo{};
				deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				deviceQueueInfo.queueFamilyIndex = m_Indices.graphicsFamily.value();
				deviceQueueInfo.queueCount = 1;
				deviceQueueInfo.pQueuePriorities = &priority;
				deviceQueueInfos.push_back(deviceQueueInfo);
			}

			VkPhysicalDeviceFeatures deviceFeatures{};

			VkDeviceCreateInfo deviceInfo{};
			deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceInfo.pQueueCreateInfos = deviceQueueInfos.data();
			deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueInfos.size());
			deviceInfo.pEnabledFeatures = &deviceFeatures;
			deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

			if (validation) {
				deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				deviceInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else {
				deviceInfo.enabledExtensionCount = 0;
			}

			VK_CHECK(vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device));
			volkLoadDevice(m_Device);

			std::cout << "Created virtual/logical device\n";

			vkGetDeviceQueue(m_Device, m_Indices.graphicsFamily.value(), 0, &m_Queue);
			
			std::cout << "Grabbed Queue\n";
		}

		// Create swapchain
		{
			SwapChainSupportDetails details = QuerySwapchainSupport(m_PhysicalDevice);

			VkSurfaceFormatKHR format = ChooseSwapchainFormat(details.formats);
			VkPresentModeKHR presentMode = ChoosePresentMode(details.presentModes);
			VkExtent2D extent = ChooseExtent(details.capabilities, window);

			uint32_t imageCount = details.capabilities.minImageCount + 1;
			if(details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
				imageCount = details.capabilities.maxImageCount;

			VkSwapchainCreateInfoKHR swapchainInfo{};
			swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainInfo.surface = m_Surface;

			swapchainInfo.minImageCount = imageCount;
			swapchainInfo.imageFormat = format.format;
			swapchainInfo.imageColorSpace = format.colorSpace;
			swapchainInfo.imageExtent = extent;
			swapchainInfo.presentMode = presentMode;
			swapchainInfo.imageArrayLayers = 1;
			swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			m_SwapchainFormat = format.format;
			m_SwapchainExtent = extent;

			if (m_Indices.graphicsFamily != m_Indices.presentFamily) {
				uint32_t familyIndices[] = {m_Indices.graphicsFamily.value(), m_Indices.presentFamily.value()};

				swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapchainInfo.queueFamilyIndexCount = 2;
				swapchainInfo.pQueueFamilyIndices = familyIndices;
			}
			else
				swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

			swapchainInfo.preTransform = details.capabilities.currentTransform;
			swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchainInfo.clipped = VK_TRUE;
			swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

			VK_CHECK(vkCreateSwapchainKHR(m_Device, &swapchainInfo, nullptr, &m_Swapchain));

			std::cout << "Created KHR Swapchain!\n";
		}

		// Get images and create image views
		{
			uint32_t swapchainImageCount;
			vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);
			m_SwapchainImages.resize(swapchainImageCount);
			vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, m_SwapchainImages.data());

			m_SwapchainImageViews.resize(swapchainImageCount);
			for (int i = 0; i < swapchainImageCount; i++) {
				VkImageViewCreateInfo imageViewInfo{};
				imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewInfo.image = m_SwapchainImages[i];

				imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewInfo.format = m_SwapchainFormat;

				imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

				imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageViewInfo.subresourceRange.baseMipLevel = 0;
				imageViewInfo.subresourceRange.levelCount = 1;
				imageViewInfo.subresourceRange.baseArrayLayer = 0;
				imageViewInfo.subresourceRange.layerCount = 1;

				VK_CHECK(vkCreateImageView(m_Device, &imageViewInfo, nullptr, &m_SwapchainImageViews[i]));
			}

			std::cout << "Created image views\n";
		}

		// Create swapchain renderpass
		{
			VkAttachmentDescription colorAttachment{};
			colorAttachment.format = m_SwapchainFormat;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentRef{};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo renderpassInfo{};
			renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderpassInfo.attachmentCount = 1;
			renderpassInfo.pAttachments = &colorAttachment;
			renderpassInfo.subpassCount = 1;
			renderpassInfo.pSubpasses = &subpass;
			renderpassInfo.dependencyCount = 1;
			renderpassInfo.pDependencies = &dependency;

			VK_CHECK(vkCreateRenderPass(m_Device, &renderpassInfo, nullptr, &m_RenderPass));

			std::cout << "Created renderpass!\n";
		}

		// Create swapchain framebuffers
		{
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.width = m_SwapchainExtent.width;
			framebufferInfo.height = m_SwapchainExtent.height;
			framebufferInfo.layers = 1;

			m_SwapchainFramebuffers.resize(m_SwapchainImageViews.size());
			for (int i = 0; i < m_SwapchainFramebuffers.size(); i++) {
				VkImageView attachments[] = {
					m_SwapchainImageViews[i]
				};

				framebufferInfo.attachmentCount = static_cast<uint32_t>(sizeof(attachments) / sizeof(attachments[0]));
				framebufferInfo.pAttachments = attachments;

				VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]));
			}
		}
	
		// Create command pool & buffer
		{
			VkCommandPoolCreateInfo commandPoolInfo{};
			commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolInfo.queueFamilyIndex = m_Indices.graphicsFamily.value();
			
			VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_CommandPool));
			std::cout << "Created command pool!\n";

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = m_CommandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = FRAMES_IN_FLIGHT;

			m_CommandBuffers.resize(FRAMES_IN_FLIGHT);
			VK_CHECK(vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()));
		}

		// Synchronization
		{
			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			m_ImagesAvailable.resize(FRAMES_IN_FLIGHT);
			m_FramesFinished.resize(FRAMES_IN_FLIGHT);
			m_InFlights.resize(FRAMES_IN_FLIGHT);

			for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
				VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImagesAvailable[i]));
				VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_FramesFinished[i]));
				VK_CHECK(vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlights[i]));
			}
		}

		// Create allocator
		{
			VmaVulkanFunctions vma_vulkan_func{};
			vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
			vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
			vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
			vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
			vma_vulkan_func.vkCreateImage = vkCreateImage;
			vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
			vma_vulkan_func.vkDestroyImage = vkDestroyImage;
			vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
			vma_vulkan_func.vkFreeMemory = vkFreeMemory;
			vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
			vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
			vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
			vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
			vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
			vma_vulkan_func.vkMapMemory = vkMapMemory;
			vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
			vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;
			vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;

			VmaAllocatorCreateInfo allocatorInfo{};
			allocatorInfo.instance = m_Instance;
			allocatorInfo.device = m_Device;
			allocatorInfo.physicalDevice = m_PhysicalDevice;
			allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
			allocatorInfo.pVulkanFunctions = &vma_vulkan_func;

			vmaCreateAllocator(&allocatorInfo, &m_Allocator);
		}
	}

	void VKRenderer::Cleanup() {
		vkDeviceWaitIdle(m_Device);

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(m_Device, m_ImagesAvailable[i], nullptr);
			vkDestroySemaphore(m_Device, m_FramesFinished[i], nullptr);
			vkDestroyFence(m_Device, m_InFlights[i], nullptr);
		}
		std::cout << "Destroyed sync objects\n";

		for (GraphicsPipeline* pipeline : m_Pipelines)
			delete pipeline;
		std::cout << "Destroyed pipelines\n";
		vmaDestroyAllocator(m_Allocator);
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
		std::cout << "Destroyed command pool\n";
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
		std::cout << "Destroyed renderpass\n";
		for (VkFramebuffer framebuffer : m_SwapchainFramebuffers)
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		std::cout << "Destroyed framebuffers\n";
		for (VkImageView view : m_SwapchainImageViews)
			vkDestroyImageView(m_Device, view, nullptr);
		std::cout << "Destroyed image views\n";

		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
		std::cout << "Destroyed swapchain\n";
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		std::cout << "Destroyed surface\n";
		vkDestroyDevice(m_Device, nullptr);
		std::cout << "Destroyed device\n";
		vkDestroyInstance(m_Instance, nullptr);
		std::cout << "Destroyed instance\n";
		DISABLE_DUMP;
	}

	void VKRenderer::Record(VkClearValue clearValue) {
		vkWaitForFences(m_Device, 1, &m_InFlights[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		if (vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_ImagesAvailable[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex) == VK_ERROR_OUT_OF_DATE_KHR) {
			// Recreate swapchain, resized window
			RecreateSwapchain();
			m_ResizedThisFrame = true;
			return;
		}

		vkResetFences(m_Device, 1, &m_InFlights[m_CurrentFrame]);
		vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;

		VK_CHECK(vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo));

		VkRenderPassBeginInfo renderpassBeginInfo{};
		renderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpassBeginInfo.renderPass = m_RenderPass;
		renderpassBeginInfo.framebuffer = m_SwapchainFramebuffers[imageIndex];
		renderpassBeginInfo.renderArea.offset = { 0, 0 };
		renderpassBeginInfo.renderArea.extent = m_SwapchainExtent;
		renderpassBeginInfo.clearValueCount = 1;
		renderpassBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VKRenderer::DrawMesh(VKMesh& mesh) {
		mesh.Render(m_CommandBuffers[m_CurrentFrame]);
	}

	void VKRenderer::Present() {
		if (m_ResizedThisFrame) {
			m_ResizedThisFrame = false;
			return;
		}
		vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);
		VK_CHECK(vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]));

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_ImagesAvailable[m_CurrentFrame]};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

		VkSemaphore signalSemaphores[] = { m_FramesFinished[m_CurrentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK(vkQueueSubmit(m_Queue, 1, &submitInfo, m_InFlights[m_CurrentFrame]));

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(m_Queue, &presentInfo);

		m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_IN_FLIGHT;
	}

	GraphicsPipeline* VKRenderer::CreatePipeline(GraphicsPipelineInfo info) {
		GraphicsPipeline* pipeline = new GraphicsPipeline(m_RenderPass);
		info.swapchainExtent = m_SwapchainExtent;
		pipeline->Create(info);
		m_Pipelines.push_back(pipeline);
		return pipeline;
	}

	VkDevice* VKRenderer::GetLogicalDevice() { return &m_Device; }
	VkExtent2D* VKRenderer::GetExtent() { return &m_SwapchainExtent; }
	VkRenderPass* VKRenderer::GetRenderPass() { return &m_RenderPass; }
	VmaAllocator* VKRenderer::GetAllocator() { return &m_Allocator; }
	VKRenderer::QueueFamilyIndices* VKRenderer::GetQueueFamilyIndices() { return &m_Indices; }
}