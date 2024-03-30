#pragma once
#include "ecs/ecs.h"
#include "Animation/AnimationLoader.h"
#include "Animation/Animation.h"
#include <string>
#include "ResourceManagement/ResourceRef.h"
namespace TDS
{
	class AnimationComponent : public IComponent
	{
	public:
		DLL_API AnimationComponent();
		DLL_API ~AnimationComponent() = default;
		//setter
		inline void	SetAnimationPack(const std::string& input) { m_AnimationPackName = input; }
		inline void ToggleAnimate(bool _animate) { m_Animate = _animate; }
		inline void setAnimateSpeed(float& _speed) { m_AnimationSpeed = _speed; }
		
		//getter
		inline bool& getAnimate() { return m_Animate; }
		inline std::string& getAnimationPackName() { return m_AnimationPackName; }
		inline float& getAnimateSpeed() { return m_AnimationSpeed; }
		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND

	public: 
		bool m_Animate{ false };
		std::string m_AnimationPackName;
		float m_AnimationSpeed;
		TypeReference<AnimationData> m_AnimationAsset;
		AnimationPlayer m_AnimationPlayer;
	};

	DLL_API AnimationComponent* GetAnimationComponent(EntityID entityID);
}