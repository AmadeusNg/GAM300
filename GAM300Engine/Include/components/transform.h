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
	class Transform : public IComponent
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

		DLL_API Vec3& GetRotation() { return mRotation; }
		DLL_API void SetRotation(Vec3 rotation) { mRotation = rotation; }
		DLL_API void SetRotation(float rotationX, float rotationY, float rotationZ) { mRotation = Vec3(rotationX, rotationY, rotationZ); }

		DLL_API Mat4 GetTransformMatrix() const { return mTransformMatrix; }
		DLL_API void SetTransform(Vec3 translate, Vec3 rotate, Vec3 scale)
		{
			Quat qRot = Quat(rotate);
			Mat4 scaleM4 = Mat4::Scale(scale);
			Mat4 rotM4 = Mat4(Quat::toMat4(qRot));
			Mat4 transM4 = Mat4::Translate(translate);
			mTransformMatrix = transM4 * rotM4 * scaleM4;
		}
	private:
		Vec3 mPosition;
		Vec3 mScale;
		Vec3 mRotation;

		Mat4 mTransformMatrix;
	};

	DLL_API Transform* GetTransform(EntityID entityID);
}

#endif // TRANSFORMCOMPONENT