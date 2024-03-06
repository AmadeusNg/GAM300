#include "Animation/AnimationLoader.h"
#include "Animation/AnimationLoader.h"
#include "Serialization/JSONSerializer.h"
#include "components/CompReflectionMacro.h"


namespace TDS
{

	RTTR_REGISTRATION
	{





		/*
				std::vector<int> m_Child;
		int				 m_ParentNode;
		int				 m_BoneID = -1;
		
		*/





		RTTR_REGISTER_WITH_NAME(AnimKeyFrame, "AnimKeyFrame")
			RTTR_REGISTER_PROPERTY(AnimKeyFrame, time);

		RTTR_REGISTER_WITH_NAME(AnimPos, "AnimPos")
			RTTR_REGISTER_PROPERTY(AnimPos, Pos);

		RTTR_REGISTER_WITH_NAME(AnimRotQ, "AnimPos")
			RTTR_REGISTER_PROPERTY(AnimRotQ, RotQ);

		RTTR_REGISTER_WITH_NAME(AnimScale, "AnimScale")
			RTTR_REGISTER_PROPERTY(AnimScale, Scale);


		RTTR_REGISTER_WITH_NAME(AnimNode, "AnimNode")
			RTTR_REGISTER_PROPERTY(AnimNode, Transform)
			RTTR_REGISTER_PROPERTY(AnimNode, Child)
			RTTR_REGISTER_PROPERTY(AnimNode, ParentNode)
			RTTR_REGISTER_PROPERTY(AnimNode, BoneID);

		RTTR_REGISTER_WITH_NAME(AnimNode, "AnimNode")
			RTTR_REGISTER_PROPERTY(AnimNode, Transform)
			RTTR_REGISTER_PROPERTY(AnimNode, boneOffset)
			RTTR_REGISTER_PROPERTY(AnimNode, Child)
			RTTR_REGISTER_PROPERTY(AnimNode, ParentNode)
			RTTR_REGISTER_PROPERTY(AnimNode, BoneID);

		RTTR_REGISTER_WITH_NAME(AnimationNodes, "AnimationNodes")
			RTTR_REGISTER_PROPERTY(AnimationNodes, ModelNode)
			RTTR_REGISTER_PROPERTY(AnimationNodes, positions)
			RTTR_REGISTER_PROPERTY(AnimationNodes, rotationsQ)
			RTTR_REGISTER_PROPERTY(AnimationNodes, scalings);


		RTTR_REGISTER_WITH_NAME(Animation, "Animation")
			RTTR_REGISTER_PROPERTY(Animation, name)
			RTTR_REGISTER_PROPERTY(Animation, duration)
			RTTR_REGISTER_PROPERTY(Animation, ticks)
			RTTR_REGISTER_PROPERTY(Animation, nodes);


		RTTR_REGISTER_WITH_NAME(AnimationData, "AnimationData")
			RTTR_REGISTER_PROPERTY(AnimationData, Animations)
			RTTR_REGISTER_PROPERTY(AnimationData, Bones)
			RTTR_REGISTER_PROPERTY(AnimationData, BoneMap);

	}
		


	void AnimationData::Serialize(AnimationData& anim, std::string_view fileName, bool read)
	{
		JSONSerializer m_JsonSerializer;

		std::string fullPath = ANIM_PATH + std::string(fileName.data());

		if (m_JsonSerializer.OpenData(fullPath, read) == JSONSerializer::ERROR_TYPE::SUCCESS)
		{
			if (read)
			{
				m_JsonSerializer.StartDeserializer(&anim);
			}
			else
			{
				m_JsonSerializer.StartSerializer(&anim);
			}
			m_JsonSerializer.CloseData(read);
		}
	}
}