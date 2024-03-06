#ifndef ANIMATION_H
#define ANIMATION_H

#include "AnimationInfo.h"

namespace TDS {

	class ModelAnimation
	{
	public:
		ModelAnimation() {}
		ModelAnimation(std::vector<Mat4> _bones, Animation animation);
		void returnToBindPose();
		void Update(float dt);

		// get list of transforms for the all of the bones at the current point of the animation.
		std::vector<Mat4>* getCurrentBones() { return &m_Bones; }
		std::string getName() { return m_Animation.m_name; }

	private:

		void processNode(const AnimationNodes& _animNode, Mat4 parentMat, bool animated);
		Mat4 boneTransform(const AnimationNodes& _animNode);

		std::vector<Mat4> m_Bones{};
		Animation m_Animation{};
		double m_CurrentTime{ 0 };
	};
}
#endif // !ANIMATION_H
