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

		if (m_PhysDeviceHandle == nullptr)
		{
			//Just get the first physical device if discrete is not supported.
			m_PhysDeviceHandle = devices[0];
			VkPhysicalDeviceProperties2 properties2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };

			vkGetPhysicalDeviceProperties2(m_PhysDeviceHandle, &properties2);

			VkPhysicalDeviceFeatures2 features2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
			vkGetPhysicalDeviceFeatures2(m_PhysDeviceHandle, &features2);

			m_Features = features2.features;
			m_Properties = properties2.properties;
		}

		//if (m_PhysDeviceHandle == VK_NULL_HANDLE)
		//{
		//	throw std::runtime_error("failed to find a suitable GPU!");
		//}

		std::cout << "physical device: " << m_Properties.deviceName << std::endl;
		m_Properties.deviceType;
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

		VkPhysicalDeviceFeatures2 features2 = {};
		features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

		VkPhysicalDeviceVulkan12Features vulkan12Features = {}; // Set up Vulkan 1.2 features
		vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		vulkan12Features.bufferDeviceAddress = VK_TRUE; // Enable the feature
		vulkan12Features.descriptorIndexing = VK_TRUE;

		// Link the Vulkan 1.2 features struct to the VkPhysicalDeviceFeatures2 struct
		features2.pNext = &vulkan12Features;

		features2.features.samplerAnisotropy = VK_TRUE;
		features2.features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
		features2.features.independentBlend = VK_TRUE;
		features2.features.multiDrawIndirect = VK_TRUE;
		/*VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures = features2.features;
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
		deviceFeatures.next*/



		//deviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature will affect performace cost

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = VK_NULL_HANDLE;
		createInfo.pNext = &features2;
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
			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
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

		VkPhysicalDeviceProperties2 properties2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };

		vkGetPhysicalDeviceProperties2(device, &properties2);

		VkPhysicalDeviceFeatures2 features2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
		vkGetPhysicalDeviceFeatures2(device, &features2);

		m_Features = features2.features;
		m_Properties = properties2.properties;
		return indices.isComplete() && extensionsSupported && swapChainAdequate && features2.features.samplerAnisotropy &&
			features2.features.shaderSampledImageArrayDynamicIndexing
			&& properties2.properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
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

	VkFormat VulkanInstance::CheckForValidDepthStencil(const std::vector<VkFormat>& depthFormats)
	{
		std::uint32_t validIndex = 0;
		std::uint32_t listCnt = std::uint32_t(depthFormats.size());
		for (; validIndex < listCnt; ++validIndex)
		{
			VkFormatProperties formatProbs;
			vkGetPhysicalDeviceFormatProperties(m_PhysDeviceHandle, depthFormats[validIndex], &formatProbs);
			if (formatProbs.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				break;


		}
		return depthFormats[validIndex];


	}

	VulkanInstance::~VulkanInstance()
	{

	}


}//namespace TDS