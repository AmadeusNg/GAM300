#pragma once
#include "FactoryBase.h"
#include <string_view>
#include <vector>
#include <filesystem>
#include <fstream>
#include "GraphicsResource/FontInfo.h"
#include <iostream>

#define FONT_PATH "../assets/Fonts/"
#define MAX_NUM_PRELOAD_FONT 500
namespace TDS
{
	template<>
	class AssetFactory<FontAtlas>
	{
	public:

		std::array<Texture, 1000> m_TextureAtlas;
		std::array<FontAtlas, 1000> m_FontAtlasInfo;
		std::unordered_map<std::string, std::uint32_t> m_FontIndices;
		std::unordered_map<std::string, std::uint32_t> m_InstanceCnt;
		std::uint32_t m_CurrentIndex = 0;
		bool m_UpdateFontTextures = false;

		std::array<Texture, 1000>& GetFontAtlasTextures()
		{
			return m_TextureAtlas;
		}
		FontAtlas& GetFontAtlas(int index)
		{
			
			return m_FontAtlasInfo[index];
			
			
		}
		int GetFontAtlasIndex(std::string_view fontName, TypeReference<FontAtlas>& typeRef)
		{
			auto prevItr = m_InstanceCnt.find(typeRef.m_AssetName);
			if (prevItr != m_InstanceCnt.end())
			{
				if (prevItr->second >= 1)
				{
					--prevItr->second;
				}
			}
			
			auto itr = m_FontIndices.find(fontName.data());
			if (itr != m_FontIndices.end())
			{
				++m_InstanceCnt[fontName.data()];
				return (int)itr->second;
			}
			return -1;
		}
		void DestroyAllFonts()
		{
			for (auto& fontTexture : m_TextureAtlas)
			{
				fontTexture.Destroy();
			}
		}
		void Preload()
		{
			std::uint32_t NumOfLoadedFonts = 0;
			std::filesystem::path dir = FONT_PATH;

			if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
			{
				std::cout << "Invalid directory" << std::endl;
				return;
			}

			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				bool loadedFont = false, loadedAtlasInfo = false;
				if (NumOfLoadedFonts >= MAX_NUM_PRELOAD_FONT)
					break;
				std::string fontTexture{};
				std::string fontAtlasInfo{};
				if (entry.is_directory())
				{
					for (const auto& file : std::filesystem::directory_iterator(entry.path()))
					{
						const std::filesystem::path& path = file.path();
						if (path.extension() == ".dds")
						{
							fontTexture = path.string();
							loadedFont = true;
							
						}
						else if (path.extension() == ".json")
						{
							fontAtlasInfo = path.string();
							loadedAtlasInfo = true;
						}

					}



					if (loadedFont && loadedAtlasInfo)
					{
						std::string AtlasName = std::filesystem::path(fontAtlasInfo).filename().string();
						std::string AtlasTextureName = std::filesystem::path(fontTexture).filename().string();

						m_TextureAtlas[m_CurrentIndex].LoadTexture(fontTexture);
						m_FontAtlasInfo[m_CurrentIndex].DeserializeFontAtlas(fontAtlasInfo);
						m_FontIndices[AtlasTextureName] = m_CurrentIndex++;

					}

				}
			}
			m_UpdateFontTextures = true;
		}
		//Give me the folder of the font, NOT THE INDIVIDUAL FILES!
		void Load(std::string_view path)
		{

			std::filesystem::path fontDir(path.data());
			bool loadedFont{ false }, loadedAtlasInfo{ false };
			std::string FontTexturePath{};
			std::string FontAtlasInfoPath{};

			for (const auto& entry : std::filesystem::directory_iterator(fontDir))
			{
				const std::filesystem::path& path = entry.path();
				if (path.extension() == ".dds")
				{
					FontTexturePath = path.string();
					loadedFont = true;

				}
				if (path.extension() == ".json")
				{
					FontAtlasInfoPath = path.string();

					loadedAtlasInfo = true;
				}
			}




			if (loadedAtlasInfo == true && loadedFont == true)
			{
				std::string FontTextureName = std::filesystem::path(FontTexturePath).filename().string();
				auto itrFind = m_FontIndices.find(FontTextureName);
				if (itrFind != m_FontIndices.end())
				{
					if (m_InstanceCnt[FontTextureName] == 0)
					{
						m_TextureAtlas[itrFind->second].Destroy();
						m_TextureAtlas[itrFind->second].LoadTexture(FontTexturePath);
						m_FontAtlasInfo[itrFind->second].DeserializeFontAtlas(FontAtlasInfoPath);
						m_UpdateFontTextures = true;
						return;
					}
					else
					{
						TDS_INFO("There is still {} entities using {} texture", m_InstanceCnt[FontTextureName], FontTextureName);
						
					}
					return;
				}

				m_TextureAtlas[m_CurrentIndex].LoadTexture(FontTexturePath);
				m_FontAtlasInfo[m_CurrentIndex].DeserializeFontAtlas(FontAtlasInfoPath);

				m_FontIndices[FontTextureName] = m_CurrentIndex++;
				m_UpdateFontTextures = true;

			}
			else
				TDS_WARN("Failed to load font {}", fontDir.string().c_str());
		}
		void Load(std::string_view path, TypeReference<FontAtlas>& fontRef)
		{
			std::filesystem::path fontDir(path.data());
			bool loadedFont{ false }, loadedAtlasInfo{ false };
			std::string FontTexturePath{};
			std::string FontAtlasInfoPath{};
			
			for (const auto& entry : std::filesystem::directory_iterator(fontDir))
			{
				const std::filesystem::path& path = entry.path();
				if (path.extension() == ".dds")
				{
					FontTexturePath = path.string();
					loadedFont = true;

				}
				if (path.extension() == ".json")
				{
					FontAtlasInfoPath = path.string();
					
					loadedAtlasInfo = true;
				}
			}

			if (loadedAtlasInfo == true && loadedFont == true)
			{
				m_TextureAtlas[m_CurrentIndex].LoadTexture(FontTexturePath);
				m_FontAtlasInfo[m_CurrentIndex].DeserializeFontAtlas(FontAtlasInfoPath);
				std::string FontTextureName = std::filesystem::path(FontAtlasInfoPath).filename().string();
				fontRef.m_ResourcePtr = &m_FontAtlasInfo[m_CurrentIndex];
				fontRef.m_AssetName = FontTextureName;
				m_FontIndices[FontTextureName] = m_CurrentIndex++;
				m_UpdateFontTextures = true;
				
			}
			else
				TDS_WARN("Failed to load font {}", fontDir.string().c_str());
			

		}


	};

}