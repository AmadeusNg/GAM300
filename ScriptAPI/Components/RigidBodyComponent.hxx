#pragma once

#include "ComponentBase.hxx"
#include "TransformComponent.hxx"

namespace ScriptAPI
{
	public value class RigidBodyComponent : ComponentBase
	{
	public:
		enum class MotionType
		{
			STATIC,
			KINEMATIC,
			DYNAMIC
		};

		void MoveKinematic(Vector3 inTargetPosition, Vector4 inTargetRotation, float inDeltaTime);
		void SetLinearAndAngularVelocity(Vector3 linearVelocity, Vector3 angularVelocity);
		void GetLinearAndAngularVelocity(Vector3& outLinearVelocity, Vector3& outAngularVelocity);
		void SetLinearVelocity(Vector3 inLinearVelocity);
		Vector3 GetLinearVelocity();
		void AddLinearVelocity(Vector3 inLinearVelocity);
		void AddLinearAndAngularVelocity(Vector3 inLinearVelocity, Vector3 inAngularVelocity);
		void SetAngularVelocity(Vector3 inAngularVelocity);
		Vector3 GetAngularVelocity();
		Vector3 GetPointVelocity(Vector3& inPoint);
		void SetPositionRotationAndVelocity(Vector3 inPosition, Vector4 inRotation, Vector3 inLinearVelocity, Vector3 inAngularVelocity);
		void SetPosition(Vector3 inPosition);
		void SetRotation(Quaternion inRotation);
		// FORCES
		void AddForce(Vector3 inForce);
		void AddForce(Vector3 inForce, Vector3 inPoint);
		void AddTorque(Vector3 inTorque);
		void AddForceAndTorque(Vector3 inForce, Vector3 inTorque);

		// IMPLUSES
		void AddImpulse(Vector3 inImpulse);
		void AddImpulse(Vector3 inImpulse, Vector3 inPoint);
		void AddAngularImpulse(Vector3 inAngularImpulse);

		// RESTITUTION
		void SetRestitution(float inRestitution);
		float GetRestitution();

		// RESTITUTION
		void SetFriction(float inFriction);
		float GetFriction();

		// RESTITUTION
		void SetGravityFactor(float inGravityFactor);
		float GetGravityFactor();

		// Raycasting (Scripting)
		bool IsRayHit();

		virtual TDS::EntityID GetEntityID();
		virtual void SetEntityID(TDS::EntityID ID);

		virtual void SetEnabled(bool enabled);
		virtual bool GetEnabled();

		TransformComponent transform;
		GameObject^ gameObject;

		// SENSOR
		bool IsSensorActivated();

	internal:
		RigidBodyComponent(TDS::EntityID ID);

	private:
		TDS::EntityID entityID;
	};
}