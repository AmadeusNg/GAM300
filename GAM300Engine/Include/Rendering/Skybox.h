#pragma once
#include "vulkanTools/VulkanPipeline.h"
#include "MathCommon.h"
#include "GraphicsResource/TextureInfo.h"
namespace TDS
{

	class StaticSkyBoxRenderer
	{
		private:
			Texture							m_SkyBoxTexture;
			std::shared_ptr<VulkanPipeline> m_SkyBoxPipeline;
			std::vector<Vec3>				m_SkylinePositions;
		public:
			void Init();
			void RenderSkyBox(VkCommandBuffer commandBuffer);



	};

}