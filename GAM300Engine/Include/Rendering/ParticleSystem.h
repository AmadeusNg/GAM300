#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM

#include "vulkanTools/vulkanInstance.h"
#include "vulkanTools/VulkanPipeline.h"
#include "components/ParticleComponent.h"
#include "dotnet/ImportExport.h"
#include "components/transform.h"
#include "renderPass.h"

namespace TDS {


	class ParticleSystem {
	public:
		struct InputRenderBuffers {
			std::shared_ptr<VMABuffer> m_VertexBuffer = nullptr;
			std::shared_ptr<VMABuffer> m_IndexBuffer = nullptr;
		};


		ParticleSystem();
		~ParticleSystem();

		ParticleSystem(const ParticleSystem&) = delete;
		ParticleSystem& operator=(const ParticleSystem&) = delete;

		void Init();
		//sends data into compute shader for calculations
		void UpdateSystem(const float deltatime, const std::vector<EntityID>& Entities, Transform* Xform, Particle_Component* Particles);
		//Spawn/Draws all particles from all entities
		void Render();
		void ShutDown();

		//helper functions
		//updates the all particles tied to a single entity
		void UpdateEmitter(float deltatime, EntityID ID, Particle_Component* Emitter);
		void AddParticlestoEmitter(Particle_Component* Emitter, std::uint32_t particleamount, EntityID id);
	private:
		VkCommandBuffer m_cmdbuffer;

		//for compute
		std::shared_ptr<VulkanPipeline> m_ComputePipeline;
		std::shared_ptr<VulkanPipeline> m_EmitterPipeline;
		std::shared_ptr<VMABuffer> m_ComputeBuffer;
		std::shared_ptr<VMABuffer> m_EmitterBuffer;

		//for rendering
		RenderPass* m_RenderPass{};
		RenderTarget* m_RenderTarget{};
		FrameBuffer* m_FrameBuffer{};
		std::shared_ptr<VulkanPipeline> m_RenderPipeline;
		std::array<InputRenderBuffers, ParticleMesh::MAX_MESHES>  MeshRenderBuffers;
	};


}

#endif // !PARTICLESYSTEM

