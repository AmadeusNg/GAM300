#pragma once

#include "ColliderComponent.hxx"

namespace ScriptAPI
{
	public ref class BoxColliderComponent : ColliderComponent
	{
	public:
		virtual bool GetIsTrigger() override;
		virtual void SetIsTrigger(bool value) override;

		Vector3 GetCenter();
		void SetCenter(Vector3 value);
		void SetCenterX(float valueX);
		void SetCenterY(float valueY);
		void SetCenterZ(float valueZ);

		Vector3 GetSize();
		void SetSize(Vector3 value);
		void SetSizeX(float valueX);
		void SetSizeY(float valueY);
		void SetSizeZ(float valueZ);

		Vector3 GetOffsetScale();
		Vector3 GetOffsetCenter();

		virtual TDS::EntityID GetEntityID() override;
		virtual void SetEntityID(TDS::EntityID ID) override;

		virtual void SetEnabled(bool enabled) override;
		virtual bool GetEnabled() override;

		TransformComponent transform;
		GameObject^ gameObject;

	internal:
		BoxColliderComponent(TDS::EntityID ID);

	private:
		property bool IsTrigger
		{
			bool get();
			void set(bool value);
		}
		property Vector3 Center
		{
			Vector3 get();
			void set(Vector3 value);
		}
		property Vector3 Size
		{
			Vector3 get();
			void set(Vector3 value);
		}

		TDS::EntityID entityID;
	};
}