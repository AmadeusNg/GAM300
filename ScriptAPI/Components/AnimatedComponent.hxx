#pragma once

#include "TransformComponent.hxx"
#include "ComponentBase.hxx"

namespace ScriptAPI
{
	public value class AnimatedComponent : ComponentBase
	{
	public:
		void SetAnimationSpeed(float speed);

		void PlayAnimation();

		void StopAnimation();

		bool IsAnimationPlaying();

		System::String^ GetAnimationName();

		void SetAnimation(System::String^ value);

		virtual TDS::EntityID GetEntityID();
		virtual void SetEntityID(TDS::EntityID ID);

		virtual void SetEnabled(bool enabled);
		virtual bool GetEnabled();

		GameObject^ gameObject;
		TransformComponent transform;
	internal:
		AnimatedComponent(TDS::EntityID ID);

	private:
		TDS::EntityID entityID;
	};


}