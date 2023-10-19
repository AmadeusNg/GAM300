#include "pch.h"
#include "vulkanTools/VulkanHelper.h"
#include "../vulkanTools/vulkanTexture.h"
#include "../vulkanTools/VMATypes/VMABuffer.h"
#include "../vulkanTools/vulkanInstance.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/CommandManager.h"
namespace TDS
{
	VulkanTexture::VulkanTexture()
	{
	}
	VulkanTexture::~VulkanTexture()
	{
		Destroy();
	}

	void VulkanTexture::CreateTexture(TextureInfo& textureinfo)
	{
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
		CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();


		CreateImageParams params{};
		params.m_usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (textureinfo.m_StorageImage)
			params.m_usage |= VK_IMAGE_USAGE_STORAGE_BIT;

		params.m_tiling = VK_IMAGE_TILING_OPTIMAL;
		params.m_Format = textureinfo.m_Format;

		if (textureinfo.m_UsingMipMaps)
		{
			if (textureinfo.mipCount == 0)
			{
				std::uint32_t maxDimen = std::max(textureinfo.m_ExtentDimen.width, textureinfo.m_ExtentDimen.height);

				textureinfo.mipCount = 0;
				while (maxDimen > 0)
				{
					maxDimen >>= 1;
					textureinfo.mipCount++;
				}
			}
		}
		else
			textureinfo.mipCount = 1;



		m_ImageHdl = CreateVulkanImage(textureinfo.m_ExtentDimen, textureinfo.mipCount, VK_SAMPLE_COUNT_1_BIT, params, m_Allocation);

		m_ImageLayout = textureinfo.m_StorageImage ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VMABuffer stagingBuffer = VMABuffer::CreateStagingBuffer(textureinfo.m_ImageSize, instance, textureinfo.m_Data);

		CommandBufferInfo cmdBufferInfo{};
		cmdMgr.CreateSingleUseCommandBuffer(cmdBufferInfo);
		if (textureinfo.m_Data)
		{
			ImageMemoryLayoutInput inputLayout{};
			inputLayout.m_Image = m_ImageHdl;
			inputLayout.m_subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			inputLayout.m_subResourceRange.levelCount = 1;
			inputLayout.m_subResourceRange.layerCount = 1;
			inputLayout.m_srcAccessFlags = 0;
			inputLayout.m_dstAccessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
			inputLayout.m_oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			inputLayout.m_NewLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			inputLayout.m_srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			inputLayout.m_dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;

			SetImageMemoryBarrier(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, inputLayout);
			VkBufferImageCopy copyBuffer = {};
			copyBuffer.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyBuffer.imageSubresource.mipLevel = 0;
			copyBuffer.imageSubresource.baseArrayLayer = 0;
			copyBuffer.imageSubresource.layerCount = 1;
			copyBuffer.imageExtent.width = textureinfo.m_ExtentDimen.width;
			copyBuffer.imageExtent.height = textureinfo.m_ExtentDimen.height;
			copyBuffer.imageExtent.depth = 1;

			vkCmdCopyBufferToImage(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, stagingBuffer.GetBuffer(), m_ImageHdl,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyBuffer);
			inputLayout.m_srcAccessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
			inputLayout.m_dstAccessFlags = VK_ACCESS_TRANSFER_READ_BIT;
			inputLayout.m_oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			inputLayout.m_NewLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

			SetImageMemoryBarrier(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, inputLayout);

			for (int i = 1; i < textureinfo.mipCount; i++)
			{
				ImageMemoryLayoutInput paramsLayout{};
				paramsLayout.m_Image = m_ImageHdl;
				paramsLayout.m_subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				paramsLayout.m_subResourceRange.levelCount = 1;
				paramsLayout.m_subResourceRange.layerCount = 1;
				paramsLayout.m_srcAccessFlags = 0;
				paramsLayout.m_dstAccessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
				paramsLayout.m_oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				paramsLayout.m_NewLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				paramsLayout.m_srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
				paramsLayout.m_dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;

				SetImageMemoryBarrier(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, paramsLayout);

				VkImageBlit imageBlit{};
				imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBlit.srcSubresource.layerCount = 1;
				imageBlit.srcSubresource.mipLevel = i - 1;
				imageBlit.srcOffsets[1].x = std::int32_t(textureinfo.m_ExtentDimen.width >> (i - 1));
				imageBlit.srcOffsets[1].y = std::int32_t(textureinfo.m_ExtentDimen.height >> (i - 1));
				imageBlit.srcOffsets[1].z = 1;

				imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBlit.dstSubresource.layerCount = 1;
				imageBlit.dstSubresource.mipLevel = i;
				imageBlit.dstOffsets[1].x = std::int32_t(textureinfo.m_ExtentDimen.width >> i);
				imageBlit.dstOffsets[1].y = std::int32_t(textureinfo.m_ExtentDimen.height >> i);
				imageBlit.dstOffsets[1].z = 1;

				vkCmdBlitImage(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, m_ImageHdl, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					m_ImageHdl,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&imageBlit,
					VK_FILTER_LINEAR);

				paramsLayout.m_srcAccessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
				paramsLayout.m_dstAccessFlags = VK_ACCESS_TRANSFER_READ_BIT;
				paramsLayout.m_oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				paramsLayout.m_NewLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

				SetImageMemoryBarrier(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, paramsLayout);

			}

		}
		else
		{
			ImageMemoryLayoutInput paramsLayoutInput{};
			paramsLayoutInput.m_Image = m_ImageHdl;
			paramsLayoutInput.m_subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			paramsLayoutInput.m_subResourceRange.levelCount = textureinfo.mipCount;
			paramsLayoutInput.m_subResourceRange.layerCount = 1;
			paramsLayoutInput.m_srcAccessFlags = 0;
			paramsLayoutInput.m_dstAccessFlags = 0;
			paramsLayoutInput.m_oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			paramsLayoutInput.m_NewLayout = m_ImageLayout;
			paramsLayoutInput.m_srcStageFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			paramsLayoutInput.m_dstStageFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			SetImageMemoryBarrier(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, paramsLayoutInput);
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			setTextureLayout(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, paramsLayoutInput);

		}
		cmdMgr.EndExecution(cmdBufferInfo);

		CreateSampler(textureinfo);
		CreateImageView(textureinfo.m_Format);
		stagingBuffer.DestroyBuffer();

	}


	std::shared_ptr<VulkanTexture> VulkanTexture::CreateDefaultTexture()
	{
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
		CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();
		Texture textureInfo{};
		std::uint32_t width{4}, height{ 4 }, mips{ 1 };
		std::uint32_t size = width * height * 4;
		std::uint32_t data = 0xFFFFFFFF;
		std::shared_ptr<VulkanTexture> texture = std::make_shared<VulkanTexture>();
		VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		CreateImageParams params{ VK_FORMAT_R8G8B8A8_UNORM, VkImageTiling::VK_IMAGE_TILING_OPTIMAL, usage };
		texture->m_ImageHdl = VulkanTexture::CreateVulkanImage(VkExtent2D{ width, height }, 1, VK_SAMPLE_COUNT_1_BIT, params, texture->m_Allocation);
		VMABuffer staging = VMABuffer::CreateStagingBuffer(size, instance, &data);


		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;

		CommandBufferInfo cmdInfo{};
		cmdMgr.CreateSingleUseCommandBuffer(cmdInfo);
		ImageMemoryLayoutInput transferToDestination{ texture->m_ImageHdl, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, subresourceRange };
		SetImageMemoryBarrier(cmdInfo.m_CommandBuffer.m_CmdBuffer, transferToDestination);

		VkBufferImageCopy bufferCopyImage{};
		bufferCopyImage.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyImage.imageSubresource.mipLevel = 0;
		bufferCopyImage.imageSubresource.baseArrayLayer = 0;
		bufferCopyImage.imageSubresource.layerCount = 1;
		bufferCopyImage.imageExtent.width = width;
		bufferCopyImage.imageExtent.height = height;
		bufferCopyImage.imageExtent.depth = 1;

		vkCmdCopyBufferToImage(cmdInfo.m_CommandBuffer.m_CmdBuffer, staging.GetBuffer(), texture->m_ImageHdl, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyImage);

		ImageMemoryLayoutInput transitionToFirstMip{ texture->m_ImageHdl, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, subresourceRange };
		SetImageMemoryBarrier(cmdInfo.m_CommandBuffer.m_CmdBuffer, transitionToFirstMip);
		cmdMgr.EndExecution(cmdInfo);

		texture->m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		texture->CreateSampler(textureInfo.m_TextureInfo);
		texture->CreateImageView(VK_FORMAT_R8G8B8A8_UNORM);

		texture->m_DescriptorImageInfo.imageLayout = texture->m_ImageLayout;
		texture->m_DescriptorImageInfo.imageView = texture->m_BaseImageView;
		texture->m_DescriptorImageInfo.sampler = texture->m_Sampler;
		staging.DestroyBuffer();

		return texture;
	}
	std::shared_ptr<VulkanTexture> VulkanTexture::CreateDefaultCubeTexture(bool useAnistrophy)
	{
		std::shared_ptr<VulkanTexture> texture = std::make_shared<VulkanTexture>();
		CreateImageParams params{};
		params.m_Format = VK_FORMAT_R8G8B8A8_UNORM;
		params.m_usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		params.m_tiling = VK_IMAGE_TILING_OPTIMAL;
		texture->m_ImageHdl = CreateCubeImage({ 4,4 }, 1, VK_SAMPLE_COUNT_1_BIT, params, texture->m_Allocation);


		ImageMemoryLayoutInput layoutParams{};
		layoutParams.m_subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		layoutParams.m_subResourceRange.baseMipLevel = 0;
		layoutParams.m_subResourceRange.levelCount = 1;
		layoutParams.m_subResourceRange.layerCount = 6;
		layoutParams.m_Image = texture->m_ImageHdl;
		layoutParams.m_oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		layoutParams.m_NewLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		layoutParams.m_dstStageFlags = layoutParams.m_srcStageFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();
		CommandBufferInfo cmdBuffer{};
		if (cmdMgr.CreateSingleUseCommandBuffer(cmdBuffer))
		{
			setTextureLayout(cmdBuffer.m_CommandBuffer.m_CmdBuffer, layoutParams);
			cmdMgr.EndExecution(cmdBuffer);
		}

		VkDevice deviceRef = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		VkPhysicalDevice physical = GraphicsManager::getInstance().getVkInstance().getVkPhysicalDevice();

		VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.mipLodBias = 0.f;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerCreateInfo.minLod = 0.f;
		samplerCreateInfo.maxLod = 1.f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		//Check if we support anisotrophy

		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceFeatures(physical, &features);
		if (useAnistrophy && features.samplerAnisotropy)
		{
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(physical, &properties);
			samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
			samplerCreateInfo.anisotropyEnable = VK_TRUE;
		}
		else
		{
			samplerCreateInfo.maxAnisotropy = 1.0f;
			samplerCreateInfo.anisotropyEnable = VK_FALSE;
		}
		VkResult result = vkCreateSampler(deviceRef, &samplerCreateInfo, 0, &texture->m_Sampler);
		VK_ASSERT(result, "Failed to create sampler");

		VkImageViewCreateInfo imageView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };

		imageView.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		imageView.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageView.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		imageView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		imageView.subresourceRange.layerCount = 6;
		imageView.subresourceRange.levelCount = 1;
		imageView.image = texture->m_ImageHdl;

		VkDevice device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		VK_ASSERT(vkCreateImageView(device, &imageView, 0, &texture->m_BaseImageView), "Failed to create image view!");

		texture->m_DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		texture->m_DescriptorImageInfo.sampler = texture->m_Sampler;
		texture->m_DescriptorImageInfo.imageView = texture->m_BaseImageView;

		return texture;
	}
	void VulkanTexture::CreateCubeMapTexture(TextureInfo& textureAssetData, TextureData& textureData)
	{

		if (textureAssetData.m_UsingMipMaps)
		{
			if (textureAssetData.mipCount == 0)
			{
				std::uint32_t maxDimen = std::max(textureAssetData.m_ExtentDimen.width, textureAssetData.m_ExtentDimen.height);

				while (maxDimen > 0)
				{
					maxDimen >>= 1;
					textureAssetData.mipCount++;
				}
			}

		}
		else
			textureAssetData.mipCount = 1;

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		CreateImageParams params{ textureAssetData.m_Format, VkImageTiling::VK_IMAGE_TILING_OPTIMAL, usage };
		m_ImageHdl = CreateCubeImage(textureAssetData.m_ExtentDimen, textureAssetData.mipCount, VK_SAMPLE_COUNT_1_BIT, params, m_Allocation);
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();

		VMABuffer staging{};
		staging.CreateStagingBuffer(textureAssetData.m_ImageSize, instance, textureAssetData.m_Data);

		std::vector<VkBufferImageCopy> CopyBuffers;
		std::uint32_t offset = 0;
		for (uint32_t face = 0; face < 6; face++)
		{
			for (uint32_t level = 0; level < textureAssetData.mipCount; level++)
			{
				const tinyddsloader::DDSFile::ImageData* imageData = textureData.m_TextureLoaded.GetImageData(level, face);
				if (imageData == nullptr)
				{
					TDS_ERROR("This sectioned is null for some reason... \n Double check your texture loading");
					continue;
				}
				VkBufferImageCopy& CopyBuffer = CopyBuffers.emplace_back();
				CopyBuffer.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				CopyBuffer.imageSubresource.mipLevel = level;
				CopyBuffer.imageSubresource.baseArrayLayer = face;
				CopyBuffer.imageSubresource.layerCount = 1;
				CopyBuffer.imageExtent.width = imageData->m_width;
				CopyBuffer.imageExtent.height = imageData->m_height;
				CopyBuffer.imageExtent.depth = 1;
				CopyBuffer.bufferOffset = offset;
				//This might be wrong.
				offset += imageData->m_memSlicePitch;
			}
		}



		CommandBufferInfo cmdInfo{};
		CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();
		cmdMgr.CreateSingleUseCommandBuffer(cmdInfo);

		ImageMemoryLayoutInput imageMemLayout{};
		imageMemLayout.m_Image = m_ImageHdl;
		imageMemLayout.m_subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemLayout.m_subResourceRange.baseMipLevel = 0;
		imageMemLayout.m_subResourceRange.levelCount = textureAssetData.mipCount;
		imageMemLayout.m_subResourceRange.layerCount = 6;
		imageMemLayout.m_subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemLayout.m_oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemLayout.m_NewLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		setTextureLayout(cmdInfo.m_CommandBuffer.m_CmdBuffer, imageMemLayout);

		vkCmdCopyBufferToImage(cmdInfo.m_CommandBuffer.m_CmdBuffer, staging.GetBuffer(), m_ImageHdl, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, std::uint32_t(CopyBuffers.size()), CopyBuffers.data());

		imageMemLayout.m_oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemLayout.m_NewLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		setTextureLayout(cmdInfo.m_CommandBuffer.m_CmdBuffer, imageMemLayout);

		m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		cmdMgr.EndExecution(cmdInfo);


		CreateSamplerViaCuteMap(textureAssetData);
		CreateImageViewViaCubeMap(textureAssetData);

		m_DescriptorImageInfo.imageLayout = m_ImageLayout;
		m_DescriptorImageInfo.imageView = m_BaseImageView;
		m_DescriptorImageInfo.sampler = m_Sampler;

		staging.DestroyBuffer();
	}
	/*
		This function will completely ignore mip maps.
		Usage is for fonts and basic plain color textures.
		For now this will be used.
	*/
	void VulkanTexture::CreateBasicTexture(TextureInfo& textureAssetData)
	{
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
		CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		CreateImageParams params{ textureAssetData.m_Format, VkImageTiling::VK_IMAGE_TILING_OPTIMAL, usage };
		params.m_Format = textureAssetData.m_Format;
		m_ImageHdl = CreateVulkanImage(textureAssetData.m_ExtentDimen,
			1, VK_SAMPLE_COUNT_1_BIT, params, m_Allocation);

		void* data = textureAssetData.m_Data;
		textureAssetData.m_UsingMipMaps = false;
		size_t size = textureAssetData.m_ImageSize;

		VMABuffer staging = VMABuffer::CreateStagingBuffer(size, instance, data);

		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;

		CommandBufferInfo info{};
		cmdMgr.CreateSingleUseCommandBuffer(info);
		ImageMemoryLayoutInput transferToDestination{ m_ImageHdl, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, subresourceRange };
		SetImageMemoryBarrier(info.m_CommandBuffer.m_CmdBuffer, transferToDestination);

		VkBufferImageCopy bufferCopyImage{};
		bufferCopyImage.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyImage.imageSubresource.mipLevel = 0;
		bufferCopyImage.imageSubresource.baseArrayLayer = 0;
		bufferCopyImage.imageSubresource.layerCount = 1;
		bufferCopyImage.imageExtent = { textureAssetData.m_ExtentDimen.width, textureAssetData.m_ExtentDimen.height, 1 };

		vkCmdCopyBufferToImage(info.m_CommandBuffer.m_CmdBuffer, staging.GetBuffer(), m_ImageHdl, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyImage);

		ImageMemoryLayoutInput transitionToFirstMip{ m_ImageHdl, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, subresourceRange };
		SetImageMemoryBarrier(info.m_CommandBuffer.m_CmdBuffer, transitionToFirstMip);
		cmdMgr.EndExecution(info);

		m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		CreateSampler(textureAssetData);
		CreateImageView(textureAssetData.m_Format);

		m_DescriptorImageInfo.imageLayout = m_ImageLayout;
		m_DescriptorImageInfo.imageView = m_BaseImageView;
		m_DescriptorImageInfo.sampler = m_Sampler;
		staging.DestroyBuffer();
	}

	void VulkanTexture::CreateSampler(TextureInfo& textureInfo)
	{
		VkDevice deviceRef = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		VkPhysicalDevice physical = GraphicsManager::getInstance().getVkInstance().getVkPhysicalDevice();

		VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = textureInfo.m_ImageFilter;
		samplerCreateInfo.minFilter = textureInfo.m_ImageFilter;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = textureInfo.m_SampleAddressMode;
		samplerCreateInfo.addressModeV = textureInfo.m_SampleAddressMode;
		samplerCreateInfo.addressModeW = textureInfo.m_SampleAddressMode;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerCreateInfo.borderColor = textureInfo.m_BorderClr;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = static_cast<float>(textureInfo.mipCount);
		//Check if we support anisotrophy
		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceFeatures(physical, &features);
		if (textureInfo.m_UseAnistrophy && features.samplerAnisotropy)
		{
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(physical, &properties);
			samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
			samplerCreateInfo.anisotropyEnable = VK_TRUE;
		}
		else
		{
			samplerCreateInfo.maxAnisotropy = 1.0f;
			samplerCreateInfo.anisotropyEnable = VK_FALSE;
		}
		VkResult result = vkCreateSampler(deviceRef, &samplerCreateInfo, 0, &m_Sampler);
		VK_ASSERT(result, "Failed to create sampler");
	}

	void VulkanTexture::CreateSamplerViaCuteMap(TextureInfo& textureInfo)
	{
		VkDevice deviceRef = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		VkPhysicalDevice physical = GraphicsManager::getInstance().getVkInstance().getVkPhysicalDevice();

		VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerCreateInfo.magFilter = textureInfo.m_ImageFilter;
		samplerCreateInfo.minFilter = textureInfo.m_ImageFilter;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = textureInfo.m_SampleAddressMode;
		samplerCreateInfo.addressModeV = textureInfo.m_SampleAddressMode;
		samplerCreateInfo.addressModeW = textureInfo.m_SampleAddressMode;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = static_cast<float>(textureInfo.mipCount);
		samplerCreateInfo.borderColor = textureInfo.m_BorderClr;
		//Check if we support anisotrophy

		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceFeatures(physical, &features);
		if (textureInfo.m_UseAnistrophy && features.samplerAnisotropy)
		{
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(physical, &properties);
			samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
			samplerCreateInfo.anisotropyEnable = VK_TRUE;
		}
		else
		{
			samplerCreateInfo.maxAnisotropy = 1.0f;
			samplerCreateInfo.anisotropyEnable = VK_FALSE;
		}
		VkResult result = vkCreateSampler(deviceRef, &samplerCreateInfo, 0, &m_Sampler);
		VK_ASSERT(result, "Failed to create sampler");
	}

	void VulkanTexture::CreateImageViewViaCubeMap(TextureInfo& textureInfo)
	{
		VkDevice deviceRef = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		viewCreateInfo.format = textureInfo.m_Format;
		viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 6;
		viewCreateInfo.subresourceRange.levelCount = textureInfo.mipCount;
		viewCreateInfo.image = m_ImageHdl;

		VkResult result = vkCreateImageView(deviceRef, &viewCreateInfo, 0, &m_BaseImageView);
		VK_ASSERT(result, "Failed to create texture view");
	}

	void VulkanTexture::CreateImageView(VkFormat format)
	{
		VkDevice deviceRef = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = format;
		viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.image = m_ImageHdl;

		VkResult result = vkCreateImageView(deviceRef, &viewCreateInfo, 0, &m_BaseImageView);
		VK_ASSERT(result, "Failed to create texture view");

	}

	void VulkanTexture::setTextureLayout(VkCommandBuffer& cmdBuffer, ImageMemoryLayoutInput layoutParam)
	{

		VkImageMemoryBarrier imageMemoryBarrier = {};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.oldLayout = layoutParam.m_oldLayout;
		imageMemoryBarrier.newLayout = layoutParam.m_NewLayout;
		imageMemoryBarrier.image = layoutParam.m_Image;
		imageMemoryBarrier.subresourceRange = layoutParam.m_subResourceRange;

		switch (layoutParam.m_oldLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			imageMemoryBarrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			break;
		}

		switch (layoutParam.m_NewLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			if (imageMemoryBarrier.srcAccessMask == 0)
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			break;
		}

		vkCmdPipelineBarrier(cmdBuffer, layoutParam.m_srcStageFlags, layoutParam.m_dstStageFlags,
			0, 0, 0, 0, 0, 1,
			&imageMemoryBarrier);

	}

	void VulkanTexture::Destroy()
	{

		VkDevice deviceRef = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		if (deviceRef == nullptr)
			return;

		if (m_BaseImageView)
			vkDestroyImageView(deviceRef, m_BaseImageView, 0);

		if (m_Sampler)
			vkDestroySampler(deviceRef, m_Sampler, 0);
		m_Sampler = 0;
		if (m_ImageHdl)
			GraphicsAllocator::FreeBuffer(m_ImageHdl, m_Allocation);
		m_ImageHdl = 0;
		m_Allocation = 0;


	}

	void VulkanTexture::SetImageMemoryBarrier(VkCommandBuffer& cmdbuffer, ImageMemoryLayoutInput& layout)
	{
		VkImageMemoryBarrier imageMemoryBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.srcAccessMask = layout.m_srcAccessFlags;
		imageMemoryBarrier.dstAccessMask = layout.m_dstAccessFlags;
		imageMemoryBarrier.oldLayout = layout.m_oldLayout;
		imageMemoryBarrier.newLayout = layout.m_NewLayout;
		imageMemoryBarrier.image = layout.m_Image;
		imageMemoryBarrier.subresourceRange = layout.m_subResourceRange;

		vkCmdPipelineBarrier(cmdbuffer, layout.m_srcStageFlags, layout.m_dstStageFlags, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	VkImage VulkanTexture::CreateVulkanImage(VkExtent2D extents, std::int32_t mipLevels, VkSampleCountFlagBits samplecntFlag, CreateImageParams& imageCreateParams, VmaAllocation& allocation, std::uint32_t NumOfArrayLayers)
	{

		VkImage newImageHdl{ nullptr };

		VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = imageCreateParams.m_Format;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = NumOfArrayLayers;
		imageCreateInfo.samples = samplecntFlag;
		imageCreateInfo.tiling = imageCreateParams.m_tiling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.extent = { extents.width, extents.height, 1 };
		imageCreateInfo.usage = imageCreateParams.m_usage;

		allocation = GraphicsAllocator::Allocate(&imageCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY, newImageHdl);
		return newImageHdl;
	}

	VkImage VulkanTexture::CreateCubeImage(VkExtent2D extents, std::int32_t mipLevels, VkSampleCountFlagBits samplecntFlag, CreateImageParams& imageCreateParams, VmaAllocation& allocation, std::uint32_t NumOfArrayLayers)
	{
		VkImage newImageHdl{ nullptr };

		VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = imageCreateParams.m_Format;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = NumOfArrayLayers;
		imageCreateInfo.samples = samplecntFlag;
		imageCreateInfo.tiling = imageCreateParams.m_tiling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.extent = { extents.width, extents.height, 1 };
		imageCreateInfo.usage = imageCreateParams.m_usage;
		imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		allocation = GraphicsAllocator::Allocate(&imageCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY, newImageHdl);
		return newImageHdl;
	}


	VkExtent2D VulkanTexture::GenMipsExtent(TextureData& data, std::uint32_t mip)
	{
		VkExtent2D out = { data.m_TextureLoaded.GetWidth(), data.m_TextureLoaded.GetHeight() };
		while (mip > 0)
		{
			out.width /= 2;
			out.height /= 2;
			--mip;
		}
		return out;
	}

	VkWriteDescriptorSet& VulkanTexture::getWriteSet()
	{
		return m_WriteSet;
	}

	VkDescriptorImageInfo& VulkanTexture::getInfo()
	{
		return m_DescriptorImageInfo;
	}

	VkImage& VulkanTexture::GetImage()
	{
		return m_ImageHdl;
	}


	DefaultTextures::DefaultTextures()
	{
	}

	DefaultTextures::~DefaultTextures()
	{
	}

	void DefaultTextures::Init()
	{
		m_DefaultTexture = VulkanTexture::CreateDefaultTexture();
		m_DefaultCubeTexture = VulkanTexture::CreateDefaultCubeTexture(true);
		m_DefaultStorage = std::make_shared<VulkanTexture>();
		TextureInfo info{};
		info.m_ExtentDimen = { 4,4 };
		info.m_StorageImage = false;
		info.m_Format = VK_FORMAT_R8G8B8A8_UNORM;
		info.m_ImageSize = 4 * 4 * 4;
		m_DefaultStorage->CreateTexture(info);
	}

	std::shared_ptr<VulkanTexture> DefaultTextures::GetDefaultTexture()
	{
		return DefaultTextures::GetInstance().m_DefaultTexture;
	}

	std::shared_ptr<VulkanTexture> DefaultTextures::GetDefaultCubeTexture()
	{
		return DefaultTextures::GetInstance().m_DefaultCubeTexture;
	}

	std::shared_ptr<VulkanTexture> DefaultTextures::GetDefaultStorage()
	{
		return DefaultTextures::GetInstance().m_DefaultStorage;
	}

	void DefaultTextures::DestroyDefaultTextures()
	{
		m_DefaultTexture->Destroy();
		m_DefaultCubeTexture->Destroy();
		m_DefaultStorage->Destroy();
	}

	DefaultTextures& DefaultTextures::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = std::make_shared<DefaultTextures>();

		return *m_Instance;
	}

}