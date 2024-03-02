#include "Rendering/ParticleSystem.h"

namespace TDS {



	ParticleSystem::ParticleSystem() {
		Init();
	}

	ParticleSystem::~ParticleSystem() {

	}

	void ParticleSystem::Init() {
		//create vulkan stuff to create
		//crreate a spawn pipeline here
		m_EmitterPipeline = std::make_shared<VulkanPipeline>();
		PipelineCreateEntry ParticleComputeEntry;
		ParticleComputeEntry.m_NumDescriptorSets = 1;
		ParticleComputeEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::COMPUTE_SHADER, "../assets/shaders/ParticleEmitter.spv"));


		m_ComputePipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry ParticleComputeEntry;
		ParticleComputeEntry.m_NumDescriptorSets = 1;
		ParticleComputeEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::COMPUTE_SHADER, "../assets/shaders/ParticleCompute.spv"));


		m_ComputeBuffer = std::make_shared<VMABuffer>();
		m_ComputeBuffer->CreateBuffer(sizeof(TDS::Particle) * m_AllActiveParticles.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU, &m_AllActiveParticles);

		m_ComputePipeline->Create(ParticleComputeEntry);

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

	void ParticleSystem::UpdateSystem(VkCommandBuffer commandbuffer, std::uint32_t frameindex, float deltatime, std::vector<EntityID>& Entities, Particle_Component* EmitterList, Transform* Xform) {
		//send data into compute shader for calculations
		/*
		* loop through all entities with the particle component and run the emitter and compute code
		*/


		//take data from compute and prep for vertex


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