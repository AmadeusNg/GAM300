#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM

#include "vulkanTools/vulkanInstance.h"
#include "vulkanTools/VulkanPipeline.h"
#include "components/ParticleComponent.h"
#include "dotnet/ImportExport.h"
#include "components/transform.h"
#include "renderPass.h"
#include "dotnet/ImportExport.h"

namespace TDS {


	class ParticleSystem {
	public:
		struct InputRenderBuffers {
			std::shared_ptr<VMABuffer> m_VertexBuffer = nullptr;
			std::shared_ptr<VMABuffer> m_IndexBuffer = nullptr;
		};

		struct CameraUBO {
			Mat4 view;
			Mat4 Proj;
		};


		DLL_API ParticleSystem();
		DLL_API ~ParticleSystem();

		ParticleSystem(const ParticleSystem&) = delete;
		ParticleSystem& operator=(const ParticleSystem&) = delete;

		DLL_API static void Init();
		//sends data into compute shader for calculations
		DLL_API static void UpdateSystem(const float deltatime, const std::vector<EntityID>& Entities, Transform* Xform, Particle_Component* Particles);
		//Spawn/Draws all particles from all entities
		DLL_API void Render(const float deltatime, const std::vector<EntityID>& Entities, Transform* Xform, Particle_Component* EmitterList);
		DLL_API void ShutDown();

		//helper functions
		
		inline static VkCommandBuffer m_cmdbuffer;
		//for compute
		inline static std::shared_ptr<VulkanPipeline> m_ComputePipeline;
		inline static std::shared_ptr<VulkanPipeline> m_EmitterPipeline;
		//for rendering
		inline static std::shared_ptr<VulkanPipeline> m_RenderPipeline;
		inline static std::array<InputRenderBuffers, ParticleMesh::MAX_MESHES>  MeshRenderBuffers;
	private:

		//std::shared_ptr<VMABuffer> m_ComputeBuffer;
		//std::shared_ptr<VMABuffer> m_EmitterBuffer;

	};


}

#endif // !PARTICLESYSTEM

