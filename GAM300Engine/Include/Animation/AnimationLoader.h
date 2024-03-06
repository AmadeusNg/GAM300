#pragma once
#include <Animation/Animation.h>
#include "dotnet/ImportExport.h"
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
#define ANIM_PATH "../assets/animations/"




namespace TDS
{
	struct AnimationData
	{
		std::vector<Animation>				m_Animations;
		std::vector<Mat4>					m_Bones;
		std::map<std::string, unsigned int> m_BoneMap;

		DLL_API static void Serialize(AnimationData& anim, std::string_view fileName, bool read);
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND

	};


	



}