#include "RigidBodyComponent.hxx"
#include "Physics/Interface/PhysicsInterface.h"
#include "../EngineInterface.hxx"

namespace ScriptAPI
{
	void RigidBodyComponent::MoveKinematic(Vector3 inTargetPosition, Vector4 inTargetRotation, float inDeltaTime)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inTargetPositionVec3 = TDS::floatsToVec3(inTargetPosition.X, inTargetPosition.Y, inTargetPosition.Z);
		TDS::Quat inTargetRotationQuat = TDS::floatsToQuat(inTargetRotation.X, inTargetRotation.Y, inTargetRotation.Z, inTargetRotation.W);
		TDS::MoveKinematic(*TDS::GetRigidBody(entityID), inTargetPositionVec3, inTargetRotationQuat, inDeltaTime);
	}

	void RigidBodyComponent::SetLinearAndAngularVelocity(Vector3 linearVelocity, Vector3 angularVelocity)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 linearVelocityVec3 = TDS::floatsToVec3(linearVelocity.X, linearVelocity.Y, linearVelocity.Z);
		TDS::Vec3 angularVelocityVec3 = TDS::floatsToVec3(angularVelocity.X, angularVelocity.Y, angularVelocity.Z);
		TDS::SetLinearAndAngularVelocity(*TDS::GetRigidBody(entityID), linearVelocityVec3, angularVelocityVec3);
	}

	void RigidBodyComponent::GetLinearAndAngularVelocity(Vector3& outLinearVelocity, Vector3& outAngularVelocity)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 outLinearVelocityVec3 = TDS::floatsToVec3(outLinearVelocity.X, outLinearVelocity.Y, outLinearVelocity.Z);
		TDS::Vec3 outAngularVelocityVec3 = TDS::floatsToVec3(outAngularVelocity.X, outAngularVelocity.Y, outAngularVelocity.Z);
		TDS::GetLinearAndAngularVelocity(*TDS::GetRigidBody(entityID), outLinearVelocityVec3, outAngularVelocityVec3);

		outLinearVelocity = Vector3(outLinearVelocityVec3);
		outAngularVelocity = Vector3(outAngularVelocityVec3);
	}

	void RigidBodyComponent::SetLinearVelocity(Vector3 inLinearVelocity)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inLinearVelocityVec3 = TDS::floatsToVec3(inLinearVelocity.X, inLinearVelocity.Y, inLinearVelocity.Z);
		TDS::SetLinearVelocity(*TDS::GetRigidBody(entityID), inLinearVelocityVec3);
	}

	Vector3 RigidBodyComponent::GetLinearVelocity()
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return Vector3(0.f, 0.f, 0.f);
		}

		TDS::Vec3 linearVelocity = TDS::GetLinearVelocity(*TDS::GetRigidBody(entityID));
		return Vector3(linearVelocity);
	}

	void RigidBodyComponent::AddLinearVelocity(Vector3 inLinearVelocity)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inLinearVelocityVec3 = TDS::floatsToVec3(inLinearVelocity.X, inLinearVelocity.Y, inLinearVelocity.Z);
		TDS::AddLinearVelocity(*TDS::GetRigidBody(entityID), inLinearVelocityVec3);
	}

	void RigidBodyComponent::AddLinearAndAngularVelocity(Vector3 inLinearVelocity, Vector3 inAngularVelocity)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inLinearVelocityVec3 = TDS::floatsToVec3(inLinearVelocity.X, inLinearVelocity.Y, inLinearVelocity.Z);
		TDS::Vec3 inAngularVelocityVec3 = TDS::floatsToVec3(inAngularVelocity.X, inAngularVelocity.Y, inAngularVelocity.Z);
		TDS::AddLinearAndAngularVelocity(*TDS::GetRigidBody(entityID), inLinearVelocityVec3, inAngularVelocityVec3);
	}

	void RigidBodyComponent::SetAngularVelocity(Vector3 inAngularVelocity)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inAngularVelocityVec3 = TDS::floatsToVec3(inAngularVelocity.X, inAngularVelocity.Y, inAngularVelocity.Z);
		TDS::SetAngularVelocity(*TDS::GetRigidBody(entityID), inAngularVelocityVec3);
	}

	Vector3 RigidBodyComponent::GetAngularVelocity()
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return Vector3(0.f, 0.f, 0.f);
		}

		return Vector3(TDS::GetAngularVelocity(*TDS::GetRigidBody(entityID)));
	}

	Vector3 RigidBodyComponent::GetPointVelocity(Vector3& inPoint)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return Vector3(0.f, 0.f, 0.f);
		}

		TDS::Vec3 inPointVec3 = TDS::floatsToVec3(inPoint.X, inPoint.Y, inPoint.Z);
		return Vector3(TDS::GetPointVelocity(*TDS::GetRigidBody(entityID), inPointVec3));
	}

	void RigidBodyComponent::SetPositionRotationAndVelocity(Vector3 inPosition, Vector4 inRotation, Vector3 inLinearVelocity, Vector3 inAngularVelocity)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inPositionVec3 = TDS::floatsToVec3(inPosition.X, inPosition.Y, inPosition.Z);
		TDS::Quat inRotationQuat = TDS::floatsToQuat(inRotation.X, inRotation.Y, inRotation.Z, inRotation.W);
		TDS::Vec3 inLinearVelocityVec3 = TDS::floatsToVec3(inLinearVelocity.X, inLinearVelocity.Y, inLinearVelocity.Z);
		TDS::Vec3 inAngularVelocityVec3 = TDS::floatsToVec3(inAngularVelocity.X, inAngularVelocity.Y, inAngularVelocity.Z);

		TDS::SetPositionRotationAndVelocity(*TDS::GetRigidBody(entityID), inPositionVec3, inRotationQuat, inLinearVelocityVec3, inAngularVelocityVec3);
	}

	void RigidBodyComponent::SetPosition(Vector3 inPosition)
	{
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inPositionVec3 = TDS::floatsToVec3(inPosition.X, inPosition.Y, inPosition.Z);
		TDS::SetPosition(*TDS::GetRigidBody(entityID), inPositionVec3, true);
	}

	void RigidBodyComponent::SetRotation(Quaternion inRotation)
	{
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Quat inRotationQuat = TDS::floatsToQuat(inRotation.X, inRotation.Y, inRotation.Z, inRotation.W);
		TDS::SetRotation(*TDS::GetRigidBody(entityID), inRotationQuat, true);

	}

	// FORCES
	void RigidBodyComponent::AddForce(Vector3 inForce)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inForceVec3 = TDS::floatsToVec3(inForce.X, inForce.Y, inForce.Z);
		TDS::AddForce(*TDS::GetRigidBody(entityID), inForceVec3);
	}
	void RigidBodyComponent::AddForce(Vector3 inForce, Vector3 inPoint)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inForceVec3 = TDS::floatsToVec3(inForce.X, inForce.Y, inForce.Z);
		TDS::Vec3 inPointVec3 = TDS::floatsToVec3(inPoint.X, inPoint.Y, inPoint.Z);
		TDS::AddForce(*TDS::GetRigidBody(entityID), inForceVec3, inPointVec3);
	}
	void RigidBodyComponent::AddTorque(Vector3 inTorque)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inTorqueVec3 = TDS::floatsToVec3(inTorque.X, inTorque.Y, inTorque.Z);
		TDS::AddTorque(*TDS::GetRigidBody(entityID), inTorqueVec3);
	}
	void RigidBodyComponent::AddForceAndTorque(Vector3 inForce, Vector3 inTorque)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inForceVec3 = TDS::floatsToVec3(inForce.X, inForce.Y, inForce.Z);
		TDS::Vec3 inTorqueVec3 = TDS::floatsToVec3(inTorque.X, inTorque.Y, inTorque.Z);
		TDS::AddForceAndTorque(*TDS::GetRigidBody(entityID), inForceVec3, inTorqueVec3);
	}

	// IMPLUSES
	void RigidBodyComponent::AddImpulse(Vector3 inImpulse)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inImpulseVec3 = TDS::floatsToVec3(inImpulse.X, inImpulse.Y, inImpulse.Z);
		TDS::AddImpulse(*TDS::GetRigidBody(entityID), inImpulseVec3);
	}
	void RigidBodyComponent::AddImpulse(Vector3 inImpulse, Vector3 inPoint)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inImpulseVec3 = TDS::floatsToVec3(inImpulse.X, inImpulse.Y, inImpulse.Z);
		TDS::Vec3 inPointVec3 = TDS::floatsToVec3(inPoint.X, inPoint.Y, inPoint.Z);
		TDS::AddImpulse(*TDS::GetRigidBody(entityID), inImpulseVec3, inPointVec3);
	}
	void RigidBodyComponent::AddAngularImpulse(Vector3 inAngularImpulse)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::Vec3 inAngularImpulseVec3 = TDS::floatsToVec3(inAngularImpulse.X, inAngularImpulse.Y, inAngularImpulse.Z);
		TDS::AddAngularImpulse(*TDS::GetRigidBody(entityID), inAngularImpulseVec3);
	}

	// RESTITUTION
	void RigidBodyComponent::SetRestitution(float inRestitution)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::SetRestitution(*TDS::GetRigidBody(entityID), inRestitution);
	}
	float RigidBodyComponent::GetRestitution()
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return 0.f;
		}

		return TDS::GetRestitution(*TDS::GetRigidBody(entityID));
	}

	// RESTITUTION
	void RigidBodyComponent::SetFriction(float inFriction)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::SetFriction(*TDS::GetRigidBody(entityID), inFriction);
	}
	float RigidBodyComponent::GetFriction()
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return 0.f;
		}

		return TDS::GetFriction(*TDS::GetRigidBody(entityID));
	}

	// RESTITUTION
	void RigidBodyComponent::SetGravityFactor(float inGravityFactor)
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::SetGravityFactor(*TDS::GetRigidBody(entityID), inGravityFactor);
	}
	float RigidBodyComponent::GetGravityFactor()
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return 0.f;
		}

		return TDS::GetGravityFactor(*TDS::GetRigidBody(entityID));
	}

	bool RigidBodyComponent::IsSensorActivated()
	{
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return false;
		}
		return TDS::GetRigidBody(entityID)->getSensorActivate();
	}

	// CONSTRUCTOR ===========================================================================
	RigidBodyComponent::RigidBodyComponent(TDS::EntityID ID) : entityID (ID), transform(TransformComponent(ID))
	{
		gameObject = EngineInterface::GetGameObject(ID);
	}

	void RigidBodyComponent::SetEntityID(TDS::EntityID ID)
	{
		entityID = ID;
		transform = TransformComponent(ID);
		gameObject = EngineInterface::GetGameObject(ID);
	}

	bool RigidBodyComponent::IsRayHit()
	{
		// May wanna change to a function
		if (!TDS::GetRigidBody(entityID))
		{
			// throw error instead (not sure how)
			return false;
		}

		return TDS::GetRigidBody(entityID)->getIsRayHit();
	}

	TDS::EntityID RigidBodyComponent::GetEntityID()
	{
		return entityID;
	}

	void RigidBodyComponent::SetEnabled(bool enabled)
	{
		TDS::setComponentIsEnable("Rigid Body", GetEntityID(), enabled);
	}
	bool RigidBodyComponent::GetEnabled()
	{
		return TDS::getComponentIsEnable("Rigid Body", GetEntityID());
	}
}