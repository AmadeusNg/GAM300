#pragma once
#include "MathCommon.h"
#include "vulkanTools/FrameInfo.h"
#include "ModelElement.h"
#include "dotnet/ImportExport.h"
namespace TDS
{
	class Material;



	class Transform;
	class AssetModel;
	class DLL_API RendererDataManager
	{
	public:
		RendererDataManager();
		~RendererDataManager();

		static void BeginSubmit();
		static void Destroy();
		static ModelElement& GetModelElement(std::uint32_t guid, AssetModel* model);
		/*static void SubmitMesh(std::uint32_t guid, Transform* trasform, AssetModel* model, float LightPos);*/
		static GlobalUBO GetUBO(std::uint32_t guid);

		/*static void EndSubmit();*/

		static std::shared_ptr<RendererDataManager> GetInstance();


		std::uint32_t m_UpdateIndex = 0;
		inline static std::shared_ptr<RendererDataManager> m_Instance = nullptr;
		std::map<std::uint32_t, ModelElement> m_ModelToRender;
		std::map<std::uint32_t, GlobalUBO> m_UBO;



	};
}