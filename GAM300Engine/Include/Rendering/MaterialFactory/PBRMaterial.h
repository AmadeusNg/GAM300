#pragma once
#include "Rendering/MaterialFactory/MaterialShader.h"

namespace TDS
{
	enum class PBRAttachmentType
	{
		BRDFLUT, IRRADIANCE, PREFILTERED
	};



	class PBRMaterial final : public MaterialShader
	{
	public:
		PBRMaterial();
		~PBRMaterial();
		virtual void Init();
		virtual void Destroy();
		void	CreateBRDFLUT(VkDescriptorImageInfo& m_BRDFLUTImageInfo);
		void    CreateIrradianceCube(VulkanTexture& cubeTexture, VkDescriptorImageInfo& m_BRDFLUTImageInfo);
		void	CreatePreFiltered(VulkanTexture& cubeTexture, VkDescriptorImageInfo& m_BRDFLUTImageInfo);

	private:
		std::array<MaterialAttachment, 3>	m_Attachments;

	};


}