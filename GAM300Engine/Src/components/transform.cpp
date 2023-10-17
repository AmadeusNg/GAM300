/*!*************************************************************************
****
\file transform.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines the functions in the Transform component class
****************************************************************************
***/

#include "components/transform.h"

namespace TDS
{
	/*!*************************************************************************
	Initializes the Transform component when created
	****************************************************************************/
	Transform::Transform() : mPosition	(Vec3(0.0f, 0.0f, 0.0f)), 
							 mScale		(Vec3(1.0f, 1.0f, 0.0f)),
							 mRotation	(Vec3(0.0f, 0.f,0.0f))
	{ }

	/*!*************************************************************************
	Initializes the Transform component when created, given another Transform
	component to move (for ECS)
	****************************************************************************/
	Transform::Transform(Transform&& toMove) noexcept : mPosition	(toMove.mPosition),
														mScale		(toMove.mScale),
														mRotation	(toMove.mRotation)
	{ }

	/*!*************************************************************************
	Deserializes the WinData component
	****************************************************************************/
	bool Transform::Deserialize(const rapidjson::Value& obj)
	{
		mPosition = Vec3(obj["positionX"].GetFloat(), obj["positionY"].GetFloat(), obj["positionZ"].GetFloat());
		mScale = Vec3(obj["scaleX"].GetFloat(), obj["scaleY"].GetFloat(), obj["scaleZ"].GetFloat());
		mRotation = obj["rotation"].GetFloat();

		return true;
	}

	/*!*************************************************************************
	Serializes the Transform component
	****************************************************************************/
	bool Transform::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
	{
		writer->Key("positionX");
		writer->Double(mPosition.x);
		writer->Key("positionY");
		writer->Double(mPosition.y);
		writer->Key("positionZ");
		writer->Double(mPosition.z);

		writer->Key("scaleX");
		writer->Double(mScale.x);
		writer->Key("scaleY");
		writer->Double(mScale.y);
		writer->Key("scaleZ");
		writer->Double(mScale.z);

		writer->Key("rotationX");
		writer->Double(mRotation.x);
		writer->Key("rotationY");
		writer->Double(mRotation.y);
		writer->Key("rotationZ");
		writer->Double(mRotation.z);

		return true;
	}

	Transform* GetTransform(EntityID entityID)
	{
		return ecs.getComponent<Transform>(entityID);
	}
}
