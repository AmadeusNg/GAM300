#pragma once
#include "FactoryBase.h"
#include "../GraphicsResource/TextureInfo.h"
#define MAX_NUM_PRELOAD_TEXTURE 10
#define TEXTURE_PATH "../assets/textures/"
namespace TDS
{

	template<>
	class AssetFactory<Texture>
	{

	private:
		std::unordered_map<std::string, std::int32_t> m_LoadedTexture;
		std::vector<std::uint64_t> m_LoadedTextureGUID;
	public:

		std::uint64_t GetTextureGUID(std::string_view modelName)
		{
			return m_LoadedTextureGUID[m_LoadedTexture[modelName.data()]];
		}
		void Preload(ResourceManager& resourceMgr)
		{
			std::uint32_t NumOfLoadedTexture = 0;
			std::filesystem::path dir = TEXTURE_PATH;

			if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
			{
				std::cout << "Invalid directory" << std::endl;
				return;
			}

			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				if (NumOfLoadedTexture >= MAX_NUM_PRELOAD_TEXTURE)
					break;
				const std::filesystem::path& path = entry.path();
				if (path.extension() == ".dds")
				{
					SingleTypeReference<Texture> TextureInstance{};

					std::string fileName = path.filename().string();
					TextureInstance.m_Identifier.GetTypeName<Texture>();
					TextureInstance.m_Identifier.CreateTypeIDByName(fileName);
					TextureInstance.m_GUID = UniqueID::CreateUniqueGuid();

					auto newTexture = resourceMgr.getResource(TextureInstance);
					if (newTexture == nullptr)
					{
						std::cout << "Failed to get texture instance!" << std::endl;
						continue;
					}
					newTexture->LoadTexture(path.string());
					m_LoadedTexture[fileName] = m_LoadedTextureGUID.size();
					m_LoadedTextureGUID.emplace_back(TextureInstance.m_GUID.GetUniqueID());
					++NumOfLoadedTexture;
				}
			}
		}
		static void Load(std::string_view path, SingleTypeReference<Texture>& textureRef, ResourceManager& resourceMgr)
		{
			auto texRef = resourceMgr.getResource(textureRef);
			if (texRef == nullptr)
			{
				std::cout << "Failed to get texture instance!" << std::endl;
				return;
			}
			texRef->LoadTexture(path);
		}

	};



}