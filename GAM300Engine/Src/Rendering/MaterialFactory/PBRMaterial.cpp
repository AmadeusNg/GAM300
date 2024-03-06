#include "Rendering/MaterialFactory/PBRMaterial.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/vulkanSwapChain.h"
#include "vulkanTools/vulkanDevice.h"
#include "vulkanTools/Renderer.h"
#include "vulkanTools/VulkanTexture.h"
#include "vulkanTools/GraphicsAllocator.h"
#include "vulkanTools/CommandManager.h"
namespace TDS
{
	PBRMaterial::PBRMaterial() :m_Attachments()
	{
	}
	PBRMaterial::~PBRMaterial()
	{
	}
	void PBRMaterial::Init()
	{
	}
	void PBRMaterial::Destroy()
	{
	}
	void PBRMaterial::CreateBRDFLUT(VkDescriptorImageInfo& m_BRDFLUTImageInfo)
	{
		

	}
	void PBRMaterial::CreateIrradianceCube(VulkanTexture& cubeTexture, VkDescriptorImageInfo& m_BRDFLUTImageInfo)
	{
		

		

	}
	void PBRMaterial::CreatePreFiltered(VulkanTexture& cubeTexture, VkDescriptorImageInfo& m_BRDFLUTImageInfo)
	{
		
	}
}