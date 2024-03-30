#ifndef ANIMATION_INFO_H
#define ANIMATION_INFO_H

#include <rttr/registration.h>
#include <rttr/registration_friend.h>
#include "TDSMath.h"
namespace TDS {


	struct AnimationVertex
	{
		Vec3				m_Position;
		Vec3				m_Bitangent;
		Vec3				m_Tangent;
		Vec3				m_Normal;
		Vec2				m_UV;
		Vec4				m_Color;
		std::vector <unsigned int> m_BoneIDs{};
		std::vector <float>		   m_BoneWeights{};

		/*RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND*/

	};

	//node transform tree of the animation model. Store the relative positions of meshes in the model
	struct AnimNode
	{
		Mat4			 m_Transform;
		Mat4			 m_boneOffset;
		std::vector<int> m_Child;
		int				 m_ParentNode;
		int				 m_BoneID = -1;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};

	struct AnimationMesh
	{
		std::vector<AnimationVertex> m_AnimVertex{};
		std::vector<unsigned int> m_Indices{};
		// The bind transform is the natural transform of the model without the influence of animations.
		// Leave as the identity matrix if the model is not animated.
		Mat4 m_BindTransform{};

		/*RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND*/

	};

	//Key frames of the state of the model's bones
	struct AnimKeyFrame
	{
		double m_time;
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};

	struct AnimPos : public AnimKeyFrame
	{
		Vec3 m_Pos{};
		RTTR_ENABLE(AnimKeyFrame)
		RTTR_REGISTRATION_FRIEND
	};

	struct AnimRotQ : public AnimKeyFrame
	{
		Quat m_RotQ;
		RTTR_ENABLE(AnimKeyFrame)
		RTTR_REGISTRATION_FRIEND
	};

	struct AnimScale : public AnimKeyFrame
	{
		Vec3 m_Scale;
		RTTR_ENABLE(AnimKeyFrame)
		RTTR_REGISTRATION_FRIEND
	};

	//a frame of animation for all the bones.
	struct AnimationNodes
	{
		AnimNode				m_ModelNode;
		std::vector<AnimPos>	m_positions;
		std::vector<AnimRotQ>	m_rotationsQ;
		std::vector<AnimScale>	m_scalings;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};


	//An animation for a 3D model.
	struct Animation
	{
		std::string m_name;
		double m_duration;
		double m_ticks;
		std::vector<AnimationNodes> m_nodes;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};


	struct BonelessAnimationNodes
	{
		std::string m_name; 
		std::vector<AnimPos>	m_positions;
		std::vector<AnimRotQ>	m_rotationsQ;
		std::vector<AnimScale>	m_scalings;
	};

	struct BonelessAnimation
	{
		float m_duration; 
		float m_ticksPerSecond; 
		std::vector<BonelessAnimationNodes> m_channels; 
	};


	//struct AnimationModel
	//{
	//	std::vector<AnimationMesh> m_Meshes{};
	//	std::vector<AnimNode> m_Nodes{};

	//	std::map<std::string, int> nodeMap{};
	//	std::map<std::string, int> m_BoneMap{};
	//	std::vector<Mat4>		   m_Bones{};
	//	std::vector<Animation>     m_Animations{};

	//	/*RTTR_ENABLE()
	//	RTTR_REGISTRATION_FRIEND*/
	//};
}

#endif
