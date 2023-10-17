#include "TransformComponent.hxx"

namespace ScriptAPI
{
	// POSITION ==============================================================================
	// Private
	Vector3 TransformComponent::Position::get()
	{
		// May wanna change to a function
		if (!TDS::GetTransform(entityID))
		{		
			// throw error instead (not sure how)
			return Vector3(0.f, 0.f, 0.f);
		}

		return Vector3(TDS::GetTransform(entityID)->GetPosition().x, TDS::GetTransform(entityID)->GetPosition().y, TDS::GetTransform(entityID)->GetPosition().z);
		
		//return TDS::GetTransform(entityID)->GetPosition();
	}
	void TransformComponent::Position::set(Vector3 value)
	{
		// May wanna change to a function
		if (!TDS::GetTransform(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetTransform(entityID)->SetPosition(value.X, value.Y, value.Z);
	}

	// Public
	Vector3 TransformComponent::GetPosition()
	{
		return Position;
	}
	void TransformComponent::SetPosition(Vector3 value)
	{
		Position = value;
	}
	void TransformComponent::SetPositionX(float valueX)
	{
		Vector3 oldPosition = Position;
		Position = Vector3(valueX, oldPosition.Y, oldPosition.Z);
	}
	void TransformComponent::SetPositionY(float valueY)
	{
		Vector3 oldPosition = Position;
		Position = Vector3(oldPosition.X, valueY, oldPosition.Z);
	}
	void TransformComponent::SetPositionZ(float valueZ)
	{
		Vector3 oldPosition = Position;
		Position = Vector3(oldPosition.X, oldPosition.Y, valueZ);
	}

	// SCALE =================================================================================
	// Private
	Vector3 TransformComponent::Scale::get()
	{
		// May wanna change to a function
		if (!TDS::GetTransform(entityID))
		{
			// throw error instead (not sure how)
			return Vector3(0.f, 0.f, 0.f);
		}

		return Vector3(TDS::GetTransform(entityID)->GetScale().x, TDS::GetTransform(entityID)->GetScale().y, TDS::GetTransform(entityID)->GetScale().z);
	}
	void TransformComponent::Scale::set(Vector3 value)
	{
		// May wanna change to a function
		if (!TDS::GetTransform(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetTransform(entityID)->SetScale(value.X, value.Y, value.Z);
	}

	// Public
	Vector3 TransformComponent::GetScale()
	{
		return Scale;
	}
	void TransformComponent::SetScale(Vector3 value)
	{
		Scale = value;
	}
	void TransformComponent::SetScaleX(float valueX)
	{
		Vector3 oldScale = Scale;
		Scale = Vector3(valueX, oldScale.Y, oldScale.Z);
	}
	void TransformComponent::SetScaleY(float valueY)
	{
		Vector3 oldScale = Scale;
		Scale = Vector3(oldScale.X, valueY, oldScale.Z);
	}
	void TransformComponent::SetScaleZ(float valueZ)
	{
		Vector3 oldScale = Scale;
		Scale = Vector3(oldScale.X, oldScale.Y, valueZ);
	}

	// ROTATION ==============================================================================
	// Private
	float TransformComponent::Rotation::get()
	{
		// May wanna change to a function
		if (!TDS::GetTransform(entityID))
		{
			// throw error instead (not sure how)
			return 0.0f;
		}

		return TDS::GetTransform(entityID)->GetRotation();
	}
	void TransformComponent::Rotation::set(float value)
	{
		// May wanna change to a function
		if (!TDS::GetTransform(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetTransform(entityID)->SetRotation(value);
	}

	// Public
	float TransformComponent::GetRotation()
	{
		return Rotation;
	}
	void TransformComponent::SetRotation(float value)
	{
		Rotation = value;
	}

	// CONSTRUCTOR ===========================================================================
	TransformComponent::TransformComponent(TDS::EntityID ID) : entityID (ID)
	{ }
}