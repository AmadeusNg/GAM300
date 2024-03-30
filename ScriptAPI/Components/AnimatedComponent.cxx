#include "AnimatedComponent.hxx"
#include "../TypeConversion.hxx"
#include "../EngineInterface.hxx"

namespace ScriptAPI
{
	AnimatedComponent::AnimatedComponent(TDS::EntityID ID) : entityID(ID)
	{
		gameObject = EngineInterface::GetGameObject(ID);
	}
	void AnimatedComponent::SetAnimationSpeed(float speed)
	{
		if (!TDS::GetAnimationComponent(entityID))
		{
			return;
		}
		TDS::GetAnimationComponent(entityID)->setAnimateSpeed(speed);
	}
	void AnimatedComponent::PlayAnimation()
	{
		if (!TDS::GetAnimationComponent(entityID))
		{
			return;
		}
		TDS::GetAnimationComponent(entityID)->ToggleAnimate(true);
	}
	void AnimatedComponent::StopAnimation()
	{
		if (!TDS::GetAnimationComponent(entityID))
		{
			return;
		}
		TDS::GetAnimationComponent(entityID)->ToggleAnimate(false);
	}
	bool AnimatedComponent::IsAnimationPlaying()
	{
		if (!TDS::GetAnimationComponent(entityID))
		{
			return false;
		}

		return TDS::GetAnimationComponent(entityID)->m_Animate;
	}
	System::String^ AnimatedComponent::GetAnimationName()
	{
		if (!TDS::GetAnimationComponent(entityID))
		{
			return "";
		}

		return toSystemString(TDS::GetAnimationComponent(entityID)->getAnimationPackName());
	}
	void AnimatedComponent::SetAnimation(System::String^ value)
	{
		if (!TDS::GetAnimationComponent(entityID))
		{
			return;
		}
		TDS::GetAnimationComponent(entityID)->SetAnimationPack(toStdString(value));
	}
	TDS::EntityID AnimatedComponent::GetEntityID()
	{
		return entityID;
	}
	void AnimatedComponent::SetEntityID(TDS::EntityID ID)
	{
		entityID = ID;
		transform = TransformComponent(ID);
		gameObject = EngineInterface::GetGameObject(ID);
	}
	void AnimatedComponent::SetEnabled(bool enabled)
	{
		TDS::setComponentIsEnable("AnimationComponent", GetEntityID(), enabled);
	}
	bool AnimatedComponent::GetEnabled()
	{
		return TDS::getComponentIsEnable("AnimationComponent", GetEntityID());
	}
}