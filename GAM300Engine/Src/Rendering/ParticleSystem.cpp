#include "Rendering/ParticleSystem.h"
#include "vulkanTools/GlobalBufferPool.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Rendering/Revamped/DeferredController.h"


namespace TDS {



	ParticleSystem::ParticleSystem() {
	}

	ParticleSystem::~ParticleSystem() {

	}

	void ParticleSystem::Init() {
		//create vulkan stuff to create
		m_cmdbuffer = GraphicsManager::getInstance().getCommandBuffer();

		//create a spawn pipeline here
		m_EmitterPipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry EmitterComputeEntry;
		EmitterComputeEntry.m_NumDescriptorSets = 1;
		EmitterComputeEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		EmitterComputeEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		EmitterComputeEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		EmitterComputeEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		EmitterComputeEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::COMPUTE_SHADER, "../assets/shaders/ParticleEmitter.spv"));

		GlobalBufferPool::GetInstance()->AddToGlobalPool(1000 * sizeof(Particle), 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, "v_ParticleOut");
		GlobalBufferPool::GetInstance()->AddToGlobalPool(1001 * sizeof(int), 2, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, "FreeList");

		m_EmitterPipeline->Create(EmitterComputeEntry);

		//compute pipeline
		m_ComputePipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry ParticleComputeEntry;
		ParticleComputeEntry.m_NumDescriptorSets = 1;
		ParticleComputeEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::COMPUTE_SHADER, "../assets/shaders/ParticleCompute.spv"));

		GlobalBufferPool::GetInstance()->AddToGlobalPool(1000 * sizeof(Mat4), 3, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, "v_TransformMatrix");

		m_ComputePipeline->Create(ParticleComputeEntry);


		//rendering pipeline
		m_RenderPipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry ParticleRenderEntry;
		ParticleRenderEntry.m_NumDescriptorSets = 1;
		ParticleRenderEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleRenderEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleRenderEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleRenderEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleRenderEntry.m_FBTarget = GraphicsManager::getInstance().GetDeferredController()->GetFrameBuffer(RENDER_PASS::RENDER_COMPOSITION);
		ParticleRenderEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/RenderParticleVert.spv"));
		ParticleRenderEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/RenderParticleFrag.spv"));

		VertexLayout layout = VertexLayout({
			VertexBufferElement(VAR_TYPE::VEC2, "in_Position")
			});
		ParticleRenderEntry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(Vec2)));
		TypeReference<MeshController> instance;

		MeshRenderBuffers[CUBE].m_IndexBuffer = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("cube_Bin.bin", instance)->GetMeshBuffer()->m_IndexBuffer;
		MeshRenderBuffers[CUBE].m_VertexBuffer = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("cube_Bin.bin", instance)->GetMeshBuffer()->m_VertexBuffer;

		MeshRenderBuffers[SPHERE].m_IndexBuffer = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("sphere_Bin.bin", instance)->GetMeshBuffer()->m_IndexBuffer;
		MeshRenderBuffers[SPHERE].m_VertexBuffer = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("sphere_Bin.bin", instance)->GetMeshBuffer()->m_VertexBuffer;

		MeshRenderBuffers[CAPSULE].m_IndexBuffer = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("capsule_Bin.bin", instance)->GetMeshBuffer()->m_IndexBuffer;
		MeshRenderBuffers[CAPSULE].m_VertexBuffer = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("capsule_Bin.bin", instance)->GetMeshBuffer()->m_VertexBuffer;
		

		m_RenderPipeline->Create(ParticleRenderEntry);


	}



	void ParticleSystem::UpdateSystem(const float deltatime, const std::vector<EntityID>& Entities, Transform* Xform, Particle_Component* EmitterList) {
		//send data into compute shader for calculations
		/*
		* loop through all entities with the particle component and run the emitter computeshader
		*/
		uint32_t currentframe = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		m_EmitterPipeline->SetCommandBuffer(m_cmdbuffer);
		m_EmitterPipeline->BindPipeline();
		for (unsigned int i{ 0 }; i < Entities.size(); ++i) {
			Particle_Component currentEmitter = EmitterList[i];
			currentEmitter.spawntimer += deltatime;

			unsigned int SpawnAmt = currentEmitter.GetSpawnTimer() / currentEmitter.GetSpawnInterval();

			if (SpawnAmt <= 0)
				continue;
			
			Particle_Emitter_PushData GPUPush = { SpawnAmt, currentEmitter.GetEmitter() };
			m_EmitterPipeline->UpdateUBO(&GPUPush, sizeof(Particle_Emitter_PushData), 3, currentframe);
			//bind ssbos?


			int numwrkgrp = (SpawnAmt + 64 - 1) / 64;
			m_EmitterPipeline->DispatchCompute(numwrkgrp, 1, 1);
		}

		//ensure all emiiter updated
		VkMemoryBarrier memBarrier{};
		memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(m_cmdbuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0, 1, &memBarrier, 0, nullptr, 0, nullptr);

		//compute particles
		m_ComputePipeline->SetCommandBuffer(m_cmdbuffer);
		m_ComputePipeline->BindPipeline();
		float dt = TimeStep::GetDeltaTime();
		for (unsigned int i{ 0 }; i < Entities.size(); ++i) {
			Particle_Component& currentEmitter = EmitterList[i];
			//bind ssbos?
			m_ComputePipeline->UpdateUBO(&dt, sizeof(float), 4, currentframe);
			int numwrkgrp = (currentEmitter.GetMaxParticles() + 128 - 1) / 128;
			m_ComputePipeline->DispatchCompute(numwrkgrp, 1, 1);
		}
	}

	void ParticleSystem::Render(const float deltatime, const std::vector<EntityID>& Entities, Transform* Xform, Particle_Component* EmitterList) {
		uint32_t currentframe = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		TDSCamera cam = GraphicsManager::getInstance().GetCamera();
		Mat4 view = cam.GetViewMatrix();
		Mat4 proj = Mat4::Perspective(cam.m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);
		//send data into vertex and fragment shader to render into scene
		m_RenderPipeline->SetCommandBuffer(m_cmdbuffer);
		m_RenderPipeline->BindPipeline();
		CameraUBO temp = { view, proj };
		m_RenderPipeline->UpdateUBO(&temp, sizeof(CameraUBO), 2, currentframe);
		for (unsigned int i{ 0 }; i < Entities.size(); ++i) {
			Particle_Component currentEmitter = EmitterList[i];
			unsigned int SpawnAmt = currentEmitter.GetSpawnTimer() / currentEmitter.GetSpawnInterval();
			
			m_RenderPipeline->BindVertexBuffer(*MeshRenderBuffers[EmitterList[i].GetMeshType()].m_VertexBuffer);
			m_RenderPipeline->BindIndexBuffer(*MeshRenderBuffers[EmitterList[i].GetMeshType()].m_IndexBuffer);



			m_RenderPipeline->DrawInstancedIndexed(*MeshRenderBuffers[EmitterList[i].GetMeshType()].m_VertexBuffer, *MeshRenderBuffers[EmitterList[i].GetMeshType()].m_IndexBuffer, SpawnAmt, currentframe);
		}
	}

	void ParticleSystem::ShutDown() {
		m_ComputePipeline->ShutDown();
		m_EmitterPipeline->ShutDown();
		m_RenderPipeline->ShutDown();
	}
}