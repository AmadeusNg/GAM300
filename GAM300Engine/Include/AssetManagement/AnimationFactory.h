#pragma once
#include "AssetManagement/FactoryBase.h"
#include "Animation/AnimationLoader.h"
#define ANIMATION_PATH "../assets/animations/"
#define MAX_PRELOAD_ANIMATIONS 1000
namespace TDS
{





	template <>
	class AssetFactory<AnimationData>
	{
		static constexpr int MAX_ANIMATIONS = 21;

	private:
		std::array<AnimationData, MAX_ANIMATIONS>						m_AnimationData;
		std::unordered_map<std::string, std::uint32_t>					m_AnimationIndices;
		std::unordered_map<std::string, std::uint32_t>					m_ReferenceCnt;
		std::uint32_t													m_CurrentIndex = 0;

	public:


		std::unordered_map<std::string, std::uint32_t>& GetReferenceCounts()
		{
			return m_ReferenceCnt;
		}

		std::array<AnimationData, MAX_ANIMATIONS>& GetAnimationArray()
		{
			return m_AnimationData;
		}
		void LoadNewAnimationPack(std::string_view path)
		{
			std::filesystem::path FilePath(path);
			std::string fileName = FilePath.filename().string();

			auto& CurrAnimationData = m_AnimationData[m_CurrentIndex];

			AnimationData::Serialize(CurrAnimationData, fileName, true);

			m_AnimationIndices[fileName] = m_CurrentIndex++;


		}



		std::string LoadAnimationPack(std::string_view path)
		{
			std::filesystem::path FilePath(path);
			std::string fileName = FilePath.filename().string();


			auto itr = m_AnimationIndices.find(fileName);
			if (itr != m_AnimationIndices.end())
			{

				int instance = m_ReferenceCnt[fileName];
				int index = itr->second;
				auto& CurrAnimPack = m_AnimationData[m_AnimationIndices[fileName]];


				if (instance == 0)
				{
					LoadNewAnimationPack(path);

					return fileName;
				}
				else
				{
					TDS_INFO("This AnimationPack is being used by {} entities, load failure", instance);
					return std::string();
				}


			}
			else
			{

				LoadNewAnimationPack(path);

				return fileName;

			}
			return std::string();
		}

		AnimationData* GetAnimationData(std::string_view modelName, TypeReference<AnimationData>& asset)
		{
			auto itr = m_AnimationIndices.find(modelName.data());
			if (itr != m_AnimationIndices.end())
			{
				UnloadReference(asset);

				asset.m_AssetName = modelName;
				asset.m_ResourcePtr = &m_AnimationData[itr->second];
				++m_ReferenceCnt[modelName.data()];
				return asset.m_ResourcePtr;

			}
			return nullptr;
			TDS_WARN("Model doesnt exist!");
		}

		void Preload()
		{
			std::filesystem::path dir = ANIMATION_PATH;

			if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
			{
				std::cout << "Invalid directory" << std::endl;
				return;
			}
			std::uint32_t numPreLoadedModels = 0;
			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				if (numPreLoadedModels >= MAX_PRELOAD_ANIMATIONS)
					break;


				const std::filesystem::path& path = entry.path();

				if (path.extension() == ".json")
				{
					std::string pathInString = entry.path().string();
					LoadAnimationPack(pathInString);
					++numPreLoadedModels;
				}
			}
		}
		void UnloadReference(TypeReference<AnimationData>& modelRef)
		{
			if (modelRef.m_ResourcePtr == nullptr)
				return;

			modelRef.m_ResourcePtr = nullptr;
			--m_ReferenceCnt[modelRef.m_AssetName];
			modelRef.m_AssetName = "";

		}
		void DestryoAllAnimation()
		{
		}


	};
}