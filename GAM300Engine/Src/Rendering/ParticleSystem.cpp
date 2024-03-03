#include "Rendering/ParticleSystem.h"
#include "vulkanTools/GlobalBufferPool.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"


namespace TDS {



	ParticleSystem::ParticleSystem() {
		Init();
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

		m_ComputePipeline->Create(ParticleComputeEntry);


		//rendering pipeline
		m_RenderPipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry ParticleRenderEntry;
		ParticleRenderEntry.m_NumDescriptorSets = 1;
		ParticleRenderEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleRenderEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleRenderEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleRenderEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleRenderEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/ParticleVert.spv"));
		ParticleRenderEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/ParticleFrag.spv"));

		for (auto buffers : MeshRenderBuffers) {
			buffers.m_IndexBuffer = std::make_shared<VMABuffer>();
			buffers.m_VertexBuffer = std::make_shared<VMABuffer>();
		}

		m_RenderPipeline->Create(ParticleRenderEntry);


	}

	void ParticleSystem::Update(const float deltatime, const std::vector<EntityID>& Entities, Transform* Xform, Particle_Component* EmitterList) {
		UpdateSystem(deltatime, Entities, Xform, EmitterList);
		Render();
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
			ParticleEmitter currentEmitter = EmitterList[i].GetEmitter();
			currentEmitter.spawntimer += deltatime;

			unsigned int SpawnAmt = currentEmitter.spawntimer / currentEmitter.spawninterval;

			if (SpawnAmt <= 0)
				continue;

			m_EmitterPipeline->UpdateUBO(&currentEmitter, sizeof(ParticleEmitter), 1, currentframe);
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
		for (unsigned int i{ 0 }; i < Entities.size(); ++i) {
			ParticleEmitter currentEmitter = EmitterList[i].GetEmitter();
			//bind ssbos?

			int numwrkgrp = (currentEmitter.maxparticles + 128 - 1) / 128;
			m_ComputePipeline->DispatchCompute(numwrkgrp, 1, 1);
		}
	}

	void ParticleSystem::Render() {
		//send data into vertex and fragment shader to render into scene


	}

	void ParticleSystem::ShutDown() {
		m_ComputePipeline->ShutDown();
		m_ComputeBuffer->DestroyBuffer();
		delete m_RenderPass;
		delete m_RenderTarget;
		delete m_FrameBuffer;
	}

	void ParticleSystem::UpdateEmitter(float deltatime, EntityID ID, Particle_Component* Emitter) {
		std::uint32_t currentparticlecount = Emitter->GetCurrentParticleCount();
		std::uint32_t Desiredparticlecount = Emitter->GetDesiredParticleCount();
		if (currentparticlecount != Desiredparticlecount) {
			AddParticlestoEmitter(Emitter, Desiredparticlecount - currentparticlecount, ID);
		}
		//if amount is equal, run the update loop for the particle activity
		else {
			for (auto& particle : Emitter->GetParticleVector()) {
				if (particle.isActive) {
					particle.Age += deltatime;
					if (particle.Age >= particle.Lifetime) {
						particle.Age = 0.f;
						particle.isActive = false;
						continue;
					}
					particle.Velocity += particle.Acceleration * deltatime;//updating velocity with acceleration
					particle.Acceleration *= std::exp(-Emitter->GetDecayRate() * deltatime);//decaying acceleration per frame
					particle.Position += particle.Velocity * deltatime;//updaing particle position
				}
			}
		}
	}

	void ParticleSystem::AddParticlestoEmitter(Particle_Component* Emitter, std::uint32_t particleamount, EntityID ID) {
		Particle newparticle = Particle();
		Transform* EntityXform = ecs.getComponent<Transform>(ID);
		newparticle.Position = EntityXform->GetPosition();
		for (size_t i{ 0 }; i < particleamount; ++i) {
			Emitter->GetParticleVector().push_back(newparticle);

		}
	}
}