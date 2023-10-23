/*!*************************************************************************
****
\file FrameInfo.h
\author Ng Zuo Xian Amadeus
\par DP email: ng.z@digipen.edu
\par Course: CSD3400
\par Section: a
\date 22-9-2023
\brief  Global Structs to be used in other files if needed
****************************************************************************
***/
#pragma once
#include "camera/camera.h"
#include "Model.h"
#include <vulkan/vulkan.h>

namespace TDS {
#define Max_Lights 10 //CHANGE THIS IN POINTLIGHTVERT/FRAG SHADERS
	struct PointLight {
		Vec4 m_Position;
		Vec4 m_Color;
	};

	struct GlobalUBO {
		Mat4			m_Projection{ 1.f };
		Mat4			m_View{ 1.f };
		//Mat4			m_InverseView{1.f}; 
		Vec4			m_AmbientLightColor{ 1.f, 1.f,1.f,0.02f };
		PointLight		m_vPointLights[Max_Lights];
		//int				m_activelights;
	};

	struct FrameInfo {
		int FrameIndex;
		float FrameTime;
		VkCommandBuffer commandBuffer;
		TDSCamera camera;
		VkDescriptorSet globalDescriptorSet;
		Model& model;
		//vector of gameobj??
	};
}