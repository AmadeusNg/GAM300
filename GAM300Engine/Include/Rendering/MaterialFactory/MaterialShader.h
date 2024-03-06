#pragma once
#include "GraphicsResource/MaterialInfo.h"
#include "vulkanTools/VMATypes/VMABuffer.h"
namespace TDS
{

	class VulkanPipeline;
	class VulkanTexture;

	struct MaterialAttachment
	{
		VkImage						m_Image = nullptr;
		VkImageView					m_ImageView = nullptr;
		VkSampler					m_Sampler = nullptr;
		VmaAllocation				m_Allocation = nullptr;
		
	};


	class MaterialShader
	{
		protected:
			SHADING_TYPE					m_ShadingType;
			std::shared_ptr<VulkanPipeline> m_Pipeline;
		public:
			MaterialShader(){}
			virtual			~MaterialShader(){}
			virtual void	Init() = 0;
			virtual void	Destroy() = 0;
			inline std::shared_ptr<VulkanPipeline> GetPipeline()
			{
				return m_Pipeline;
			}

			
	};


	

}