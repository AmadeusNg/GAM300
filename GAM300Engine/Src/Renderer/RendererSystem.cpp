#include "Rendering/RendererSystem.h"
#include "components/components.h"
#include "Rendering/RenderDataManager.h"
#include "Rendering/Renderer3D.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/VulkanPipeline.h"
#include "vulkanTools/vulkanSwapChain.h"
#include "vulkanTools/Renderer.h"
namespace TDS
{
	struct PushConstantData {
		Mat4 ModelMat{ 1.f };
		Mat4 NormalMat{ 1.f };
	};
	float RendererSystem::lightPosX = 0.f;
	void RendererSystem::OnInit()
	{
		Renderer3D::getInstance()->models = nullptr;
	
	}
	void RendererSystem::OnUpdate(const float dt, const std::vector<EntityID>& entities, Transform* _TransformComponent, GraphicsComponent* _Graphics)
	{
		std::uint32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();

		VkCommandBuffer commandBuffer = GraphicsManager::getInstance().getCommandBuffer();
		//GraphicsManager::getInstance().GetSwapchainRenderer().BeginSwapChainRenderPass(commandBuffer);
		for (size_t i = 0; i < entities.size(); ++i)
		{
			Renderer3D::getPipeline()->SetCommandBuffer(commandBuffer);
			_Graphics[i].GetAsset().m_GUID = *assetManager->GetModelFactory().m_LoadedModelsGUID.begin();
			_Graphics[i].GetAsset().m_Identifier.CreateTypeIDByName(assetManager->GetModelFactory().m_ModelMap.begin()->first);
			_Graphics[i].GetAsset().m_Identifier.GetTypeName<AssetModel>();

			assetManager->getResourceManager().getResource(_Graphics[i].GetAsset());

			if (_Graphics[i].ShowMesh() == true)
			{
				if (Renderer3D::getPipeline()->GetCreateEntry().m_EnableDoubleBuffering)
				{
					
					GlobalUBO ubo = RendererDataManager::GetUBO(_Graphics->GetAsset().m_GUID.GetUniqueID());
					ModelElement elem = RendererDataManager::GetModelElement(_Graphics->GetAsset().m_GUID.GetUniqueID(), _Graphics->GetAsset().m_Reference);
					PushConstantData pushData{};
					Mat4 temp{ 1.0f, 0.f, 0.f, 0.f,
							0.f, 1.f, 0.f, 0.f,
							0.f, 0.f, 1.f, 0.f,
							0.f, 0.f, 0.f, 1.f };


					Vec3 Position = _TransformComponent[i].GetPosition();
					temp = temp.Translate(Position);
					pushData.ModelMat = temp;

					temp.transpose();
					temp.inverse();
					pushData.NormalMat = temp;

					ubo.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
					ubo.m_vPointLights[0].m_Position= Vec4(lightPosX, 1.f, 0.f, 0.2f);
					ubo.m_vPointLights[0].m_Color = Vec4(1.f, 1.f, 1.f, 1.f);
					ubo.m_Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
						GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 10.f);
					ubo.m_Projection.m[1][1] *= -1;
					
					Renderer3D::getPipeline()->BindPipeline();
					Renderer3D::getPipeline()->UpdateUBO(&ubo, sizeof(GlobalUBO), 0, frame);
					Renderer3D::getPipeline()->BindDescriptor(frame);
					Renderer3D::getPipeline()->SubmitPushConstant(&pushData, sizeof(PushConstantData), SHADER_FLAG::VERTEX);
					/*Renderer3D::getInstance()->models->bind(commandBuffer);*/
					Renderer3D::getInstance()->models->draw(commandBuffer);

					
					
					
					

					

				}


			}
		}
		//GraphicsManager::getInstance().GetSwapchainRenderer().EndSwapChainRenderPass(commandBuffer);
		

	/*	OnRender(entities, &_transform, _Graphics)*/
		/*SubmitModels(entities, &_transform, _Graphics);*/
	/*	Renderer3D::DrawFrame(*RendererDataManager::GetInstance());*/
	}
	void RendererSystem::OnRender(const float dt, const std::vector<EntityID>& entities, GraphicsComponent* _Graphics)
	{
	}
	void RendererSystem::SubmitPointLights()
	{
	}
	void RendererSystem::SubmitModels(const std::vector<EntityID>& entities, Transform* _transform, GraphicsComponent* _Graphics)
	{
		
		
	}
	void RendererSystem::SubmitTextures()
	{
	}
}