/*!*************************************************************************
****
\file vulkanInstance.cpp
\author Ng Zuo Xian Amadeus
\par DP email: ng.z@digipen.edu
\par Course: CSD3400
\par Section: a
\date 22-9-2023
\brief  Function definitions of the vulkanInstance Class
****************************************************************************
***/

//#define STB_IMAGE_IMPLEMENTATION
//#include "dds/stb_image.h"

#include "vulkanTools/vulkanInstance.h"
#include "Logger/Logger.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/CommandManager.h"
//#include "Input/Input.h"
//
//#include "vulkanTools/vmaSetup.h"
namespace TDS
{
	VulkanInstance::VulkanInstance(const WindowsWin& _Windows)
	{
		VkResult err;
		vsync = _Windows.settings.vsync;

		err = createInstance(_Windows.settings.validation);
		if (err) {
			TDS_ERROR("Could not create Vulkan instance\n");
		}
		//if request, enable default validation layers for debugging
		if (_Windows.settings.validation)
		{
			enableValidate = true; //to be remove
			TDS::Debug::setupDebugger(m_VKhandler);
		}

		CreateSurface(_Windows);
		ChoosePhysicalDevice();
		CreateLogicalDevice(_Windows);

	}
	void VulkanInstance::CreateSurface(const WindowsWin& _Windows)
	{
		auto pFNVKCreateWin32Surface
		{
		  reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>
		  (
			vkGetInstanceProcAddr(m_VKhandler, "vkCreateWin32SurfaceKHR")
		  )
		};
		if (nullptr == pFNVKCreateWin32Surface)
			TDS_ERROR("Vulkan Driver missing the VK_KHR_win32_surface extension\n");


		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext = nullptr;
		surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
		surfaceCreateInfo.hwnd = _Windows.getWindowHandler();
		VkResult err = vkCreateWin32SurfaceKHR(m_VKhandler, &surfaceCreateInfo, nullptr, &m_Surface);

		if (err != VK_SUCCESS)
			TDS_ERROR("failed to create window surface!");
	}

	void VulkanInstance::ChoosePhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VKhandler, &deviceCount, nullptr);

		if (deviceCount == 0)
			TDS_ERROR("failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VKhandler, &deviceCount, devices.data());

		//evaluate each of them and check suitability
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				m_PhysDeviceHandle = device;
				//m_msaaSamples = getMaxUsableSampleCount();
				break;
			}
		}

		if (m_PhysDeviceHandle == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		vkGetPhysicalDeviceProperties(m_PhysDeviceHandle, &m_Properties);
		std::cout << "physical device: " << m_Properties.deviceName << std::endl;
	}

	void VulkanInstance::CreateLogicalDevice(const WindowsWin& _Windows)
	{
		m_SelectedIndices = findQueueFamilies(m_PhysDeviceHandle);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies =
		{
		  m_SelectedIndices.graphicsFamily.value(),
		  m_SelectedIndices.presentFamily.value(),
		  m_SelectedIndices.computeFamily.value()
		};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		//deviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature will affect performace cost

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (_Windows.settings.validation)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_PhysDeviceHandle, &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(m_logicalDevice, m_SelectedIndices.graphicsFamily.value(), 0, &m_graphicQueue);
		vkGetDeviceQueue(m_logicalDevice, m_SelectedIndices.presentFamily.value(), 0, &m_presentQueue);
		vkGetDeviceQueue(m_logicalDevice, m_SelectedIndices.computeFamily.value(), 0, &m_ComputeQueue);
	}
	//void VulkanInstance::CreateCommandPool()
	//{

	//	//VkCommandPoolCreateInfo poolInfo{};
	//	//poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//	//poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	//	//poolInfo.queueFamilyIndex = m_SelectedIndices.graphicsFamily.value();

	//	//if (vkCreateCommandPool(m_logicalDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
	//	//	throw std::runtime_error("failed to create command pool!");
	//	//}
	//}

	VkResult VulkanInstance::createInstance(bool enableValidation)
	{
		//feed our application info to struct VkApplicationInfo
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Tear Drop Studio";
		appInfo.pEngineName = "Tear Drop Engine";
		appInfo.apiVersion = apiVersion;


		//enabling Extensions
		std::vector<const char*> instanceExtensions
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};

		// Get extensions supported by the instance and store for later use
		uint32_t extCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (const VkExtensionProperties& extension : extensions)
				{
					supportedInstanceExtensions.push_back(extension.extensionName);
				}
			}
		}

		// Enabled requested instance extensions
		if (enabledInstanceExtensions.size() > 0)
		{
			for (const char* enabledExtension : enabledInstanceExtensions)
			{
				// Output message if requested extension is not available
				if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), enabledExtension) == supportedInstanceExtensions.end())
				{
					std::cerr << "Enabled instance extension \"" << enabledExtension << "\" is not present at instance level\n";
				}
				instanceExtensions.push_back(enabledExtension);
			}
		}


		//Instance create info
		VkInstanceCreateInfo instanceCreateInfo{};
		{
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pNext = nullptr;
			//instanceCreateInfo.flags				= 0;
			instanceCreateInfo.pApplicationInfo = &appInfo;

			//Validation Layering
			if (enableValidation)
			{
				if (checkValidationLayerSupport())
				{
					instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
					instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
				}
				else
				{
					std::cerr << "Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled\n";
				}
			}

			if (enableValidation || std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != supportedInstanceExtensions.end()) {
				instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}

			if (instanceExtensions.size() > 0)
			{
				instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
				instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
			}
		};



		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_VKhandler);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
		// If the debug utils extension is present we set up debug functions, so samples an label objects for debugging
		if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != supportedInstanceExtensions.end())
		{
			std::cout << "Engine: " << appInfo.pEngineName << std::endl;
		}
		return result;

	}

	//checks if all of the requested layers are available
	bool VulkanInstance::checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}


		return true;
	}

	VulkanInstance::QueueFamilyIndices VulkanInstance::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i{ 0 };
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}
			else if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				indices.computeFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

			if (presentSupport)
			{
				if (!indices.presentFamily.has_value())
				{
					if (indices.graphicsFamily != i)
						TDS_WARN("Different queue index is being used for presentation vs graphics: %d\n", i);
					indices.presentFamily = i;
				}

			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	bool VulkanInstance::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	bool VulkanInstance::isDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;

		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	VulkanInstance::SwapChainSupportDetails VulkanInstance::querySwapChainSupport(const VkPhysicalDevice& device)
	{
		SwapChainSupportDetails details;

		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities) != VK_SUCCESS)
			std::cerr << "error";

		uint32_t formatCount;
		if (vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr) != VK_SUCCESS)
			std::cerr << "error";

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			if (vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data()) != VK_SUCCESS)
				std::cerr << "error";
		}

		uint32_t presentModeCount;
		if (vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr) != VK_SUCCESS)
			std::cerr << "error";

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			if (vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data()) != VK_SUCCESS)
				std::cerr << "error";
		}

		return details;
	}

	void VulkanInstance::ShutDown()
	{
		if (m_logicalDevice)
		{
			vkDestroyDevice(m_logicalDevice, nullptr);
			m_logicalDevice = nullptr;
		}
		if (enableValidate)
		{
			TDS::Debug::freeDebugger(m_VKhandler);
		}
		if (m_Surface)
		{
			vkDestroySurfaceKHR(m_VKhandler, m_Surface, nullptr);
			m_Surface = nullptr;
		}
		if (m_VKhandler)
		{
			vkDestroyInstance(m_VKhandler, nullptr);
			m_VKhandler = nullptr;
		}
	}

	uint32_t VulkanInstance::findMemoryType(const uint32_t& typeFiler, VkMemoryPropertyFlags properties)
	{
		//query info about the available types of memory 
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_PhysDeviceHandle, &memProperties);

		for (uint32_t i{ 0 }; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFiler & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	VkFormat VulkanInstance::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (auto format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_PhysDeviceHandle, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;

		}

		throw std::runtime_error("failed to find supported format!");
	}



	void VulkanInstance::createBuffers(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
		VkDeviceMemory& buffermemory)
	{

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;									 //size of buffer in bytes
		bufferInfo.usage = usage;								 //use for other usage cause use | (other usage)
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;			 //can own by a specific queue family but for now graphics queue only
		bufferInfo.flags = 0;									 // to configure sparse buffer memory not relevant now

		if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &buffer))
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}


		//Memory Requirements
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_logicalDevice, buffer, &memRequirements);

		//Memory Allocation
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		//The right way to allocate memory for a large number of objects at the same time is to create a custom allocator that splits up a 
		// single allocation among many different objects by using the offset parameters that we've seen in many functions.
		//You can either implement such an allocator yourself, or use the VulkanMemoryAllocator library provided by the GPUOpen initiative

		if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &buffermemory) != VK_SUCCESS)//cant call this everytime  
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(m_logicalDevice, buffer, buffermemory, 0);
	}

	VkCommandBuffer VulkanInstance::beginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = GraphicsManager::getInstance().getCommandManager().GetPool(POOLTYPE::GRAPHICS);
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanInstance::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer); //must end it before submitting

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_graphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_graphicQueue);

		vkFreeCommandBuffers(m_logicalDevice, GraphicsManager::getInstance().getCommandManager().GetPool(POOLTYPE::GRAPHICS), 1, &commandBuffer);
	}

	//copy contents from one buffer to another
	void VulkanInstance::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{

		VkCommandBuffer commandBuffer = beginSingleTimeCommands();
		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}

	void VulkanInstance::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		endSingleTimeCommands(commandBuffer);
	}

	void VulkanInstance::createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory)
	{
		if (vkCreateImage(m_logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
			throw std::runtime_error("failed to create image");


		VkMemoryRequirements memRequirements{};
		vkGetImageMemoryRequirements(m_logicalDevice, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		if (vkBindImageMemory(m_logicalDevice, image, imageMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to bind image memory!");
		}
	}

	VulkanInstance::~VulkanInstance()
	{
		//cleanupSwapChain();

		//vkDestroyPipeline(m_logicalDevice, m_graphicPipeline, nullptr);
		//vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr);
		//vkDestroyRenderPass(m_logicalDevice, m_RenderPass, nullptr);

		//for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; i++)
		//{
		//	vkDestroyBuffer(m_logicalDevice, uniformBuffers[i], nullptr);
		//	vkFreeMemory(m_logicalDevice, uniformBuffersMemory[i], nullptr);
		//}
		////vkDestroyDescriptorPool(m_logicalDevice, m_ImguiDescriptorPool, nullptr);
		//vkDestroyDescriptorPool(m_logicalDevice, m_descriptorPool, nullptr);

		//vkDestroySampler(m_logicalDevice, m_textureSampler, nullptr);
		//vkDestroyImageView(m_logicalDevice, m_textureImageView, nullptr);

		//vkDestroyImage(m_logicalDevice, m_textureImage, nullptr);
		//vkFreeMemory(m_logicalDevice, m_textureImageMemory, nullptr);



		//vkDestroyDescriptorSetLayout(m_logicalDevice, m_descriptorSetLayout, nullptr);

		//vkDestroyBuffer(m_logicalDevice, m_indexBuffer, nullptr);
		//vkFreeMemory(m_logicalDevice, m_IndexBufferMemory, nullptr);

		//vkDestroyBuffer(m_logicalDevice, m_vertexBuffer, nullptr);
		//vkFreeMemory(m_logicalDevice, m_vertexBufferMemory, nullptr);





		//vkDestroyCommandPool(m_logicalDevice, m_CommandPool, nullptr);// command pool will free command buffers for us.
		//ShutDown();
	}
	/*

		//swapchain setup
		{
			createSwapChain(_Windows);
		}
		//image View
		{
			createImageViews();
		}
		//render pass
		{
			VkAttachmentDescription colorAttachment{};
			colorAttachment.format = m_swapChainImageFormat;
			colorAttachment.samples = m_msaaSamples;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription depthAttachment{};
			depthAttachment.format = findDepthFormat();
			depthAttachment.samples = m_msaaSamples;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		//dont care about depth data??
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			//resolve multisampled color image into regular attachment
			VkAttachmentDescription colorAttachmentResolve{};
			colorAttachmentResolve.format = m_swapChainImageFormat;
			colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


			VkAttachmentReference colorAttachmentRef{};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depthAttachmentRef{};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference colorAttachmentResolveRef{};
			colorAttachmentResolveRef.attachment = 2;
			colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;
			subpass.pResolveAttachments = &colorAttachmentResolveRef;


			VkSubpassDependency dependency{}; //refering to both attachments
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


			//will be more for stencil or ????
			std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment , colorAttachmentResolve};

			VkRenderPassCreateInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
				throw std::runtime_error("failed to create render pass!");
			}

		}


		{
			createDescriptorSetLayout();
		}

		//graphics pipeline???
		{
			auto vertShaderCode = readFile("../assets/shaders/vert.spv");
			auto fragShaderCode = readFile("../assets/shaders/frag.spv");

			VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
			VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			shaderStages = { vertShaderStageInfo, fragShaderStageInfo };


			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

			//get the 2 descriptions from struct vertex
			auto bindingDescription = Vertex::getBindingDescription();
			auto attributeDescription = Vertex::getAttributeDescriptions();
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());

			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //careful with face
			rasterizer.depthBiasEnable = VK_FALSE;

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;//VK_TRUE; // enable sample shading in the pipeline come with additional performance cost
			multisampling.rasterizationSamples = m_msaaSamples;
			//multisampling.minSampleShading = .2f; // min fraction for sample shading; closer to one is smoother

			//depthStencil
			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;  //check if depth of new fragments should compare to depthbuffer.. then decide whether to discard
			depthStencil.depthWriteEnable = VK_TRUE; //if pass depth testing , should it be written to the depth buffer?
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; //comparison that is performed to keep or discard fragments(lower depth = closer, so the depth of new fragments should be less.)
			depthStencil.depthBoundsTestEnable = VK_FALSE; // keep fragments that fall within the specified depth range
			depthStencil.minDepthBounds = 0.f; //optional
			depthStencil.maxDepthBounds = 1.f; //optional
			depthStencil.stencilTestEnable = VK_FALSE; // *to use this, need to ensure that the format of depth/stencil image contains a stencil component
			depthStencil.front = {}; // Optional
			depthStencil.back = {}; // Optional




			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;

			VkPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY;
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f;
			colorBlending.blendConstants[1] = 0.0f;
			colorBlending.blendConstants[2] = 0.0f;
			colorBlending.blendConstants[3] = 0.0f;


			//Dynamic state make it more flexible to feed in data in draw time?
			std::vector<VkDynamicState> dynamicStates
			{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.pNext = VK_NULL_HANDLE;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();
			//dynamicstate.pFlages ??? dont need?

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
			pipelineLayoutInfo.pushConstantRangeCount = 0;

			if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages.data();
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = m_pipelineLayout;
			pipelineInfo.renderPass = m_RenderPass;
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

			if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicPipeline) != VK_SUCCESS) {
				throw std::runtime_error("failed to create graphics pipeline!");
			}



			vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
			vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);

		}
		//msaa
		{
			createColorResource();
		}
		//depth buffer must call before framebuffers cause of depth image view
		{
			createDepthResource();
		}


		//framebuffers
		{
			createFrameBuffer();
		}




		//stb texture (since  use command buffers we share call it after command pool )
		{
			createTextureImage();
		}

		//Texture imageView
		{
			createTextureImageView();
		}
		//create sampler
		{
			createTextureSampler();
		}

		{
			loadModel();
		}
		//vertexBuffer
		{
			createVertexBuffer();
		}
		//Indexbuffer
		{
			createIndexBuffer();
		}
		//create UniformBuffer();
		{
			createUniformBuffers();
		}

		//create descriptorPool
		{
			createDescriptorPool();
			createDescriptorSets();
		}


		//command buffer
		{
			m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = m_commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

			if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate command buffers!");
			}
		}
		//create sync object
		{
			//all need to resize as per frame needed
			m_imageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
			m_renderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
			m_inFlightFence.resize(MAX_FRAMES_IN_FLIGHT);


			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (size_t i{0}; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore[i]) != VK_SUCCESS ||
					vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]) != VK_SUCCESS ||
					vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS)
					{
						throw std::runtime_error("failed to create synchronization objects for a frame!");
					}
			}
		}

	}


	VkShaderModule VulkanInstance::createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}


	void VulkanInstance::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_swapChainExtent;

		//now have multiple attachment load op clear (depthattach and colour attach)
		//therefore need mulitple clear values???
		std::array<VkClearValue, 2> clearValues{};
		Vec4 clearColorVal{ 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[0].color = {{clearColorVal.x,
								 clearColorVal.y,
								 clearColorVal.z,
								 clearColorVal.w} };
		clearValues[1].depthStencil = { 1.f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();



		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicPipeline);


		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swapChainExtent.width);
		viewport.height = static_cast<float>(m_swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


		//binding vertex buffer
		VkBuffer vertexBuffers[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		//not possible to use different indices for each vertex attribute, so we do still have
		//to completely duplicate vertex data even if just one attribute varies.
		vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32); //can be VK_INDEX_TYPE_UINT16

		// bind the right descriptor set for each frame to the descriptors in the shader
		//with vkCmdBindDescriptorSets
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout, 0, 1, &m_descriptorSets[m_currentFrame], 0, nullptr);

		//this one is like opengl draw array
		//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		//this one is like opengl draw index
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);//3rd parameter instance count is for instancing

		//imgui draw
		imguiHelper::Draw(commandBuffer);


		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
	void VulkanInstance::drawFrame(const WindowsWin& _Windows, float deltaTime)
	{

		vkWaitForFences(m_logicalDevice, 1, &m_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		if (VkResult result{ vkAcquireNextImageKHR(m_logicalDevice, m_SwapChain, UINT64_MAX, m_imageAvailableSemaphore[m_currentFrame], VK_NULL_HANDLE, &imageIndex) };
			result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain(_Windows);
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		updateUniformBuffer(m_currentFrame, deltaTime);




		// Only reset the fence if we are submitting work
		vkResetFences(m_logicalDevice, 1, &m_inFlightFence[m_currentFrame]);

		vkResetCommandBuffer(m_commandBuffers[m_currentFrame], /*VkCommandBufferResetFlagBits 0);

		recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex);




		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore[m_currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];

		VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore[m_currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(m_graphicQueue, 1, &submitInfo, m_inFlightFence[m_currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_SwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		if (VkResult result{ vkQueuePresentKHR(m_presentQueue, &presentInfo) };
			result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
		{
			framebufferResized = false;
			recreateSwapChain(_Windows);
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}


		//advance to the next frame
		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanInstance::updateUniformBuffer(uint32_t currentImage , float deltaTime)
	{
		camera.UpdateCamera(deltaTime);

		UniformBufferObject ubo{};
		ubo.model = Mat4();
		ubo.view = camera.GetViewMatrix();
		ubo.proj = Mat4::Perspective(camera.m_Fov * Mathf::Deg2Rad,
			static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height), 0.1f, 10.f);
		ubo.proj.m[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

	}

	void VulkanInstance::recreateSwapChain(const WindowsWin& _Windows)
	{

		vkDeviceWaitIdle(m_logicalDevice); //should not touch resources that may still be in use


		//maybe touching on recreate the renderpass in the future, for now just make it simple
		cleanupSwapChain();

		createSwapChain(_Windows);
		createImageViews();
		createColorResource();
		createDepthResource(); //The resolution of the depth buffer should change when the window is resized to match the new color attachment resolution
		createFrameBuffer();
	}

	void VulkanInstance::cleanupSwapChain()
	{
		//depth
		vkDestroyImageView(m_logicalDevice, m_depthImageView, nullptr);
		vkDestroyImage(m_logicalDevice, m_depthImage, nullptr);
		vkFreeMemory(m_logicalDevice, m_depthImageMemory, nullptr);

		//color msaa
		vkDestroyImageView(m_logicalDevice, m_colorImageView, nullptr);
		vkDestroyImage(m_logicalDevice, m_colorImage, nullptr);
		vkFreeMemory(m_logicalDevice, m_colorImageMemory, nullptr);

		//clean up framebuffers
		for (const auto& framebuffer : m_swapChainFramebuffers)
		{
			vkDestroyFramebuffer(m_logicalDevice, framebuffer, nullptr);
		}

		//clean up imageView
		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(m_logicalDevice, imageView, nullptr);
		}

		//clean up SwapChain
		vkDestroySwapchainKHR(m_logicalDevice, m_SwapChain, nullptr);
	}

	void VulkanInstance::createSwapChain(const WindowsWin& _Windows)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysDeviceHandle);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, _Windows);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;


		VkSwapchainCreateInfoKHR SwapChainInfo{};

		SwapChainInfo.sType = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		SwapChainInfo.pNext = { nullptr };
		SwapChainInfo.flags = { 0 };
		SwapChainInfo.surface = { m_Surface };
		SwapChainInfo.minImageCount = { imageCount };
		SwapChainInfo.imageFormat = { surfaceFormat.format };
		SwapChainInfo.imageColorSpace = { surfaceFormat.colorSpace };
		SwapChainInfo.imageExtent =
		{
		  { static_cast<decltype(VkExtent2D::width)>(_Windows.getWidth()) },
		  { static_cast<decltype(VkExtent2D::height)>(_Windows.getHeight()) }
		};
		SwapChainInfo.imageArrayLayers = { 1 };
		SwapChainInfo.imageUsage = { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };

		QueueFamilyIndices indices = findQueueFamilies(m_PhysDeviceHandle);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			SwapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			SwapChainInfo.queueFamilyIndexCount = 2;
			SwapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			SwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}


		SwapChainInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		SwapChainInfo.compositeAlpha = { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR };
		SwapChainInfo.presentMode = { presentMode };
		SwapChainInfo.clipped = VK_TRUE;
		SwapChainInfo.oldSwapchain = nullptr;  // reused here :^D

		if (vkCreateSwapchainKHR(m_logicalDevice, &SwapChainInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}
		vkGetSwapchainImagesKHR(m_logicalDevice, m_SwapChain, &imageCount, nullptr);
		m_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_logicalDevice, m_SwapChain, &imageCount, m_swapChainImages.data());

		m_swapChainImageFormat = surfaceFormat.format;
		m_swapChainExtent = extent;
	}

	void VulkanInstance::createImageViews()
	{
		swapChainImageViews.resize(m_swapChainImages.size());
		for (size_t i = 0; i < m_swapChainImages.size(); i++)
		{
			swapChainImageViews[i] = createImageView(m_swapChainImages[i], m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

		}
	}

	void VulkanInstance::createFrameBuffer()
	{
		m_swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			std::array<VkImageView, 3> attachments
			{
			   m_colorImageView,
			   m_depthImageView,
			   swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_swapChainExtent.width;
			framebufferInfo.height = m_swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	//use a host visible buffer as temp buffer and use a device local as actual vertex buffer
	void VulkanInstance::createVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;


		createBuffers(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,//Buffer used as source in a memory transfer operation
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);


		//Filling the vertex buffer
		void* data;
		vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

		createBuffers(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

		copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

		vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);

	}

	void VulkanInstance::createIndexBuffer()
	{
		VkDeviceSize buffersize = sizeof(m_indices[0]) * m_indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingbufferMemory;
		createBuffers(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingBuffer, stagingbufferMemory);


		void* data;
		vkMapMemory(m_logicalDevice, stagingbufferMemory, 0, buffersize, 0, &data);
		memcpy(data, m_indices.data(), (size_t)buffersize);
		vkUnmapMemory(m_logicalDevice, stagingbufferMemory);

		createBuffers(buffersize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_IndexBufferMemory);

		copyBuffer(stagingBuffer, m_indexBuffer, buffersize);

		vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(m_logicalDevice, stagingbufferMemory, nullptr);

	}

	//uniformbuffers
	void VulkanInstance::createUniformBuffers()
	{
		VkDeviceSize buffersize = sizeof(UniformBufferObject);
		//we don't want to update the buffer in preparation of the next frame while a previous one is still reading from it!
		//Thus, we need to have as many uniform buffers as we have frames in flight,
		//and write to a uniform buffer that is not currently being read by the GPU
		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; i++)
		{

			createBuffers(buffersize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(m_logicalDevice, uniformBuffersMemory[i], 0, buffersize, 0, &uniformBuffersMapped[i]);

		}

	}

	}
	void VulkanInstance::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0; //location 0?
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // what we describing now is a uniform buffer?
		uboLayoutBinding.descriptorCount = 1; //one uniform buffer for now
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;// referencing the descriptor from the vertex shader for now but can putcan be a combination of VkShaderStageFlagBits values  or VK_SHADER_STAGE_ALL_GRAPHICS
		uboLayoutBinding.pImmutableSamplers = nullptr; //optional (only relevant for image sampling

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
	void VulkanInstance::createDescriptorPool()
	{
		//describe which descriptor types our descriptor sets are going to contain and how many of them
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);


		//allocate one of these descriptors for every frame
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		//specify the maximum number of descriptor sets that may be allocated
		poolInfo.maxSets = static_cast<uint32_t>(poolSizes.size())*2;

		if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}

	}

	//descriptor pool to allocate from, the number of descriptor sets to allocate,
	//and the descriptor layout to base them on
	void VulkanInstance::createDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_textureImageView;
			imageInfo.sampler = m_textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(m_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}


	void VulkanInstance::createTextureImage()
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels{ stbi_load(TEXTURE_PATH.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha)};
		if (!pixels)
		{
			throw std::runtime_error("failed to load texture image!");
		}
		m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1; //add 1 so that the original image has a miplevel//std::log2 tell how many time can it be divide by 2

		VkDeviceSize imagesize = texWidth * texHeight * 4;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffers(imagesize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		// directly copy the pixel values that we got from the image loading library to the buffer:

		void* data;
		vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, imagesize, 0, &data);

		memcpy(data, pixels, static_cast<size_t>(imagesize));

		vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

		createImage(texWidth , texHeight, m_mipLevels, VK_SAMPLE_COUNT_1_BIT,VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT| VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);

		transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);

		copyBufferToImage(stagingBuffer, m_textureImage, texWidth , texHeight);

		/*transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_mipLevels);

		vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);


		//generate mipmaps

		generateMipmaps(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,texWidth, texHeight, m_mipLevels);
	}

	void VulkanInstance::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
	{
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(m_PhysDeviceHandle, imageFormat, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}



		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		//barrier. subresource.miplevel, oldlayout, newlayout, srcAccessmask and dstAccessmask will changed each transition
		for (uint32_t i{ 1 }; i < mipLevels; i++) //start with 1 is it because of the plus one in createTexture image
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
				0, 0, nullptr, 0, nullptr, 1, &barrier);

			//blit command will wait on this transition
			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0,0,0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0,0,0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;
			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			//tell the shaders
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;

		}

		//transitions the last mip level
		//not handled by the loop, since the last mip level is never blitted from.
		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		endSingleTimeCommands(commandBuffer);


	}


	void VulkanInstance::createImage(uint32_t width, uint32_t height, uint32_t miplevels, VkSampleCountFlagBits numSamples, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D; //tell is 1D, 2D or 3D
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = miplevels;
		imageInfo.arrayLayers = 1;
		//Vulkan supports many possible image formats, but we should use the same format for the
		//texels as the pixels in the buffer, otherwise the copy operation will fail.
		imageInfo.format = format;
		imageInfo.tiling = tiling;//one of two values
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//or VK_IMAGE_LAYOUT_PREINITIALIZED
		imageInfo.usage = usage;
		imageInfo.samples = numSamples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = 0; // Optional (is related to multisampling) (some optional flags for images that are related to sparse images)

		if (vkCreateImage(m_logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_logicalDevice, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(m_logicalDevice,image, imageMemory, 0);

	}




	void VulkanInstance::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipmaps)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(); // one time command

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipmaps;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(commandBuffer);
	}




	void VulkanInstance::createTextureImageView()
	{
		m_textureImageView = createImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
	}

	VkImageView VulkanInstance::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectflags, uint32_t mipmaps)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectflags; //VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipmaps;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	void VulkanInstance::createTextureSampler()
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_PhysDeviceHandle, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		samplerInfo.minLod = 0.f;
		samplerInfo.maxLod = static_cast<float>(m_mipLevels);
		samplerInfo.mipLodBias = 0.f;

		if (vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	void VulkanInstance::createColorResource()
	{
		VkFormat colorFormat = m_swapChainImageFormat;

		createImage(m_swapChainExtent.width, m_swapChainExtent.height, 1, m_msaaSamples, colorFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_colorImage, m_colorImageMemory);

		m_colorImageView = createImageView(m_colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	void VulkanInstance::createDepthResource()
	{
		VkFormat lookfordepthFormat = findDepthFormat();

		//create image
		createImage(m_swapChainExtent.width, m_swapChainExtent.height,1, m_msaaSamples ,lookfordepthFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthImageMemory);
		//create image view
		m_depthImageView = createImageView(m_depthImage, lookfordepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	}


	bool VulkanInstance::hasStencilComponent(VkFormat format)
	{
		return (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT);
	}

	void VulkanInstance::loadModel()
	{
		//read file via assimp
		Assimp::Importer importer;

		importer.SetPropertyBool(AI_CONFIG_PP_PTV_NORMALIZE, true); // telling assimp to normalize for us but take note that assimp normalize is form (-1 to 1)

		const aiScene* scene = importer.ReadFile(MODEL_PATH.data(), aiProcess_FlipUVs |
			aiProcess_GenUVCoords |
			aiProcess_Triangulate |
			//aiProcess_FindInstances |
			//aiProcess_FindInvalidData |
			//aiProcess_FindDegenerates |
			//aiProcess_FlipWindingOrder |
			//aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			//aiProcess_TransformUVCoords |
			aiProcess_PreTransformVertices |
			aiProcess_JoinIdenticalVertices |
			aiProcess_RemoveRedundantMaterials);

		//error check
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) //if is not zero
		{
			throw std::runtime_error( importer.GetErrorString() );
			return;
		}

		processNode(scene->mRootNode, scene);


	}

	void VulkanInstance::processNode(aiNode* node, const aiScene* scene)
	{
		// process each mesh located at the current node
		for (unsigned int i{ 0 }; i < node->mNumMeshes; i++)
		{
			//the node object only contains indices to index the actual objects in the scene.
			//the scene contains all the data , node is just to keep stuff organized (like relations between nodes)
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
		}

		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	void VulkanInstance::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill

		// walk through each of the mesh's vertices

		for (unsigned int i{ 0 }; i < mesh->mNumVertices; i++)
		{
			Vertex vertex{};
			Vec2 texVec{};
			Vec3 vector{}; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to our vec3 class so we transfer the data to this placeholder vec3 first.
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			vertex.pos = vector;

			// Get texture coordinates for the first set (index 0)
			if( mesh->mNumUVComponents[0])
			{
				texVec.x = mesh->mTextureCoords[0][i].x;
				texVec.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				texVec.x = 0.f;
				texVec.y = 0.f;
			}

			vertex.texCoord = texVec;
			vertex.color = { 1.f, 1.f ,1.f };
			vertices.push_back(vertex);
		}

		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				m_indices.push_back(face.mIndices[j]);
		}

	}

	VkSampleCountFlagBits VulkanInstance::getMaxUsableSampleCount()
	{
		//get the gpu propertoies
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysDeviceHandle, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
									physicalDeviceProperties.limits.framebufferDepthSampleCounts;

		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}
	*/

}//namespace TDS