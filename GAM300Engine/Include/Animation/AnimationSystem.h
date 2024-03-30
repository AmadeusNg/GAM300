#pragma once
#include <ecs/ecs.h>
#include "Animation/Animation.h"

namespace TDS
{
	class AnimationComponent;
	class GraphicsComponent;

	class AnimationSystem
	{
	public:
		static void Init();
		static void Update(const float dt, const std::vector<EntityID>& entities, GraphicsComponent* transform, AnimationComponent* _Sprite);
		static void UpdateAnimationPack(AnimationComponent* _Sprite);
	};


}