/*!*****************************************************************************
 * \file          CollisionSystem.h
 * \author        Cheang Wen Ding
 * \par DP email: cheang.w@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         The system that check and handle collision.
 *******************************************************************************/
#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "ecs/ecs.h"
#include "components/sphereCollider.h"
#include "components/boxCollider.h"
#include "components/capsuleCollider.h"
#include "components/transform.h"
#include "components/GraphicsComponent.h"
#include "dotnet/ImportExport.h"

namespace TDS
{
	class DLL_API CollisionSystem
	{
	public:
		/*!*************************************************************************
		 * Collision System Init and Update (Will be used by the ECS)
		 ***************************************************************************/
		static void CollisionSystemInit();
		// Main purpose is to get the offset value from each collider and pass to graphics for render
		static void CollisionSystemUpdate(const float dt, const std::vector<EntityID>& entities, Transform* _transform, GraphicsComponent* _graphics);
		
		// Center at 0,0,0, length = 1
		static std::vector<Vec3> cubeVertices;
		static std::vector<unsigned int> cubeIndices;
		// Center at 0,0,0, radius = 1
		static std::vector<float> sphereVertices;
		static std::vector<unsigned int> sphereIndices;
		// Center at 0,0,0, radius = 0.25, height = 1
		static std::vector<float> capsuleVertices;
		static std::vector<unsigned int> capsuleIndices;

	private:
		// Helper functions
		static void AddVertex(std::vector<float>& vertices, float x, float y, float z);
		static void AddVertices(std::vector<float>& vertices, const float v1[3], const float v2[3], const float v3[3], const float v4[3]);
		static void AddIndex(std::vector<unsigned int>& indices, unsigned int a, unsigned int b, unsigned int c);
		static void ClearAll();
		// Cube functions
		static void GenerateCube();
		
		// Sphere functions
		static void GenerateSphere();
		static std::vector<float> getUnitPositiveX(unsigned int pointsPerRow);

		// Capsule functions
		static void GenerateCapsule();


	};
}


#endif // !COLLISIONSYSTEM_H
