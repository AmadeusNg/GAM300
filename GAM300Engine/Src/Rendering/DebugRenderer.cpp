#include "Rendering/DebugRenderer.h"
#include "GraphicsResource/AssetModel.h"
#include "Physics/CollisionSystem.h"
#include "GraphicsResource/VertexInfo.h"

namespace TDS {


	void DebugBatch::AddtoBatch(Transform* Xform, std::uint32_t entity) {
		(entity);
		m_vDebugModel[m_InstanceCnt].TransformMatrix = Xform->GetTransformMatrix();
		m_vDebugModel[m_InstanceCnt].DebugColor = { 0.f,1.f,0.f,1.f };
		++m_InstanceCnt;
	}

	void DebugBatch::PrepareBatch() {
		if (m_InstanceCnt != m_vCPU2GPU.size())
			m_vCPU2GPU.resize(m_InstanceCnt);
		for (std::uint32_t i{ 0 }; i < m_InstanceCnt; ++i) {
			auto& instanceinfo = m_vDebugModel[i];

			m_vCPU2GPU[i].DebugColor = instanceinfo.DebugColor;
			m_vCPU2GPU[i].TransformMatrix = instanceinfo.TransformMatrix;
		}
	}


	void DebugBatch::Update(std::unique_ptr<VulkanPipeline>& Pipeline, VkCommandBuffer cmdbuffer, int frame, GlobalUBO& ubo) {
		Pipeline->SetCommandBuffer(cmdbuffer);
		Pipeline->BindPipeline(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
		m_UBO.Proj = ubo.m_Projection;
		m_UBO.View = ubo.m_View;
		Pipeline->UpdateUBO(m_vCPU2GPU.data(), sizeof(DebugPushConstants) * m_InstanceCnt, 11, frame);
	}

	void DebugBatch::Draw(std::unique_ptr<VulkanPipeline>& Pipeline, VkCommandBuffer cmdbuffer, int frame, VMABuffer& VertexBuffer, VMABuffer& IndexBuffer) {
		Pipeline->SubmitPushConstant(&m_UBO, sizeof(DebugSceneUBO), SHADER_FLAG::VERTEX);
		Pipeline->BindVertexBuffer(VertexBuffer);
		Pipeline->BindIndexBuffer(IndexBuffer);
		Pipeline->BindDescriptor(frame, 1);
		Pipeline->BindArrayDescriptorSet(0, 1, 1);
		Pipeline->DrawInstancedIndexed(VertexBuffer, IndexBuffer, m_InstanceCnt);
	}

	DebugRenderer::DebugRenderer(VulkanInstance& Instance) : m_Instance(Instance) {
		CollisionSystem::CollisionSystemInit();
		PipelineCreateEntry PipelineEntry;
		PipelineEntry.m_NumDescriptorSets = 1;
		PipelineEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/DebugVert.spv"));
		PipelineEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/DebugFrag.spv"));
		PipelineEntry.m_PipelineConfig.m_EnableDepthTest = true;
		PipelineEntry.m_PipelineConfig.m_EnableDepthWrite = true;
		PipelineEntry.m_PipelineConfig.m_PipelineDrawModes = { VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST };
		VertexLayout layout = VertexLayout(
			{
			  VertexBufferElement(VAR_TYPE::VEC3, "vPosition")
			});
		m_CubeVertexBuffer = std::make_shared<VMABuffer>();
		m_CubeIndexBuffer = std::make_shared<VMABuffer>();
		m_CubeVertexBuffer->CreateVertexBuffer(CollisionSystem::cubeVertices.size() * sizeof(Vec3), true, CollisionSystem::cubeVertices.data());
		m_CubeIndexBuffer->CreateIndexBuffer(CollisionSystem::cubeIndices.size() * sizeof(unsigned int), true, CollisionSystem::cubeIndices.data());
		//m_CapsuleIndexBuffer->CreateIndexBuffer(CollisionSystem::capsuleIndices.size() * sizeof(unsigned int), true, CollisionSystem::capsuleIndices.data());
		//m_SphereIndexBuffer->CreateIndexBuffer(CollisionSystem::sphereIndices.size() * sizeof(unsigned int), true, CollisionSystem::sphereIndices.data());
		PipelineEntry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(Vec3)));
		m_Pipeline = std::make_unique <VulkanPipeline>();
		m_Pipeline->Create(PipelineEntry);
		//m_CapsuleVertexBuffer->CreateVertexBuffer();
		//m_SphereVertexBuffer->CreateVertexBuffer();
	}

	DebugRenderer::~DebugRenderer()
	{
	}

	void DebugRenderer::Render(VkCommandBuffer cmdbuffer, int frame, GlobalUBO& ubo, const std::vector<EntityID>& Entities, GraphicsComponent* Gp, Transform* Transform) {
		std::array<DebugBatch, 3> vDebugBatches;
		
		for (size_t i = 0; i < Entities.size(); ++i) {
			if (!Gp[i].IsDebugOn())
				continue;
			BoxCollider* Box = ecs.getComponent<BoxCollider>(Entities[i]);
			SphereCollider* Sphere = ecs.getComponent<SphereCollider>(Entities[i]);
			CapsuleCollider* Capsule = ecs.getComponent<CapsuleCollider>(Entities[i]);

			//check for collider type and add to relevant batch
			if (Box) {
				vDebugBatches[CUBE].AddtoBatch(&Transform[i], Entities[i]);
			}
			if (Capsule) {
				vDebugBatches[CAPSULE].AddtoBatch(&Transform[i], Entities[i]);
			}
			if (Sphere) {
				vDebugBatches[SPHERE].AddtoBatch(&Transform[i], Entities[i]);
			}

		}
		vDebugBatches[CUBE].PrepareBatch();
		vDebugBatches[CAPSULE].PrepareBatch();
		vDebugBatches[SPHERE].PrepareBatch();
		//update and render here

		for (int i{ 0 }; i < vDebugBatches.size(); ++i) {
			//bind vertex and index buffer from wending collision system
			switch (i)
			{
			case 0:
			{
				vDebugBatches[CUBE].Update(m_Pipeline, cmdbuffer, frame, ubo);
				vDebugBatches[CUBE].Draw(m_Pipeline, cmdbuffer, frame, *m_CubeVertexBuffer, *m_CubeIndexBuffer);
			}
			default:
				break;
			}
		}


		vDebugBatches[CUBE].Clear();
		vDebugBatches[CAPSULE].Clear();
		vDebugBatches[SPHERE].Clear();
		//
		/*m_Pipeline->BindPipeline(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
		DebugPushConstants Pushdata;
		Pushdata.TransformMatrix = Transform->GetOffsetMatrix();
		Pushdata.DebugColor = { 0.f,1.f,0.f,1.f };
		m_Pipeline->SubmitPushConstant(&Pushdata, sizeof(DebugPushConstants), SHADER_FLAG::VERTEX | SHADER_FLAG::FRAGMENT);

		m_Pipeline->BindVertexBuffer(*Gp->GetAsset().m_ResourcePtr->GetVertexBuffer());
		m_Pipeline->BindIndexBuffer(*Gp->GetAsset().m_ResourcePtr->GetIndexBuffer());*/
	}
	VulkanPipeline& DebugRenderer::GetPipeline() {
		return *m_Pipeline;
	}
}