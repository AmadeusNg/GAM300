#include "Rendering/RenderDataManager.h"
#include "GraphicsResource/AssetModel.h"
#include "Rendering/GraphicsManager.h"
#include "components/transform.h"
namespace TDS
{
	RendererDataManager::RendererDataManager()
	{
	}
	RendererDataManager::~RendererDataManager()
	{
	}
	void RendererDataManager::BeginSubmit()
	{
	}
	void RendererDataManager::Destroy()
	{
		GetInstance()->m_ModelToRender.clear();
		GetInstance()->m_UBO.clear();
	}
	ModelElement& RendererDataManager::GetModelElement(std::uint32_t guid, AssetModel* model)
	{
		RendererDataManager& inst = *GetInstance();
		auto findItr = inst.m_ModelToRender.find(guid);
		if (findItr == inst.m_ModelToRender.end())
		{
			inst.m_ModelToRender[guid].m_VertexBuffer = std::make_shared<VMABuffer>();
			inst.m_ModelToRender[guid].m_IndexBuffer = std::make_shared<VMABuffer>();
			inst.m_ModelToRender[guid].m_VertexBuffer->CreateVertexBuffer(model->GetVertexData().size() * sizeof(VertexData), true, model->GetVertexData().data());
			inst.m_ModelToRender[guid].m_IndexBuffer->CreateIndexBuffer(model->GetIndexData().size(), true, model->GetIndexData().data());
			inst.m_ModelToRender[guid].m_VertexBuffer->SetDataCnt(std::uint32_t(model->GetVertexData().size()));
			inst.m_ModelToRender[guid].m_IndexBuffer->SetDataCnt(std::uint32_t(model->GetIndexData().size()));
			inst.m_ModelToRender[guid].m_MaterialIndex = 0;
			return inst.m_ModelToRender[guid];
		}
		return findItr->second;

	}
	GlobalUBO RendererDataManager::GetUBO(std::uint32_t guid)
	{
		RendererDataManager& inst = *GetInstance();
		return inst.m_UBO[guid];
	}
	//void RendererDataManager::SubmitMesh(std::uint32_t guid, Transform* trasform, AssetModel* model, float Light)
	//{
	//	if (!trasform || !model)
	//	{
	//		return;
	//	}
	//	RendererDataManager& inst = *GetInstance();
	//	auto findItr = inst.m_ModelToRender.find(guid);
	//	if (findItr == inst.m_ModelToRender.end())
	//	{
	//		inst.m_ModelToRender[guid].m_VertexBuffer = std::make_shared<VMABuffer>();
	//		inst.m_ModelToRender[guid].m_VertexBuffer->CreateVertexBuffer(model->GetVertexData().size(), false, model->GetVertexData().data());
	//		inst.m_ModelToRender[guid].m_IndexBuffer->CreateIndexBuffer(model->GetIndexData().size(), false, model->GetIndexData().data());
	//		inst.m_ModelToRender[guid].m_MaterialIndex = 0;//Todo: integrate this soon.
	//	}

	//	inst.m_UBO[guid].m_Transform.Translate(trasform->GetPosition());
	//	inst.m_UBO[guid].m_Transform.Rotate(Vec3(0.f, 0.f, 0.f), trasform->GetRotation());
	//	inst.m_UBO[guid].m_Transform.Scale(trasform->GetScale());
	//	inst.m_UBO[guid].m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
	//	inst.m_UBO[guid].m_vPointLights[0].m_Position= Vec4(Light, 0.5f, 0.f, 0.2f);
	//	inst.m_UBO[guid].m_vPointLights[0].m_Color = Vec4(1.f, 1.f, 1.f, 1.f);
	//}

	std::shared_ptr<RendererDataManager> RendererDataManager::GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance = std::make_shared<RendererDataManager>();
		}
		return m_Instance;
	}
}