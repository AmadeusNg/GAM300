#include "components/animationComponent.h"
#include "components/CompReflectionMacro.h"

RTTR_REGISTRATION
{
	using namespace TDS;
	rttr::registration::class_<AnimationComponent>("AnimationComponent")
		.property("AnimationPackName", &AnimationComponent::m_AnimationPackName)
		.property("isAnimated", &AnimationComponent::m_Animate)
		.property("Speed", &AnimationComponent::m_AnimationSpeed);
}
namespace TDS
{

	AnimationComponent::AnimationComponent() : m_Animate(false), m_AnimationPackName(), m_AnimationSpeed(1.f),  m_AnimationAsset(), m_AnimationPlayer()
	{
	}
	DLL_API AnimationComponent* GetAnimationComponent(EntityID entityID)
	{
		return ecs.getComponent<AnimationComponent>(entityID);
	}

}