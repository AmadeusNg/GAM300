/*!*****************************************************************************
 * \file          CollisionSystem.cpp
 * \author        Cheang Wen Ding
 * \par DP email: cheang.w@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         Implementation of the collision system.
 *******************************************************************************/
#include "Physics/CollisionSystem.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/PhysicsSystem.h"
namespace TDS
{
	// Static variables
	std::vector<Vec3>				CollisionSystem::cubeVertices;
	std::vector<unsigned int>		CollisionSystem::cubeIndices;
	std::vector<float>				CollisionSystem::sphereVertices;
	std::vector<unsigned int>		CollisionSystem::sphereIndices;
	std::vector<float>				CollisionSystem::capsuleVertices;
	std::vector<unsigned int>		CollisionSystem::capsuleIndices;

	void CollisionSystem::CollisionSystemInit()
	{
		ClearAll();
		GenerateCube();
		GenerateSphere();
		GenerateCapsule();
	}

	void CollisionSystem::CollisionSystemUpdate(const float dt, const std::vector<EntityID>& entities, Transform* _transform, GraphicsComponent* _graphics)
	{
		for (int i = 0; i < entities.size(); ++i)
		{
			Mat4 modelMatrix = Mat4::identity();
			if (!ecs.getEntityIsEnabled(entities[i]) || !ecs.getComponentIsEnabled<GraphicsComponent>(entities[i]))
			{
				continue;
			}
			if (PhysicsSystem::GetIsPlaying()) continue;
			if (GetSphereCollider(entities[i]))
			{
				SphereCollider* vSphere = GetSphereCollider(entities[i]);
				_transform[i].SetOffSetPos(vSphere->GetColliderCenter());
				float radiusFactor = (vSphere->GetRadiusFactor() - 1.f);
				_transform[i].SetOffSetScale(radiusFactor);

				float newRadius = (vSphere->GetRadiusFactor() * _transform[i].GetScale().x);
				if (newRadius != vSphere->GetColliderRadius())
				{
					vSphere->SetColliderRadius(newRadius);
				}

			}

			else if (GetBoxCollider(entities[i]))
			{
				BoxCollider* vBox = GetBoxCollider(entities[i]);
				if (_graphics[i].GetModelName() == "cube_Bin.bin")
				{
					vBox->SetColliderSize(_transform[i].GetScale());
					vBox->SetColliderCenter(_transform[i].GetPosition());
				}
				else
				{
					//if (!vBox->GetModelInit())
					{
						TDS::AssetModel* tmp_assetModel = AssetManager::GetInstance()->GetModelFactory().GetModel(_graphics[i].GetModelName(), _graphics[i].GetAsset());
						std::string key = _graphics[i].GetMeshName();
						auto it = tmp_assetModel->m_Meshes.find(key);
						if (it != tmp_assetModel->m_Meshes.end())
						{
							MeshData* tmp_MeshData = &(it->second);
							Vec3 minBoundingBox(FLT_MAX, FLT_MAX, FLT_MAX);
							Vec3 maxBoundingBox(-FLT_MAX, -FLT_MAX, -FLT_MAX);
							for (auto& modelCoord : tmp_MeshData->m_VertexData)
							{
								minBoundingBox.x = Mathf::Min(minBoundingBox.x, modelCoord.m_Pos.x);
								minBoundingBox.y = Mathf::Min(minBoundingBox.y, modelCoord.m_Pos.y);
								minBoundingBox.z = Mathf::Min(minBoundingBox.z, modelCoord.m_Pos.z);

								maxBoundingBox.x = Mathf::Max(maxBoundingBox.x, modelCoord.m_Pos.x);
								maxBoundingBox.y = Mathf::Max(maxBoundingBox.y, modelCoord.m_Pos.y);
								maxBoundingBox.z = Mathf::Max(maxBoundingBox.z, modelCoord.m_Pos.z);

							}

							Vec3 vSize = (maxBoundingBox - minBoundingBox) * 0.5f;
							Vec3 vCenter = (maxBoundingBox + minBoundingBox) * 0.5f;
							Vec4 worldCenter = _transform[i].GetTransformMatrix() * Vec4(vCenter, 1.f);
							
							vBox->SetColliderSize(vSize);
							vBox->SetColliderCenter(Vec3(worldCenter.x, worldCenter.y, worldCenter.z));
							vBox->SetModelSize(vSize); // scale
							vBox->SetModelRotation(_transform[i].GetRotation()); // rotate
							vBox->SetModelCenter(vCenter); // translate


						}
						vBox->SetModelInit(true);
					}
					// scaling the box collider & other offset values
					//else
					{
						if (_transform[i].GetScale() + 1.f != vBox->GetColliderScale() || vBox->GetOffsetScale() != Vec3(0.f))
						{
							Vec3 vScale = _transform[i].GetScale() + vBox->GetOffsetScale();
							Vec3 vSize = vBox->GetModelSize();
							vSize.x *= vScale.x;
							vSize.y *= vScale.y;
							vSize.z *= vScale.z;
							vBox->SetColliderSize(vSize);
							vBox->SetColliderScale(vScale);
						}
					}
				}
				//vBox->SetColliderCenter(_transform[i].GetPosition() + vBox->GetOffsetCenter());
				
				_transform[i].SetOffSetPos(vBox->GetOffsetCenter());
				Vec3 vScale = _transform[i].GetScale() - vBox->GetColliderScale() ;
				_transform[i].SetOffSetScale(vBox->GetOffsetScale());
			}

			else if (GetCapsuleCollider(entities[i]))
			{
				CapsuleCollider* vCapsule = GetCapsuleCollider(entities[i]);
				_transform[i].SetOffSetPos(vCapsule->GetCenter());
				float vRadius = vCapsule->GetRadius() * 2.f;
				float vHeight = vCapsule->GetHeight() / 2.f;
				Vec3 vScale = Vec3(vRadius, vHeight, vRadius) - Vec3(1.f);
				_transform[i].SetOffSetScale(vScale);
			}
		}
	}
	/*
	* Functions that helps to generate shapes vertices and index
	*/
	void CollisionSystem::GenerateCube()
	{
		cubeVertices =
		{
			// Front face
			Vec3( - 0.5, -0.5,  0.5), // Bottom-left
			Vec3( 0.5, -0.5,  0.5), // Bottom-right
			Vec3( 0.5,  0.5,  0.5), // Top-right
			Vec3(-0.5,  0.5,  0.5), // Top-left

			// Back face
			Vec3(-0.5, -0.5, -0.5), // Bottom-left
			Vec3( 0.5, -0.5, -0.5), // Bottom-right
			Vec3( 0.5,  0.5, -0.5), // Top-right
			Vec3(-0.5,  0.5, -0.5)  // Top-left
		};

		cubeIndices =
		{
			// Front face
			0, 1, 2,
			2, 3, 0,

			// Top face
			3, 2, 6,
			6, 7, 3,

			// Back face
			7, 6, 5,
			5, 4, 7,

			// Bottom face
			4, 5, 1,
			1, 0, 4,

			// Left face
			4, 0, 3,
			3, 7, 4,

			// Right face
			1, 5, 6,
			6, 2, 1

		};
	}

	void CollisionSystem::GenerateSphere()
	{
		unsigned int vertexCountPerRow = 9; // 2^subdivision + 1, subdivision = 3
		unsigned int vertexCountPerFace = vertexCountPerRow * vertexCountPerRow;

		std::vector<float> unitVertices = getUnitPositiveX(vertexCountPerRow);

		unsigned int k = 0, k1, k2, i1, i2; // indices
		float v1[3], v2[3], v3[3], v4[3];   // tmp vertices


		// +X face
		for (unsigned int i = 0; i < vertexCountPerRow - 1; ++i)
		{
			k1 = i * vertexCountPerRow;              // index at curr row
			k2 = k1 + vertexCountPerRow;             // index at next row

			for (unsigned int j = 0; j < vertexCountPerRow - 1; ++j, ++k1, ++k2)
			{
				i1 = k1 * 3;
				i2 = k2 * 3;

				// 4 vertices of a quad
				// v1--v3
				// | / |
				// v2--v4
				v1[0] = unitVertices[i1];
				v1[1] = unitVertices[i1 + 1];
				v1[2] = unitVertices[i1 + 2];
				v2[0] = unitVertices[i2];
				v2[1] = unitVertices[i2 + 1];
				v2[2] = unitVertices[i2 + 2];
				v3[0] = unitVertices[i1 + 3];
				v3[1] = unitVertices[i1 + 4];
				v3[2] = unitVertices[i1 + 5];
				v4[0] = unitVertices[i2 + 3];
				v4[1] = unitVertices[i2 + 4];
				v4[2] = unitVertices[i2 + 5];

				// add 4 vertex attributes
				AddVertices(sphereVertices, v1, v2, v3, v4);

				// add indices of 2 triangles
				AddIndex(sphereIndices, k, k + 1, k + 2);
				AddIndex(sphereIndices, k + 2, k + 1, k + 3);

				k += 4;     // next
			}
		}

		// array size and index for building next face
		unsigned int startIndex;                    // starting index for next face
		int vertexSize = (int)sphereVertices.size();      // vertex array size of +X face
		int indexSize = (int)sphereIndices.size();        // index array size of +X face

		// build -X face by negating x and z values
		startIndex = sphereVertices.size() / 3;
		for (int i = 0, j = 0; i < vertexSize; i += 3, j += 2)
		{
			AddVertex(sphereVertices, -sphereVertices[i], sphereVertices[i + 1], -sphereVertices[i + 2]);
		}
		for (int i = 0; i < indexSize; ++i)
		{
			sphereIndices.push_back(startIndex + sphereIndices[i]);
		}

		// build +Y face by swapping x=>y, y=>-z, z=>-x
		startIndex = sphereVertices.size() / 3;
		for (int i = 0, j = 0; i < vertexSize; i += 3, j += 2)
		{
			AddVertex(sphereVertices, -sphereVertices[i + 2], sphereVertices[i], -sphereVertices[i + 1]);
		}
		for (int i = 0; i < indexSize; ++i)
		{
			sphereIndices.push_back(startIndex + sphereIndices[i]);
		}

		// build -Y face by swapping x=>-y, y=>z, z=>-x
		startIndex = sphereVertices.size() / 3;
		for (int i = 0, j = 0; i < vertexSize; i += 3, j += 2)
		{
			AddVertex(sphereVertices, -sphereVertices[i + 2], -sphereVertices[i], sphereVertices[i + 1]);
		}
		for (int i = 0; i < indexSize; ++i)
		{
			sphereIndices.push_back(startIndex + sphereIndices[i]);
		}

		// build +Z face by swapping x=>z, z=>-x
		startIndex = sphereVertices.size() / 3;
		for (int i = 0, j = 0; i < vertexSize; i += 3, j += 2)
		{
			AddVertex(sphereVertices, -sphereVertices[i + 2], sphereVertices[i + 1], sphereVertices[i]);
		}
		for (int i = 0; i < indexSize; ++i)
		{
			sphereIndices.push_back(startIndex + sphereIndices[i]);
		}

		// build -Z face by swapping x=>-z, z=>x
		startIndex = sphereVertices.size() / 3;
		for (int i = 0, j = 0; i < vertexSize; i += 3, j += 2)
		{
			AddVertex(sphereVertices, sphereVertices[i + 2], sphereVertices[i + 1], -sphereVertices[i]);
		}
		for (int i = 0; i < indexSize; ++i)
		{
			sphereIndices.push_back(startIndex + sphereIndices[i]);
		}


	}

	std::vector<float> CollisionSystem::getUnitPositiveX(unsigned int pointsPerRow)
	{
		std::vector<float> vertices;
		float n1[3];        // normal of longitudinal plane rotating along Y-axis
		float n2[3];        // normal of latitudinal plane rotating along Z-axis
		float v[3];         // direction vector intersecting 2 planes, n1 x n2
		float a1;           // longitudinal angle along y-axis
		float a2;           // latitudinal angle
		float scale;

		// rotate latitudinal plane from 45 to -45 degrees along Z-axis
		for (unsigned int i = 0; i < pointsPerRow; ++i)
		{
			// normal for latitudinal plane
			a2 = Mathf::Deg2Rad * (45.0f - 90.0f * i / (pointsPerRow - 1));
			n2[0] = -sin(a2);
			n2[1] = cos(a2);
			n2[2] = 0;

			// rotate longitudinal plane from -45 to 45 along Y-axis
			for (unsigned int j = 0; j < pointsPerRow; ++j)
			{
				// normal for longitudinal plane
				a1 = Mathf::Deg2Rad * (-45.0f + 90.0f * j / (pointsPerRow - 1));
				n1[0] = -sin(a1);
				n1[1] = 0;
				n1[2] = -cos(a1);

				// find direction vector of intersected line, n1 x n2
				v[0] = n1[1] * n2[2] - n1[2] * n2[1];
				v[1] = n1[2] * n2[0] - n1[0] * n2[2];
				v[2] = n1[0] * n2[1] - n1[1] * n2[0];

				// normalize direction vector
				scale = 1.f / Mathf::Sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
				v[0] *= scale;
				v[1] *= scale;
				v[2] *= scale;

				vertices.push_back(v[0]);
				vertices.push_back(v[1]);
				vertices.push_back(v[2]);

			}
		}

		return vertices;
	}

	void CollisionSystem::GenerateCapsule()
	{
		float height = 1.f;
		float radius = 0.25f;
		int segments = 20;

		if (segments % 2 != 0)
		{
			segments++;
		}

		int points = segments + 1;
		float pX[25], pY[25], pZ[25], pR[25];

		float calcH = 0.f;
		float calcV = 0.f;

		for (int i = 0; i < points; ++i)
		{
			pX[i] = Mathf::Sin(calcH * Mathf::Deg2Rad);
			pZ[i] = Mathf::Cos(calcH * Mathf::Deg2Rad);
			pY[i] = Mathf::Cos(calcV * Mathf::Deg2Rad);
			pR[i] = Mathf::Sin(calcV * Mathf::Deg2Rad);

			calcH += 360.f / (float)segments;
			calcV += 180.f / (float)segments;
		}

		// Y-offset is half the height minus the diameter
		float yOff = (height - (radius * 2.f)) * 0.5f;
		if (yOff < 0)
			yOff = 0;
		int top = Mathf::CeilToInt(points * 0.5f);
		for (int y = 0; y < top; ++y)
		{
			for (int x = 0; x < points; ++x)
			{
				Vec3 tmp = Vec3(pX[x] * pR[y], pY[y], pZ[x] * pR[y] ) * radius;
				tmp.y = yOff + tmp.y;
				AddVertex(capsuleVertices, tmp.x, tmp.y, tmp.z);
			}
		}

		// Bottom Hemisphere
		int btm = Mathf::FloorToInt(points * 0.5f);
		for (int y = btm; y < points; ++y)
		{
			for (int x = 0; x < points; ++x)
			{
				Vec3 tmp = Vec3(pX[x] * pR[y], pY[y], pZ[x] * pR[y]) * radius;
				tmp.y = -yOff + tmp.y;
				AddVertex(capsuleVertices, tmp.x, tmp.y, tmp.z);
			}
		}

		// Index
		for (int y = 0; y < segments + 1; ++y)
		{
			for (int x = 0; x < segments; ++x)
			{
				int i0,i1, i2, i3, i4, i5;
				i0 = (y + 0)* (segments + 1) + x + 0;
				i1 = (y + 1)* (segments + 1) + x + 0;
				i2 = (y + 1)* (segments + 1) + x + 1;

				i3 = (y + 0)* (segments + 1) + x + 1;
				i4 = (y + 0)* (segments + 1) + x + 0;
				i5 = (y + 1)* (segments + 1) + x + 1;

				AddIndex(capsuleIndices, i0, i1, i2);
				AddIndex(capsuleIndices, i3, i4, i5);
			}
		}

	}

	void CollisionSystem::AddVertex(std::vector<float>& vertices, float x, float y, float z)
	{
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);
	}

	void CollisionSystem::AddVertices(std::vector<float>& vertices, const float v1[3], const float v2[3], const float v3[3], const float v4[3])
	{
		vertices.insert(vertices.end(), v1, v1 + 3);    // v1
		vertices.insert(vertices.end(), v2, v2 + 3);    // v2
		vertices.insert(vertices.end(), v3, v3 + 3);    // v3
		vertices.insert(vertices.end(), v4, v4 + 3);    // v4
	}

	void CollisionSystem::AddIndex(std::vector<unsigned int>& indices, unsigned int a, unsigned int b, unsigned int c)
	{
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	void CollisionSystem::ClearAll()
	{
		cubeVertices.clear();
		cubeIndices.clear();
		sphereVertices.clear();
		sphereIndices.clear();
		capsuleVertices.clear();
		capsuleIndices.clear();
	}



}

