#include "Animation/AnimationSystem.h"
#include "components/animationComponent.h"
#include "components/GraphicsComponent.h"
#include "AssetManagement/AssetManager.h"
namespace TDS
{
	void AnimationSystem::Init()
	{
	}
	void AnimationSystem::Update(const float dt, const std::vector<EntityID>& entities, GraphicsComponent* graphics, AnimationComponent* animatedComponent)
	{
		/*
		
				if (auto pAC = ecs.getComponent<AnimationComponent>(entityID); pAC != nullptr)
		{
			if (!pAC->m_AnimationJson.empty())
			{
				pAC->m_AnimationPlayer.Update(_dt);
				auto animBones = pAC->m_AnimationPlayer.getCurrentBonesMatrices();

				for (int i{ 0 }; i < animBones->size() && i < MAX_BONES; i++)
				{
					m_BonesUniform.m_Bones[i] = animBones->at(i);
				}
			}
		}
		
		*/
		auto& AnimationResources = AssetManager::GetInstance()->GetAnimationFactory();
		for (size_t i = 0; i < entities.size(); ++i)
		{
			if (graphics[i].m_MeshControllerRef.m_ResourcePtr == nullptr)
				continue;

			UpdateAnimationPack(&animatedComponent[i]);
			
			if (animatedComponent[i].m_AnimationAsset.m_ResourcePtr == nullptr)
				continue;

			if (animatedComponent[i].m_Animate)
			{
				animatedComponent[i].m_AnimationPlayer.Update(dt, animatedComponent[i].m_AnimationSpeed);
				graphics[i].m_IsAnimated = true;
			}


			
		}
	}
	void AnimationSystem::UpdateAnimationPack(AnimationComponent* animationComponent)
	{
		if (animationComponent->m_AnimationPackName != animationComponent->m_AnimationAsset.m_AssetName)
		{
			auto& AnimationResources = AssetManager::GetInstance()->GetAnimationFactory();
			animationComponent->m_AnimationAsset.m_ResourcePtr = AnimationResources.GetAnimationData(animationComponent->m_AnimationPackName, animationComponent->m_AnimationAsset);


			if (animationComponent->m_AnimationAsset.m_ResourcePtr)
				animationComponent->m_AnimationPlayer.setAnimation(animationComponent->m_AnimationAsset.m_ResourcePtr->m_Animations[0]);

		}
	}
}
