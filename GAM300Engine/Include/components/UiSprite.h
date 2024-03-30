#pragma once
#include "ecs/ecs.h"
#include "ResourceManagement/ResourceRef.h"
#include "GraphicsResource/FontInfo.h"
namespace TDS
{

	class Texture;
	class UISprite : public IComponent
	{
	public:
		DLL_API UISprite();
		DLL_API UISprite(UISprite&& toMove) noexcept;
		DLL_API ~UISprite() = default;
		inline TypeReference<Texture>& GetReference()
		{
			return m_TextureReference;
		}
		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND
	private:
		TypeReference<Texture>			m_TextureReference;
		TypeReference<FontAtlas>		m_FontReference{};
	public:
		bool							m_IsDirty = false;
		bool							m_EnableSprite = true;
		bool							m_IsFont = false;
		int								m_LayerID = -1;
		Vec4							m_Color;
		std::string						m_TextureName = "";
		Vec2							m_BoundingBoxMin = { -0.5f,-0.5f };
		Vec2							m_BoundingBoxMax = { 0.5f, 0.5f };
		Mat4							m_TransformMatrix = Mat4::identity();


		void	DLL_API					SetLayerID(int layerID);
		void	DLL_API					SetColour(Vec4 color);
		void	DLL_API					SetColourViaFloat(float x, float y, float z, float w);
		bool	DLL_API					IsMouseCollided();
		void	DLL_API					SetTextureName(std::string stringName);
		std::string	DLL_API				GetTextureName();

		bool	DLL_API					IsFont();
		void	DLL_API					SetIsFont(bool condition);
		void	DLL_API					SetFontName(std::string fontName);
		void	DLL_API					SetFontMessage(std::string msg);
		void	DLL_API					SetFontColour(Vec4 Color);
		void	DLL_API					SetFontColourViaFloat(float x, float y, float z, float w);
		void	DLL_API					SetFontBackgroundColour(Vec4 Color);
		void	DLL_API					SetFontBackgroundColourViaFloat(float x, float y, float z, float w);
		void	DLL_API					SetEnableSprite(bool status);

	public:
		//If is font data
		inline TypeReference<FontAtlas>& GetFontReference()
		{
			return m_FontReference;
		}
		std::string						m_Message = "Hello Testing";
		std::string						m_FontName = "";
		Vec4							m_BackGroundColour = { 0.f, 0.f, 0.f, 0.f };
		Vec4							m_ForeGroundColour = { 0.f, 0.f, 0.f, 0.f };

		
	};
	DLL_API UISprite* GetUISprite(EntityID entityID);
}