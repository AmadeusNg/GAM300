#pragma once
#include "Shader/ShaderData.h"
#include "CommandManager.h"
#include "BufferInfo.h"
#include "GraphicsResource/GraphicsResourceDefines.h"
#include "VertexTypes.h"
#include "dotnet/ImportExport.h"

namespace TDS
{
	struct PipelineConfig
	{
		VkBlendOp			m_ColorBlend{ VkBlendOp::VK_BLEND_OP_ADD },
			m_AlphaBlend{ VkBlendOp::VK_BLEND_OP_ADD };

		VkBlendFactor		m_SrcClrBlend{ VkBlendFactor::VK_BLEND_FACTOR_ZERO },
			m_DstClrBlend{ VkBlendFactor::VK_BLEND_FACTOR_ZERO },
			m_SrcAlphaBlend{ VkBlendFactor::VK_BLEND_FACTOR_ZERO },
			m_DstAlphaBlend{ VkBlendFactor::VK_BLEND_FACTOR_ZERO };

		VkCullModeFlags		m_CullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
		VkPolygonMode		m_PolygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;

		float				m_MinDepth{ 0.f },
			m_MaxDepth{ 1.f };

		bool				m_EnableDepthTest{ true },
			m_EnableDepthWrite{ true },
			m_EnableDepthBiased{ false },
			m_EnablePrimitiveRestart{ false };

		std::vector<VkPrimitiveTopology>			m_PipelineDrawModes = { VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };

	};

	struct VulkanPipelineDescriptor
	{
		std::unordered_map<std::string, std::uint32_t>							m_LocalBufferNames;
		std::unordered_map<std::uint32_t, std::vector<std::shared_ptr<UBO>>>	m_UpdateBufferFrames;
		//This one is for texture
		std::unordered_map<std::uint32_t, VkWriteDescriptorSet>					m_WriteSetFrames;
		std::vector<VkDescriptorSet>											m_DescriptorSets;
		std::uint32_t															descContainerSize = 0;
		VkDescriptorSetLayout													m_DescSetLayout{};
		VkDescriptorImageInfo													m_ImageInfo{};
	};

	struct ShaderInputs
	{
		//The shaders u want to use for this pipeline
		std::map<SHADER_FLAG, std::string>			m_Shaders;

		std::map<std::uint32_t, BufferInfo>			m_StaticBuffers;
		std::vector<VertexBufferInfo>				m_InputVertex;
		std::int32_t								m_StageCnt = -1;
	};

	struct ShaderLoadedData
	{
		std::vector<VkPushConstantRange>					m_VkPushConstantRanges;
		std::vector<VkPipelineShaderStageCreateInfo>		m_VkPipelineShaderStages;
		std::unordered_map<SHADER_FLAG, VkShaderModule>     m_ShaderModules;
	};

	class FrameBuffer;
	struct PipelineCreateEntry
	{
		PipelineConfig								m_PipelineConfig;
		ShaderInputs								m_ShaderInputs;

		std::vector<std::shared_ptr<FrameBuffer>>	m_FBTarget;
		std::int32_t								m_NumDescriptorSets = 1;

		std::string									m_PipelineName = "";
		bool										m_EnableDoubleBuffering = true;
		bool										m_UseSwapchain = false;
	};

	class VulkanTexture;

	class DLL_API VulkanPipeline
	{
	public:
		VulkanPipeline();
		~VulkanPipeline();

		bool									Create(PipelineCreateEntry& createEntry);
		void									GeneratePipeline(VkPrimitiveTopology drawMode);
		void									SetClearColor(iColor clearColor);

		void									StartRenderPass();
		void									EndRenderPass();
		void									ShutDown();

		bool									LoadCachedPieline(std::string_view fileName, VkPrimitiveTopology drawMode);
		bool									SavePipelineCache(std::string_view fileName, VkPrimitiveTopology drawMode);

		void									CreateDescriptors(ShaderInputs& shaderInputs, std::uint32_t numDescriptorSets);

		void									Draw(VMABuffer& vertexBuffer, std::uint32_t frameIndex = 0);
		void									DrawIndexed(VMABuffer& vertexBuffer, VMABuffer& indexBuffer, std::uint32_t frameIndex = 0);
		void									DrawInstanced(VMABuffer& vertexBuffer, std::uint32_t instance = 1, std::uint32_t frameIndex = 0);
		void									DrawInstancedIndexed(VMABuffer& vertexBuffer, VMABuffer& indexBuffer, std::uint32_t instance = 1, std::uint32_t frameIndex = 0);
		void									SubmitPushConstant(void* data, size_t size, SHADER_FLAG shaderStage);
		void									UpdateUBO(void* data, size_t size, std::uint32_t binding, std::uint32_t frameIndex = 0, std::uint32_t offset = 0);
		void									UpdateTextureArray(std::uint32_t binding, VkDescriptorType descriptorType, std::vector<std::shared_ptr<VulkanTexture>>& texture);
		void									UpdateTexture(std::uint32_t binding, VkDescriptorType descriptorType, std::shared_ptr<VulkanTexture>& texture);

		void									BindPipeline(VkPrimitiveTopology drawMode = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		void									BindDescriptor(std::int32_t DescIndex = 0);
		void									BindVertexBuffer(VMABuffer& vertexBuffer);
		void									BindIndexBuffer(VMABuffer& IndexBuffer);
		void									LoadShader(std::string_view shaderPath, SHADER_FLAG shaderFlag);

		VkPipeline& GetPipeline(VkPrimitiveTopology drawMode);
		VkPipelineLayout& GetLayout();
		bool									IsBlendEnabled();
		std::uint32_t							GetBufferBinding(std::string_view bufferName);
		std::uint32_t							GetTextureBinding(std::string_view textureBinding);
		VulkanPipelineDescriptor& GetPipelineDescriptor();
		PipelineCreateEntry& GetCreateEntry();

		void									SetFlipViewport(bool condition);
		bool									IsFlipViewport() const;
		void									SetCommandBuffer(VkCommandBuffer& buffer);
		//Descriptors
		void									FreeDescriptors();
		void									CreateDescriptorSet(ShaderInputs& shader, VulkanPipelineDescriptor& descriptor);
		void									CreateUniformBuffers(ShaderInputs& shader, VulkanPipelineDescriptor& descriptor);
		void									CreateSamplerDescriptors(ShaderInputs& shader, VulkanPipelineDescriptor& descriptor);
		void									UpdateDescriptor(VkDescriptorImageInfo& imageInfo, VkDescriptorType type, std::uint32_t bindingPoint);
		VkDescriptorSetLayout					GetLayout(std::uint32_t index = 0) const;
		const std::vector<VkDescriptorSet>& GetDescriptorSets(std::uint32_t index = 0) const;

	private:


		bool									m_BlendingEnabled = false;
		bool									m_Pipeline = false;
		bool									m_FlipViewport = false;
		bool									m_UsingMainCommand = false;
		std::uint32_t							m_CurrentFBIndex = 0;
		std::uint32_t							m_CurrentFBAttachmentIndex = 0;
		PipelineCreateEntry						m_PipelineEntry{};
		ShaderLoadedData						m_ShaderLoadedData{};
		VkPrimitiveTopology						m_CurrentPrimitiveMode = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkPipelineLayout						m_PipelineLayout = nullptr;
		VkRenderPass							m_RenderTarget = nullptr;
		VkCommandBuffer							m_CommandBuffer = nullptr;
		VkDescriptorPool						m_DescriptorPool = nullptr;
		CommandBufferInfo						m_CommandBufferInfo{};

		VulkanPipelineDescriptor				m_PipelineDescriptor;
		std::unordered_map<VkPrimitiveTopology, VkPipeline>	m_Pipelines;
		std::unordered_map<VkPrimitiveTopology, VkPipelineCache> m_Caches;

		void									DestroyModules();

	public:
		//I might set this manually now
		std::vector<VkVertexInputBindingDescription> m_inputBindings;
		std::vector<VkVertexInputAttributeDescription> m_InputAttributes;

		static void									GenerateMatrixInputAttribute(std::vector<VkVertexInputAttributeDescription>& inputAttris,
			std::uint32_t& prevOffset, std::uint32_t& location, std::uint32_t& binding, VertexBufferElement& inputVertexInfo);
		static VkVertexInputAttributeDescription	GenerateVectorInputAttribute(std::uint32_t& location, std::uint32_t& binding, VertexBufferElement& inputVertexInfo);
	};


}