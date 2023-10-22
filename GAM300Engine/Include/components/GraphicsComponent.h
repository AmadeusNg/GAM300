#pragma once
#include "ecs/ecs.h"
#include "ResourceManagement/ResourceManager.h"
#include "vulkanTools/VulkanPipeline.h"
namespace TDS
{
	class AssetModel;
	class Texture;

	class GraphicsComponent : public IComponent
	{
	public:
		DLL_API GraphicsComponent();
		DLL_API GraphicsComponent(GraphicsComponent&& toMove) noexcept;
		DLL_API ~GraphicsComponent() = default;
		DLL_API virtual bool Deserialize(const rapidjson::Value& obj);
		DLL_API virtual bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const;

		inline void ToggleRender(bool condition)
		{
			m_ShowMesh = condition;
		}
		inline bool ShowMesh()
		{
			return m_ShowMesh;
		}
		inline SingleTypeReference<AssetModel>& GetAsset()
		{
			return m_AssetReference;
		}
		inline SingleTypeReference<Texture>& GetTexture()
		{
			return m_TextureReference;
		}
		struct PointLightComponent {
			float lightintensity{ 1.f };
		};
		bool Pointlight{ false };
	private:
		SingleTypeReference<AssetModel> m_AssetReference;
		SingleTypeReference<Texture>	m_TextureReference;
		bool m_ShowMesh = true;
		

	};
}