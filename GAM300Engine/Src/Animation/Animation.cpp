#include "Animation/Animation.h"

namespace TDS {

	ModelAnimation::ModelAnimation(std::vector<Mat4> _bones, Animation animation)
	{
		this->m_Bones = _bones;
		this->m_Animation = animation;
		returnToBindPose();
	}

	void ModelAnimation::returnToBindPose()
	{
		processNode(m_Animation.m_nodes[0], Mat4::identity(), false);
	}

	void ModelAnimation::Update(float dt)
	{
		m_CurrentTime = fmod(m_CurrentTime + (dt * m_Animation.m_ticks), m_Animation.m_duration);
		processNode(m_Animation.m_nodes[0], Mat4::identity(), true);


	}

	void ModelAnimation::processNode(const AnimationNodes& _animNode, Mat4 parentMat, bool animated)
	{
		Mat4 nodeMatrix = _animNode.m_ModelNode.m_Transform;

		if (_animNode.m_ModelNode.m_BoneID != -1)
		{
			if (animated)
				nodeMatrix = boneTransform(_animNode);
			
			m_Bones[_animNode.m_ModelNode.m_BoneID] =
				parentMat * nodeMatrix * _animNode.m_ModelNode.m_boneOffset;
		}

		//if happened to have child and parent nodes, recursive iterate it
		nodeMatrix = parentMat * nodeMatrix;
		for (const auto& childID : _animNode.m_ModelNode.m_Child)
			processNode(m_Animation.m_nodes[childID], nodeMatrix, animated);

	}

	
	//this function handle the TRS of each keyframes at the current time
	template<typename T>
	Mat4 bone(const std::vector<T>& keyFrames, float currentTime);


	Mat4 ModelAnimation::boneTransform(const AnimationNodes& _animNode)
	{
		Mat4 matrix =
			bone(_animNode.m_positions, m_CurrentTime) *
			bone(_animNode.m_rotationsQ, m_CurrentTime) *
			bone(_animNode.m_scalings, m_CurrentTime);
		if (matrix == Mat4::identity())
			return _animNode.m_ModelNode.m_Transform;
		else
			return matrix;
	}
	//helpers f1 = frame1 , f2 = frame 2 , r = factor
	struct FrameProperties { int f1{ 0 }, f2{ 0 }; double r{ 0.f }; };


	double calculateFactor(double t1, double t2, double currentTime)
	{
		return (currentTime - t1) / (t2 - t1);
	}

	float getTime(AnimKeyFrame frame) { return frame.m_time; }

	template<typename T>
	FrameProperties interpolateFrames(const std::vector<T>& frames, float currentTime)
	{
		if (frames.size() == 1)//time to last, set back to origin state and reloop it
			return FrameProperties{ 0, 0,0 };
		
		int second{ 0 };
		
		for (int i{ 0 }; i < frames.size(); i++)
		{
			if (getTime(frames[i]) >= currentTime)
			{
				second = i;
				break;
			}
		}
		int first = (second - 1) % frames.size();
		double factor = calculateFactor(getTime(frames[first]), getTime(frames[second]), currentTime);
		return FrameProperties{ first, second, factor };

	}

	//take two values(x and y) interpolate with t
	template<typename T>
	T interpolate(const T& x, const T& y, float t)
	{
		return x * (1.f - t) + y * t;
	}

	Mat4 frameMatrix(const std::vector<AnimPos>& frames, FrameProperties fp)
	{
		Vec3 trans = interpolate<Vec3>(frames[fp.f1].m_Pos, frames[fp.f2].m_Pos, fp.r);
		return Mat4::Translate(trans);
	}

	Mat4 frameMatrix(const std::vector<AnimRotQ>& frames, FrameProperties fp)
	{
		Quat val = Quat::slerp(frames[fp.f1].m_RotQ, frames[fp.f2].m_RotQ, fp.r);
		return Quat::toMat4(val);
	}

	Mat4 frameMatrix(const std::vector<AnimScale>& frames, FrameProperties fp)
	{
		Vec3 scale = interpolate<Vec3>(frames[fp.f1].m_Scale, frames[fp.f2].m_Scale, fp.r);
		return Mat4::Scale(scale);
	}

	template<typename T>
	Mat4 bone(const std::vector<T>& frames, float currentTime)
	{
		if (frames.size() == 0)
			return Mat4::identity();

		auto interpolate = interpolateFrames(frames, currentTime);
		return frameMatrix(frames, interpolate);
	}



}