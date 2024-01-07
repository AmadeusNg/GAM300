#include "Rendering/RendererSystem.h"
#include "components/components.h"
#include "Rendering/RenderDataManager.h"
#include "Rendering/Renderer3D.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/VulkanPipeline.h"
#include "vulkanTools/vulkanSwapChain.h"
#include "vulkanTools/VulkanTexture.h"
#include "vulkanTools/Renderer.h"
namespace TDS
{
	GlobalUBO ubo{};

	struct PushConstantData {
		Mat4 ModelMat{ 1.f };
		Mat4 NormalMat{ 1.f };
		unsigned int textureIndex = 499;
		float padding[3];
		uint32_t Id{};

	};

	float RendererSystem::lightPosX = 0.f;
	void RendererSystem::OnInit()
	{

	}
	void RendererSystem::OnUpdate(const float dt, const std::vector<EntityID>& entities, Transform* _TransformComponent, GraphicsComponent* _Graphics)
	{


		std::uint32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		VkCommandBuffer commandBuffer = GraphicsManager::getInstance().getCommandBuffer();
		GraphicsManager::getInstance().m_PointLightRenderer->newupdate(ubo, entities, _TransformComponent, _Graphics);
		//std::cout << ubo.m_activelights << std::endl;
		for (size_t i = 0; i < entities.size(); ++i)
		{
			if (!ecs.getEntityIsEnabled(entities[i]) || !ecs.getComponentIsEnabled<GraphicsComponent>(entities[i]))
			{
				continue;
			}

			if (GraphicsManager::getInstance().IsViewingFrom2D())
			{
				if (_Graphics[i].m_UsedIn2D == false)
					continue;
			}

			PushConstantData pushData{};
			pushData.Id = entities[i];
			if (_Graphics[i].m_ModelName != _Graphics[i].m_AssetReference.m_AssetName)
			{
				AssetModel* pModel = AssetManager::GetInstance()->GetModelFactory().GetModel(_Graphics[i].m_ModelName, _Graphics[i].m_AssetReference);

				if (pModel == nullptr)
				{
					//TDS_WARN("No such model called {}", _Graphics[i].m_AssetReference.m_AssetName);
				}
				else
				{
					_Graphics[i].m_AssetReference.m_AssetName = _Graphics[i].m_ModelName;
					_Graphics[i].m_AssetReference.m_ResourcePtr = pModel;
				}

			}
			std::string texName = _Graphics[i].m_TextureName;
			int textureID = AssetManager::GetInstance()->GetTextureFactory().GetTextureIndex(_Graphics[i].m_TextureName, _Graphics[i].m_TextureReference);

			if (textureID == -1)
			{
				pushData.textureIndex = 499;
			}
			else
			{
				pushData.textureIndex = textureID;
			}

			Renderer3D::getPipeline()->SetCommandBuffer(commandBuffer);
			GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().SetCommandBuffer(commandBuffer);
			//GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().SetCommandBuffer(commandBuffer);


			if (Renderer3D::getPipeline()->GetCreateEntry().m_EnableDoubleBuffering)
			{
				if (Vec3 Scale = _TransformComponent[i].GetScale(); Scale.x <= 0.f || Scale.y <= 0.f || Scale.z <= 0.f) {
				}
				else {
					_TransformComponent[i].GenerateTransform();
				}

				Mat4 temp = _TransformComponent[i].GetTransformMatrix();
				pushData.ModelMat = temp;
				temp.inverse();
				temp.transpose();
				pushData.NormalMat = temp;

				ubo.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
				ubo.m_Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
					GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);
				ubo.m_Projection.m[1][1] *= -1;

				if (_Graphics[i].IsPointLight())
				{
					GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().BindDescriptor(frame, 1);
					GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().UpdateUBO(&ubo, sizeof(GlobalUBO), 1, frame);
					GraphicsManager::getInstance().m_PointLightRenderer->render(&_Graphics[i], &_TransformComponent[i]);
				}
				else {//if not point light render using model
					if (_Graphics[i].m_AssetReference.m_ResourcePtr != nullptr)
					{

						auto& ModelReference = _Graphics[i].m_AssetReference;

						//Means we did not split any mesh and the whole model is one mesh
						if (ModelReference.m_ResourcePtr->m_Meshes.size() == 1)
						{
							if (ModelReference.m_ResourcePtr->BufferIsNull())
								ModelReference.m_ResourcePtr->CreateBuffers();


						}
						else
						{
							MeshData* ptr = nullptr;
							auto findItr = ModelReference.m_ResourcePtr->m_Meshes.find(_Graphics[i].m_MeshName);
							if (findItr != ModelReference.m_ResourcePtr->m_Meshes.end())
							{
								ptr = &findItr->second;
								ModelReference.m_ResourcePtr->m_CurrMeshData = ptr;
								if (ModelReference.m_ResourcePtr->m_CurrMeshData->BufferIsNull())
									ModelReference.m_ResourcePtr->m_CurrMeshData->CreateBuffers();
							}
							else
							{
								findItr = ModelReference.m_ResourcePtr->m_Meshes.begin();
								ptr = &findItr->second;
								ModelReference.m_ResourcePtr->m_CurrMeshData = ptr;
								if (ModelReference.m_ResourcePtr->m_CurrMeshData->BufferIsNull())
									ModelReference.m_ResourcePtr->m_CurrMeshData->CreateBuffers();
							}
						}



						Renderer3D::getPipeline()->BindPipeline();

						if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray3D)
						{
							Renderer3D::getPipeline()->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
							AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray3D = false;
						}



						Renderer3D::getPipeline()->UpdateUBO(&ubo, sizeof(GlobalUBO), 0, frame);

						Renderer3D::getPipeline()->SubmitPushConstant(&pushData, sizeof(PushConstantData), SHADER_FLAG::VERTEX | SHADER_FLAG::FRAGMENT);
						Renderer3D::getPipeline()->BindVertexBuffer(*_Graphics[i].m_AssetReference.m_ResourcePtr->GetVertexBuffer());
						Renderer3D::getPipeline()->BindIndexBuffer(*_Graphics[i].m_AssetReference.m_ResourcePtr->GetIndexBuffer());
						Renderer3D::getPipeline()->BindDescriptor(frame, 1);
						Renderer3D::getPipeline()->BindArrayDescriptorSet(0, 1, 1);
						if (_Graphics[i].ShowMesh())
						{
							Renderer3D::getPipeline()->DrawIndexed(*_Graphics[i].m_AssetReference.m_ResourcePtr->GetVertexBuffer(),
								*_Graphics[i].m_AssetReference.m_ResourcePtr->GetIndexBuffer(),
								frame);
						}
						/*if (_Graphics[i].IsDebugOn()) {
							GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().BindDescriptor(frame, 1);
							GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().UpdateUBO(&ubo, sizeof(GlobalUBO), 0, frame);
							GraphicsManager::getInstance().m_DebugRenderer->Render(&_Graphics[i], &_TransformComponent[i]);
							GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().DrawIndexed(*_Graphics[i].m_AssetReference.m_ResourcePtr->GetVertexBuffer(),
								*_Graphics[i].m_AssetReference.m_ResourcePtr->GetIndexBuffer(),
								frame);
						}*/
					}
				}

			}
		}
		//GraphicsManager::getInstance().m_DebugRenderer->Render(commandBuffer,frame,ubo, entities, _Graphics, _TransformComponent);

	}
	void RendererSystem::OnRender(const float dt, const std::vector<EntityID>& entities, GraphicsComponent* _Graphics)
	{
	}

}