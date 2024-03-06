/*!*****************************************************************************
 * \file          PhysicsInterface.h
 * \author        Cheang Wen Ding
 * \par DP email: cheang.w@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          07/11/2023
 * \brief         Interfacing for CLI to call Jolt Physics function
 *******************************************************************************/

#ifndef PHYSICSINTERFACE_H
#define PHYSICSINTERFACE_H

//#include <Jolt/Physics/Body/BodyInterface.h>
#include "components/rigidBody.h"

namespace TDS
{
	/*!*************************************************************************
	 * Public Member Function
	 ***************************************************************************/
	DLL_API void MoveKinematic(RigidBody& rb, Vec3& inTargetPosition, Quat& inTargetRotation, float inDeltaTime);
	DLL_API void SetLinearAndAngularVelocity(RigidBody& rb, Vec3& inLinearVelocity, Vec3& inAngularVelocity);
	DLL_API void GetLinearAndAngularVelocity(RigidBody& rb, Vec3& outLinearVelocity, Vec3& outAngularVelocity);
	DLL_API void SetLinearVelocity(RigidBody& rb, Vec3& inLinearVelocity);
	DLL_API Vec3 GetLinearVelocity(RigidBody& rb);
	DLL_API void AddLinearVelocity(RigidBody& rb, Vec3& inLinearVelocity);
	DLL_API void AddLinearAndAngularVelocity(RigidBody& rb, Vec3& inLinearVelocity, Vec3& inAngularVelocity);
	DLL_API void SetAngularVelocity(RigidBody& rb, Vec3& inAngularVelocity);
	DLL_API Vec3 GetAngularVelocity(RigidBody& rb);
	DLL_API Vec3 GetPointVelocity(RigidBody& rb, Vec3& inPoint);
	DLL_API void SetPositionRotationAndVelocity(RigidBody& rb, Vec3& inPosition, Quat& inRotation, Vec3& inLinearVelocity, Vec3& inAngularVelocity);
	DLL_API Mat4 GetInverseInertia(RigidBody& rb);
	
	/*!*************************************************************************
	 * Position and rotation of a body
	 ***************************************************************************/
	void SetPositionAndRotation(RigidBody& rb, Vec3& inPosition, Quat& inRotation, bool inActivate);
	void SetPositionAndRotationWhenChanged(RigidBody& rb, Vec3& inPosition, Quat& inRotation, bool inActivate);
	void GetPositionAndRotation(RigidBody& rb, Vec3& outPosition, Quat& outRotation);
	DLL_API void SetPosition(RigidBody& rb, Vec3& inPosition, bool inActivate);
	Vec3 GetPosition(RigidBody& rb);
	Vec3 GetCenterOfMassPosition(RigidBody& rb);
	DLL_API void SetRotation(RigidBody& rb, Quat& inRotation, bool inActivate);
	Quat GetRotation(RigidBody& rb);
	Mat4 GetWorldTransform(RigidBody& rb);
	Mat4 GetCenterOfMassTransform(RigidBody& rb);

	/*!*************************************************************************
	 * Add forces to the body
	 ***************************************************************************/
	DLL_API void AddForce(RigidBody& rb, Vec3& inForce);
	DLL_API void AddForce(RigidBody& rb, Vec3& inForce, Vec3& inPoint);
	DLL_API void AddTorque(RigidBody& rb, Vec3& inTorque);
	DLL_API void AddForceAndTorque(RigidBody& rb, Vec3& inForce, Vec3& inTorque);
	
	/*!*************************************************************************
	 * Add an impulse to the body
	 ***************************************************************************/
	DLL_API void AddImpulse(RigidBody& rb, Vec3& inImpulse);
	DLL_API void AddImpulse(RigidBody& rb, Vec3& inImpulse, Vec3& inPoint);
	DLL_API void AddAngularImpulse(RigidBody& rb, Vec3& inAngularImpulse);
	
	/*!*************************************************************************
	 * Restitution 
	 ***************************************************************************/
	DLL_API void SetRestitution(RigidBody& rb, float inRestitution);
	DLL_API float GetRestitution(RigidBody& rb);

	/*!*************************************************************************
	 * Friction
	 ***************************************************************************/
	DLL_API void SetFriction(RigidBody& rb, float inFriction);
	DLL_API float GetFriction(RigidBody& rb);

	/*!*************************************************************************
	 * Gravity Factor
	 ***************************************************************************/
	DLL_API void SetGravityFactor(RigidBody& rb, float inGravityFactor);
	DLL_API float GetGravityFactor(RigidBody& rb);

}



#endif // !PHYSICSINTERFACE_H
