#ifndef ANIMATION_H
#define ANIMATION_H

#include "AnimationInfo.h"
namespace TDS {

	class AnimationPlayer
	{
	public:
		DLL_API void setAnimation(const Animation& animation);
		void returnToBindPose();
		void Update(float dt, float speed);

		// get list of transforms for the all of the bones at the current point of the animation.
		std::vector<Mat4>* getCurrentBonesMatrices() { return &m_BonesMatrices; }
		const std::string& getName() { return m_pAnimation->m_name; }

	private:

		void processNode(const AnimationNodes& _animNode, Mat4 parentMat, bool animated);
		Mat4 boneTransform(const AnimationNodes& _animNode);

		std::vector<Mat4> m_BonesMatrices{};
		const Animation* m_pAnimation{};
		double m_CurrentTime{ 0 };
	};

	//FOR SINGLE NODE MESH, DO NOT USE THIS IF UR MESH GOT CHILD MESH, IF NOT I WILL CONVERT IT TO A SINGLE MESH
	class ModelAnimationPlayer
	{
	public:
		DLL_API void SetAnimation(const BonelessAnimation& modelAnim);


		void		SetAbsTransform(Mat4& transform)
		{
					m_AbsTransform = transform;
		}

		void		SetOriginalTransform(Mat4& transform)
		{
					m_OriginalTransform = transform;
		}

		void		ResetTransformation();
		void		Update(float dt, float speed);

		inline Mat4& GetAbsoluteTransform()
		{
			return m_AbsTransform;
		}
	private:
		void					UpdateChannel(const BonelessAnimationNodes& _animNode, bool animated);
		Mat4					ModelTransform(const BonelessAnimationNodes& _animNode);
		Mat4					m_AbsTransform;
		Mat4					m_OriginalTransform;
		const BonelessAnimation* m_pModelAnimation;
		double					m_CurrentTime{ 0 };


	};
	
}
#endif // !ANIMATION_H
