/*!*************************************************************************
****
\file transform.h
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program declares the functions in the transform component class
****************************************************************************
***/

#ifndef TRANSFORMCOMPONENT
#define TRANSFORMCOMPONENT

#include "TDSMath.h"
#include "ecs/ecs.h"

namespace TDS
{
	DLL_API class Transform : public IComponent
	{
	public:
		/*!*************************************************************************
		Initializes the Transform component when created
		****************************************************************************/
		DLL_API Transform();
		/*!*************************************************************************
		Initializes the Transform component when created, given another Transform
		component to move (for ECS)
		****************************************************************************/
		DLL_API Transform(Transform&& toMove) noexcept;
		/*!*************************************************************************
		Destructor for the Transform component class
		****************************************************************************/
		DLL_API ~Transform() = default;
		/*!*************************************************************************
		Deserializes the WinData component
		****************************************************************************/
		DLL_API virtual bool Deserialize(const rapidjson::Value& obj);
		/*!*************************************************************************
		Serializes the Transform component
		****************************************************************************/
		DLL_API virtual bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const;

		/*!*************************************************************************
		Getter and setter functions for the variables in the Transform component 
		class
		****************************************************************************/
		DLL_API Vec3& GetPosition() { return mPosition;  }
		DLL_API void SetPosition(Vec3 position) { mPosition = position; }
		DLL_API void SetPosition(float positionX, float positionY, float positionZ) { mPosition = Vec3(positionX, positionY, positionZ); }

		DLL_API Vec3& GetScale() { return mScale; }
		DLL_API void SetScale(Vec3 scale) { mScale = scale; }
		DLL_API void SetScale(float scaleX, float scaleY, float scaleZ) { mScale = Vec3(scaleX, scaleY, scaleZ); }

		DLL_API float& GetRotation() { return mRotation; }
		DLL_API void SetRotation(float rotation) { mRotation = rotation; }

	private:
		Vec3 mPosition;
		Vec3 mScale;
		float mRotation;
	};

	DLL_API Transform* GetTransform(EntityID entityID);
}

#endif // TRANSFORMCOMPONENT