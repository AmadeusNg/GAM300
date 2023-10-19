/*!*****************************************************************************
 * \file          ModelFactory.h
 * \author        Eugene Ho Shu Sheng
 * \par DP email: shushengeugene.ho@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         This file contains the declaration of the ModelFactory class.
 *******************************************************************************/
#pragma once
#include "FactoryBase.h"
#include <string_view>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>

#define MODEL_PATH "../assets/models/"
#define MAX_PRELOAD_MODELS 5

namespace TDS
{
	/*!*************************************************************************
	 * template class ModelFactory
	 ***************************************************************************/
	template<>
	class AssetFactory<AssetModel>
	{
	public:
		std::mutex m_Mutex;
		std::unordered_map<std::string, std::int32_t> m_ModelMap;
		std::vector<std::uint64_t> m_LoadedModelsGUID;



		/*!*************************************************************************
		 * Deserialize the model from the file.
		 ***************************************************************************/
		DLL_API static void DeserializeGeom(GeomCompiled& geomOut, std::string_view PathData)
		{
			std::ifstream inFile(PathData.data(), std::ios::binary);
			if (!inFile)
			{
				std::cerr << "ERROR: Could not open file for reading!\n";
				return;
			}

			auto deserializeVec3 = [](std::ifstream& in, Vec3& vec)
			{
				in.read(reinterpret_cast<char*>(&vec.x), sizeof(vec.x));
				in.read(reinterpret_cast<char*>(&vec.y), sizeof(vec.y));
				in.read(reinterpret_cast<char*>(&vec.z), sizeof(vec.z));
			};

			auto deserializeVec2 = [](std::ifstream& in, Vec2& vec)
			{
				in.read(reinterpret_cast<char*>(&vec.x), sizeof(vec.x));
				in.read(reinterpret_cast<char*>(&vec.y), sizeof(vec.y));
			};

			auto deserializeMesh = [](std::ifstream& in, GeomCompiled::Mesh& mesh)
			{
				in.read(reinterpret_cast<char*>(&mesh.m_Name), sizeof(mesh.m_Name));
			};

			auto deserializeSubMesh = [](std::ifstream& in, GeomCompiled::SubMesh& subMesh)
			{
				in.read(reinterpret_cast<char*>(&subMesh.m_nFaces), sizeof(subMesh.m_nFaces));
				in.read(reinterpret_cast<char*>(&subMesh.m_iIndices), sizeof(subMesh.m_iIndices));
				in.read(reinterpret_cast<char*>(&subMesh.m_iVertices), sizeof(subMesh.m_iVertices));
				in.read(reinterpret_cast<char*>(&subMesh.m_nVertices), sizeof(subMesh.m_nVertices));
				in.read(reinterpret_cast<char*>(&subMesh.m_iMaterial), sizeof(subMesh.m_iMaterial));
			};

			auto deserializeExtraVertices = [&](std::ifstream& in, GeomCompiled::ExtraVertices& extraVertices)
			{
				deserializeVec2(in, extraVertices.m_UV);
				deserializeVec3(in, extraVertices.m_Normal);
				deserializeVec3(in, extraVertices.m_Tanget);
				deserializeVec3(in, extraVertices.m_Bitangent);
				in.read(reinterpret_cast<char*>(&extraVertices.m_Colour), sizeof(extraVertices.m_Colour));
			};

			uint32_t meshSize;
			inFile.read(reinterpret_cast<char*>(&meshSize), sizeof(meshSize));
			geomOut.m_Mesh.resize(meshSize);
			for (auto& mesh : geomOut.m_Mesh)
			{
				deserializeMesh(inFile, mesh);
			}

			uint32_t subMeshSize;
			inFile.read(reinterpret_cast<char*>(&subMeshSize), sizeof(subMeshSize));
			geomOut.m_SubMesh.resize(subMeshSize);
			for (auto& subMesh : geomOut.m_SubMesh)
			{
				deserializeSubMesh(inFile, subMesh);
			}

			uint32_t posSize;
			inFile.read(reinterpret_cast<char*>(&posSize), sizeof(posSize));
			geomOut.m_Pos.resize(posSize);
			for (auto& pos : geomOut.m_Pos)
			{
				deserializeVec3(inFile, pos);
			}

			uint32_t extraSize;
			inFile.read(reinterpret_cast<char*>(&extraSize), sizeof(extraSize));
			geomOut.m_Extra.resize(extraSize);
			for (auto& extra : geomOut.m_Extra)
			{
				deserializeExtraVertices(inFile, extra);
			}

			uint32_t indicesSize;
			inFile.read(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
			geomOut.m_Indices.resize(indicesSize);
			inFile.read(reinterpret_cast<char*>(geomOut.m_Indices.data()), indicesSize * sizeof(uint32_t));

			inFile.close();
		}
		/*!*************************************************************************
		 * Preload the model from the file.
		 ***************************************************************************/
		void Preload(ResourceManager& resourceMgr)
		{
			m_LoadedModelsGUID.reserve(20);
			std::filesystem::path dir = MODEL_PATH;

			if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
			{
				std::cout << "Invalid directory" << std::endl;
				return;
			}
			std::uint32_t numLoadedModels = 0;
			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				if (numLoadedModels >= MAX_PRELOAD_MODELS)
					break;


				const std::filesystem::path& path = entry.path();

				if (path.extension() == ".bin")
				{
					GeomCompiled newGeom{};
					SingleTypeReference<AssetModel> modelInstance{};

					DeserializeGeom(newGeom, entry.path().string());
					std::string fileName = path.filename().string();
					modelInstance.m_Identifier.GetTypeName<AssetModel>();
					modelInstance.m_Identifier.CreateTypeIDByName(fileName);
					modelInstance.m_GUID = UniqueID::CreateUniqueGuid();

					auto newModel = resourceMgr.getResource(modelInstance);
					if (newModel == nullptr)
					{
						std::cout << "Failed to create model instance!" << std::endl;
						continue;
					}
					newModel->LoadGeomData(newGeom);
					m_ModelMap[fileName] = static_cast<int>(m_LoadedModelsGUID.size());
					m_LoadedModelsGUID.emplace_back(modelInstance.m_GUID.GetUniqueID());

					++numLoadedModels;


				}
			}
		}

		/*!*************************************************************************
		 * Loading the model from the file.
		 ***************************************************************************/
		DLL_API  static void Load(std::string_view path, SingleTypeReference<AssetModel>& model, ResourceManager& resourceMgr)
		{
			GeomCompiled geom{};
			DeserializeGeom(geom, path);
			if (resourceMgr.getResource(model) == nullptr)
			{
				std::cout << "Failed to load resource!" << std::endl;
				return;
			}
			model.m_Reference->LoadGeomData(geom);

		}



	};



}