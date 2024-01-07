#ifndef DEBUG_RENDERER
#define DEBUG_RENDERER
#include "vulkanTools/vulkanInstance.h"
#include "dotnet/ImportExport.h"
#include "components/components.h"
#include "vulkanTools/VulkanPipeline.h"
#include "vulkanTools/FrameInfo.h"
namespace TDS {
	struct DebugPushConstants {
		Mat4 TransformMatrix;
		Vec4 DebugColor;
	};
	enum DebugModel {
		CUBE = 0,
		SPHERE,
		CAPSULE
	};

	struct DebugSceneUBO {
		Mat4 Proj;
		Mat4 View;
	};

	struct Batch3D {
		std::uint32_t m_InstanceCnt = 0;
		void Clear() {
			m_InstanceCnt = 0;
		}
		virtual void AddtoBatch(Transform* Xform, std::uint32_t entity) = 0;
		virtual void PrepareBatch() = 0;
	};
	struct DebugBatch : Batch3D {
		std::array<DebugPushConstants, 5000> m_vDebugModel;
		std::vector<DebugPushConstants> m_vCPU2GPU;
		DebugSceneUBO m_UBO;
		virtual void DLL_API AddtoBatch(Transform* Xform, std::uint32_t entity) override;
		virtual void DLL_API PrepareBatch() override;
		void Update(std::unique_ptr<VulkanPipeline>& Pipeline, VkCommandBuffer cmdbuffer, int frame, GlobalUBO& ubo);
		void Draw(std::unique_ptr<VulkanPipeline>& Pipeline, VkCommandBuffer cmdbuffer, int frame, VMABuffer& VertexVuffer, VMABuffer& IndexBuffer);

	};
	class DebugRenderer {
	public:
		DLL_API DebugRenderer(VulkanInstance& Instance);
		DLL_API ~DebugRenderer();
		DebugRenderer(const DebugRenderer&) = delete;
		DebugRenderer& operator=(const DebugRenderer&) = delete;
		DLL_API void Render(VkCommandBuffer cmdbuffer,int frame,GlobalUBO& ubo, const std::vector<EntityID>& Entities, GraphicsComponent* Gp, Transform* Transform);
		DLL_API VulkanPipeline& GetPipeline();
	private:
		VulkanInstance& m_Instance;
		std::unique_ptr<VulkanPipeline> m_Pipeline;
		std::shared_ptr<VMABuffer> m_CubeVertexBuffer = nullptr;
		std::shared_ptr<VMABuffer> m_CubeIndexBuffer = nullptr;
		std::shared_ptr<VMABuffer> m_CapsuleVertexBuffer = nullptr;
		std::shared_ptr<VMABuffer> m_CapsuleIndexBuffer = nullptr;
		std::shared_ptr<VMABuffer> m_SphereVertexBuffer = nullptr;
		std::shared_ptr<VMABuffer> m_SphereIndexBuffer = nullptr;
	};
}
#endif // !DEBUG_RENDERER