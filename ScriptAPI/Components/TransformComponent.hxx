#pragma once

#include "../IncludeFromEngine.hxx"
#include "../Vector3.hxx"

namespace ScriptAPI
{
	public value class TransformComponent
	{
	public:
		Vector3 GetPosition();
		void SetPosition(Vector3 value);
		void SetPositionX(float valueX);
		void SetPositionY(float valueY);
		void SetPositionZ(float valueZ);

		Vector3 GetScale();
		void SetScale(Vector3 value);
		void SetScaleX(float valueX);
		void SetScaleY(float valueY);
		void SetScaleZ(float valueZ);

		Vector3 GetRotation();
		void SetRotation(Vector3 value);
		void SetRotationX(float valueX);
		void SetRotationY(float valueY);
		void SetRotationZ(float valueZ);

	internal:
		TransformComponent(TDS::EntityID ID);

	private:
		property Vector3 Position
		{
			Vector3 get();
			void set(Vector3 value);
		}
		property Vector3 Scale
		{
			Vector3 get();
			void set(Vector3 value);
		}
		property Vector3 Rotation
		{
			Vector3 get();
			void set(Vector3 value);
		}

		TDS::EntityID entityID;
	};
}