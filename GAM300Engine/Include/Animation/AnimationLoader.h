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
		std::string							m_AnimationPackName;
		//std::map<std::string, unsigned int> m_BoneMap;

		DLL_API static void Serialize(AnimationData& anim, std::string_view fileName, bool read);
		RTTR_ENABLE()
			friend void ::rttr_auto_register_reflection_function_(); template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer> friend struct rttr::detail::constructor_invoker;

	};


	struct BonelessAnimationData
	{
		std::vector<BonelessAnimation>	m_Animations;
		std::string						m_AnimationPackName;

		DLL_API static void Serialize(BonelessAnimationData& anim, std::string_view fileName, bool read);
		RTTR_ENABLE()
			friend void ::rttr_auto_register_reflection_function_(); template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer> friend struct rttr::detail::constructor_invoker;
	};



}