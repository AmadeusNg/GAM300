/*!*************************************************************************
****
\file vulkanSwapChain.cpp
\author Ng Zuo Xian Amadeus
\par DP email: ng.z@digipen.edu
\par Course: CSD3400
\par Section: a
\date 22-9-2023
\brief  Function definitions of the vulkanSwapChain Class
****************************************************************************
***/

//#ifndef NOMINMAX
//#define NOMINMAX
//#endif//NOMINMAX


#include "vulkanTools/vulkanSwapChain.h"
#include "Logger/Logger.h"
#include "vulkanTools/vulkanTexture.h"

namespace TDS
{
	VulkanSwapChain::VulkanSwapChain(VulkanInstance& Instance, VkExtent2D WindowExtent) : m_Instance(Instance), m_WindowExtent(WindowExtent) {
		m_ClearAttachments.resize(2);
		init();
	}

	VulkanSwapChain::VulkanSwapChain(VulkanInstance& Instance, VkExtent2D WindowExtent, std::shared_ptr<VulkanSwapChain> prev) :
		m_Instance(Instance), m_WindowExtent(WindowExtent), m_OldSwapChain(prev) {
		m_ClearAttachments.resize(2);
		init();
		m_OldSwapChain = nullptr;
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		/*ShutDown();*/
	}

	void VulkanSwapChain::init() {
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDepthResource();
		CreateFrameBuffers();
		CreateSyncObjects();
	}


	/**
	* Acquires the next image in the swap chain
	*
	* @param presentCompleteSemaphore (Optional) Semaphore that is signaled when the image is ready for use
	* @param imageIndex Pointer to the image index that will be increased if the next image could be acquired
	*
	* @note The function will always wait until the next image has been acquired by setting timeout to UINT64_MAX
	*
	* @return VkResult of the image acquisition
	*/
	VkResult VulkanSwapChain::acquireNextImage(uint32_t* imageIndex)
	{
		vkWaitForFences(m_Instance.getVkLogicalDevice(), 1, &m_vInFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);


		// By setting timeout to UINT64_MAX we will always wait until the next image has been acquired or an actual error is thrown
		// With that we don't have to handle VK_NOT_READY
		return vkAcquireNextImageKHR(m_Instance.getVkLogicalDevice(), m_SwapChain, UINT64_MAX, m_vImageAvailableSemaphore[m_currentFrame], VK_NULL_HANDLE, imageIndex);
	}

	VkResult VulkanSwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* ImageIndex) {
		if (m_vImagesinFlight[*ImageIndex] != VK_NULL_HANDLE)
			vkWaitForFences(m_Instance.getVkLogicalDevice(), 1, &m_vImagesinFlight[*ImageIndex], VK_TRUE, UINT64_MAX);
		m_vImagesinFlight[*ImageIndex] = m_vImagesinFlight[m_currentFrame];

		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphore[] = { m_vImageAvailableSemaphore[m_currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = waitSemaphore;
		SubmitInfo.pWaitDstStageMask = waitStages;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = buffers;

		VkSemaphore SignalSemaphore[]{ m_vRenderFinishedSemaphore[m_currentFrame] };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = SignalSemaphore;

		vkResetFences(m_Instance.getVkLogicalDevice(), 1, &m_vInFlightFences[m_currentFrame]);

		if (vkQueueSubmit(m_Instance.getGraphicsQueue(), 1, &SubmitInfo, m_vInFlightFences[m_currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("failed to submit draw command buffer command");

		VkPresentInfoKHR PresentInfo{};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = SignalSemaphore;

		VkSwapchainKHR SwapChains[] = { m_SwapChain };
		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = SwapChains;
		PresentInfo.pImageIndices = ImageIndex;

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		return vkQueuePresentKHR(m_Instance.getPresentQueue(), &PresentInfo);

	}

	void VulkanSwapChain::ClearColors(VkCommandBuffer& cmdBuffer, iColor clearColor)
	{
		VkClearRect clearRect = {};
		clearRect.layerCount = 1;
		clearRect.baseArrayLayer = 0;
		clearRect.rect.offset = { 0, 0 };
		clearRect.rect.extent = m_SwapChainExtent;

		vkCmdClearAttachments(cmdBuffer, 2, m_ClearAttachments.data(), 1, &clearRect);
	}

	void VulkanSwapChain::CreateSwapChain() {
		VulkanInstance::SwapChainSupportDetails swapChainSupport = m_Instance.getSwapChainSupport();

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imagecount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imagecount > swapChainSupport.capabilities.maxImageCount)
			imagecount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createinfo.surface = m_Instance.getSurface();

		createinfo.minImageCount = imagecount;
		createinfo.imageFormat = surfaceFormat.format;
		createinfo.imageColorSpace = surfaceFormat.colorSpace;
		createinfo.imageExtent = extent;
		createinfo.imageArrayLayers = 1;
		createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VulkanInstance::QueueFamilyIndices Indices = m_Instance.findPhysicalQueueFamilies();
		uint32_t queuefamilyindices[] = { Indices.graphicsFamily.value(), Indices.presentFamily.value() };

		if (Indices.graphicsFamily != Indices.presentFamily) {
			createinfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createinfo.queueFamilyIndexCount = 2;
			createinfo.pQueueFamilyIndices = queuefamilyindices;
		}
		else {
			createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createinfo.queueFamilyIndexCount = 0;
			createinfo.pQueueFamilyIndices = nullptr;
		}

		createinfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createinfo.presentMode = presentMode;
		createinfo.clipped = VK_TRUE;

		createinfo.oldSwapchain = (m_OldSwapChain == nullptr) ? VK_NULL_HANDLE : m_OldSwapChain->m_SwapChain;

		if (vkCreateSwapchainKHR(m_Instance.getVkLogicalDevice(), &createinfo, nullptr, &m_SwapChain) != VK_SUCCESS)
			throw std::runtime_error("failed to create swap chain!");

		vkGetSwapchainImagesKHR(m_Instance.getVkLogicalDevice(), m_SwapChain, &imagecount, nullptr);
		m_vSwapChainImages.resize(imagecount);
		vkGetSwapchainImagesKHR(m_Instance.getVkLogicalDevice(), m_SwapChain, &imagecount, m_vSwapChainImages.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	void VulkanSwapChain::CreateImageViews() {
		m_vSwapChainImageViews.resize(m_vSwapChainImages.size());
		for (size_t i{ 0 }; i < m_vSwapChainImages.size(); ++i) {
			VkImageViewCreateInfo viewinfo{};
			viewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewinfo.image = m_vSwapChainImages[i];
			viewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewinfo.format = m_SwapChainImageFormat;
			viewinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewinfo.subresourceRange.baseMipLevel = 0;
			viewinfo.subresourceRange.levelCount = 1;
			viewinfo.subresourceRange.baseArrayLayer = 0;
			viewinfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_Instance.getVkLogicalDevice(), &viewinfo, nullptr, &m_vSwapChainImageViews[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create texture image view!");
		}
	}

	void VulkanSwapChain::CreateRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = getSwapChainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		//dont care about depth data??
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;


		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency{}; //refering to both attachments
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;

		dependency.dstSubpass = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		//will be more for stencil or ????
		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_Instance.getVkLogicalDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void VulkanSwapChain::CreateFrameBuffers() {
		m_vSwapChainFramebuffers.resize(getImageCount());
		for (size_t i{ 0 }; i < getImageCount(); ++i) {
			std::array<VkImageView, 2> Attachments = { m_vSwapChainImageViews[i], m_vDepthImageViews[i] };

			VkExtent2D swapchainextent = getSwapChainExtent();
			VkFramebufferCreateInfo framebufferinfo{};
			framebufferinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferinfo.renderPass = m_RenderPass;
			framebufferinfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
			framebufferinfo.pAttachments = Attachments.data();
			framebufferinfo.width = swapchainextent.width;
			framebufferinfo.height = swapchainextent.height;
			framebufferinfo.layers = 1;

			if (vkCreateFramebuffer(m_Instance.getVkLogicalDevice(), &framebufferinfo, nullptr, &m_vSwapChainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create framebuffer");
			m_ClearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[0].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_ClearAttachments[0].colorAttachment = 0;

			m_ClearAttachments[1].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			m_ClearAttachments[1].clearValue.depthStencil = { 1.0f, 0 };
		}
	}

	void VulkanSwapChain::CreateDepthResource() {
		VkFormat depthFormat = findDepthFormat();
		m_SwapChainDepthFormat = depthFormat;
		VkExtent2D swapchainExtent = getSwapChainExtent();

		m_vDepthImages.resize(getImageCount());
		m_vDepthImageAllocation.resize(getImageCount());
		/*m_vDepthImageMemory.resize(getImageCount());*/
		m_vDepthImageViews.resize(getImageCount());

		for (int i{ 0 }; i < m_vDepthImages.size(); ++i) {
			VkImageCreateInfo imageinfo{};
			imageinfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageinfo.imageType = VK_IMAGE_TYPE_2D;
			imageinfo.extent.width = swapchainExtent.width;
			imageinfo.extent.height = swapchainExtent.height;
			imageinfo.extent.depth = 1;
			imageinfo.mipLevels = 1;
			imageinfo.arrayLayers = 1;
			imageinfo.format = depthFormat;
			imageinfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageinfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageinfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageinfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageinfo.flags = 0;

			//CreateImageParams params{};
			//params.m_Format = depthFormat;
			//params.m_tiling = VK_IMAGE_TILING_OPTIMAL;
			//params.m_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			
			m_vDepthImageAllocation[i] = GraphicsAllocator::Allocate(&imageinfo, VMA_MEMORY_USAGE_GPU_ONLY, m_vDepthImages[i]);
			/*m_Instance.createImageWithInfo(imageinfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vDepthImages[i], m_vDepthImageMemory[i]);*/

			VkImageViewCreateInfo viewinfo{};
			viewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewinfo.image = m_vDepthImages[i];
			viewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewinfo.format = depthFormat;
			viewinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewinfo.subresourceRange.baseMipLevel = 0;
			viewinfo.subresourceRange.levelCount = 1;
			viewinfo.subresourceRange.baseArrayLayer = 0;
			viewinfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_Instance.getVkLogicalDevice(), &viewinfo, nullptr, &m_vDepthImageViews[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create texture image view");
		}

	}

	void VulkanSwapChain::CreateSyncObjects() {
		m_vImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
		m_vRenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
		m_vInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_vImagesinFlight.resize(getImageCount(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			if (vkCreateSemaphore(m_Instance.getVkLogicalDevice(), &semaphoreInfo, nullptr, &m_vImageAvailableSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_Instance.getVkLogicalDevice(), &semaphoreInfo, nullptr, &m_vRenderFinishedSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(m_Instance.getVkLogicalDevice(), &fenceInfo, nullptr, &m_vInFlightFences[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create synchronization objects for a frame");
		}
	}

	void VulkanSwapChain::ShutDown()
	{
		for (auto IV : m_vSwapChainImageViews) {
			if (IV)
			{
				vkDestroyImageView(m_Instance.getVkLogicalDevice(), IV, nullptr);
				IV = nullptr;
			}
		}
		m_vSwapChainImageViews.clear();

		if (m_SwapChain)
		{
			vkDestroySwapchainKHR(m_Instance.getVkLogicalDevice(), m_SwapChain, nullptr);
			m_SwapChain = nullptr;
		}

		for (int i{ 0 }; i < m_vDepthImages.size(); ++i) {
			if (m_vDepthImageViews[i])
			{
				vkDestroyImageView(m_Instance.getVkLogicalDevice(), m_vDepthImageViews[i], nullptr);
				m_vDepthImageViews[i] = nullptr;
			}
			if (m_vDepthImages[i])
			{
				GraphicsAllocator::FreeBuffer(m_vDepthImages[i], m_vDepthImageAllocation[i]);
				//vkDestroyImage(m_Instance.getVkLogicalDevice(), m_vDepthImages[i], nullptr);
				m_vDepthImages[i] = nullptr;
			}
			
			//Use VMA
			/*vkFreeMemory(m_Instance.getVkLogicalDevice(), m_vDepthImageMemory[i], nullptr);*/
		}
		m_vDepthImages.clear();
		for (auto Fb : m_vSwapChainFramebuffers) {
			if (Fb)
			{
				vkDestroyFramebuffer(m_Instance.getVkLogicalDevice(), Fb, nullptr);
				Fb = nullptr;
			}
		}
		m_vSwapChainFramebuffers.clear();
		if (m_RenderPass)
		{
			vkDestroyRenderPass(m_Instance.getVkLogicalDevice(), m_RenderPass, nullptr);
			m_RenderPass = nullptr;
		}


		for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			if (m_vImageAvailableSemaphore[i])
				vkDestroySemaphore(m_Instance.getVkLogicalDevice(), m_vImageAvailableSemaphore[i], nullptr);
			if (m_vRenderFinishedSemaphore[i])
				vkDestroySemaphore(m_Instance.getVkLogicalDevice(), m_vRenderFinishedSemaphore[i], nullptr);

		}

		for (auto& fence : m_vImagesinFlight)
		{
			if (fence)
				vkDestroyFence(m_Instance.getVkLogicalDevice(), fence, nullptr);
		}
		for (auto& fence : m_vInFlightFences)
		{
			if (fence)
				vkDestroyFence(m_Instance.getVkLogicalDevice(), fence, nullptr);
		}
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		/*	for (const auto& availablePresentMode : availablePresentModes) {
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
					std::cout << "Present mode: MailBox\n";
					return availablePresentMode;
				}
			}
			std::cout << "Present mode: V-Sync\n";
			return VK_PRESENT_MODE_FIFO_KHR;*/
		if (m_Instance.getVsync())
		{
			std::cout << "Present mode: V-Sync\n";
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				std::cout << "Present mode: MailBox\n";
				return availablePresentMode;
			}
		}
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {


			VkExtent2D actualExtent{ m_WindowExtent };

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	VkFormat VulkanSwapChain::findDepthFormat() {
		return m_Instance.findSupportedFormat({VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT }
		, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkFormat VulkanSwapChain::GetSwapChainImageFormat()
	{
		return m_SwapChainImageFormat;
	}

}//namespace TDS