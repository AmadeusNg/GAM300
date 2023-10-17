/*!*****************************************************************************
 * \file          GraphicsAllocator.cpp
 * \author        Eugene Ho Shu Sheng
 * \par DP email: shushengeugene.ho@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         This file contains the implementation of 
 *				  the GraphicsAllocator class.
 *******************************************************************************/
#define VMA_IMPLEMENTATION
#include "vulkanTools/GraphicsAllocator.h"
#include "vulkanTools/vulkanInstance.h"


namespace TDS
{
	GraphicsAllocator GraphicsAllocator::m_instance;
	GraphicsAllocator::GraphicsAllocator() :m_TotalAllocatedBytes(0), m_VulkanAllocator(0)
	{
	}
	GraphicsAllocator::~GraphicsAllocator()
	{
		ShutDown();
	}
	void GraphicsAllocator::Init(VulkanInstance& instance)
	{
		VmaAllocatorCreateInfo vmaCreateinfo{};
		vmaCreateinfo.vulkanApiVersion = VK_API_VERSION_1_3;
		vmaCreateinfo.physicalDevice = instance.getVkPhysicalDevice();
		vmaCreateinfo.device = instance.getVkLogicalDevice();
		vmaCreateinfo.instance = instance.getInstance();
		vmaCreateinfo.instance = instance.getInstance();
		vmaCreateAllocator(&vmaCreateinfo, &m_VulkanAllocator);
	}

	void GraphicsAllocator::UnMapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(GraphicsAllocator::GetInstance().GetAllocator(), allocation);
	}

	void GraphicsAllocator::FreeMemory(VmaAllocation allocation)
	{
		vmaFreeMemory(GraphicsAllocator::GetInstance().GetAllocator(), allocation);
	}

	void GraphicsAllocator::ShutDown()
	{
		if (m_VulkanAllocator)
		{
			vmaDestroyAllocator(m_VulkanAllocator);
			m_VulkanAllocator = nullptr;
		}
	}



}