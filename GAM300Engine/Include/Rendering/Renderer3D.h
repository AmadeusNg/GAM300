#pragma once
#include "MathCommon.h"
namespace TDS
{
	class Model;
	class RendererDataManager;
	class VulkanPipeline;
	class FrameBuffer;
	class Renderer3D
	{
	private:
		inline static std::shared_ptr<Renderer3D> m_Instance = nullptr;
		std::shared_ptr<VulkanPipeline> m_DefaultPipeline = nullptr;
		std::shared_ptr<FrameBuffer>	m_FrameBuffer = nullptr;

	public:
		static std::shared_ptr<Renderer3D> getInstance();
		static void Init();
		static void Update(RendererDataManager& renderData);
		static void UpdateUniform(RendererDataManager& renderData);
		static void DrawFrame(RendererDataManager& renderData);
		static std::shared_ptr<VulkanPipeline>& getPipeline();
		void ShutDown();



	};
}