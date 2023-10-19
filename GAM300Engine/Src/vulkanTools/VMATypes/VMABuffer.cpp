#include "vulkanTools/VMATypes/VMABuffer.h"
#include "vulkanTools/vulkanInstance.h"
#include "vulkanTools/GraphicsAllocator.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/CommandManager.h"
namespace TDS
{

	VMABuffer::VMABuffer()
	{
	}
	VMABuffer::~VMABuffer()
	{
		DestroyBuffer();
	}
	void* VMABuffer::Map()
	{
		std::uint8_t* request{nullptr};
		GraphicsAllocator::MapMemory(request, m_Allocation);
		return request;
	}
	void VMABuffer::UnMap()
	{

		if (m_MappedMemory)
		{
			GraphicsAllocator::UnMapMemory(m_Allocation);
			m_MappedMemory = nullptr;
		}
	}
	void VMABuffer::CreateBuffer(std::uint64_t size, VkBufferUsageFlags bufferUsage, std::int32_t VmaUsage, void* data)
	{
		VkDeviceSize bufferSize = size;
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = bufferUsage;
		bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;


		m_Allocation = GraphicsAllocator::Allocate(&bufferInfo, (VmaMemoryUsage)VmaUsage, m_Buffer);
		m_BufferSize = size;
		if (data)
			MapData(data, m_BufferSize);
	}
	VMABuffer VMABuffer::CreateStagingBuffer(size_t size, VulkanInstance& instance, void* data)
	{
		VMABuffer StagingBuffer;
		VmaMemoryUsage usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		StagingBuffer.CreateBuffer(size, usageFlags, (VmaMemoryUsage)usage, data);
		return StagingBuffer;
	}
	void VMABuffer::MappedStaging(size_t size, VkBufferUsageFlags usage, VulkanInstance& instance, void* data)
	{
		VMABuffer Staging = CreateStagingBuffer(size, instance, data);

		CreateBuffer(size, usage, VMA_MEMORY_USAGE_GPU_ONLY);
		CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();
		CommandBufferInfo cmdBufferInfo = {};
		cmdMgr.CreateSingleUseCommandBuffer(cmdBufferInfo);

		VkBufferCopy copyBuffer{};

		copyBuffer.size = m_BufferSize;

		vkCmdCopyBuffer(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, Staging.m_Buffer, m_Buffer, 1, &copyBuffer);

		cmdMgr.EndExecution(cmdBufferInfo);


	}
	void VMABuffer::MapData(const void* data, size_t size, std::uint32_t offset)
	{
		m_MappedMemory = Map();
		if (m_MappedMemory)
		{
			m_MappedMemory = reinterpret_cast<std::int8_t*>(m_MappedMemory) + offset;
			std::memcpy(m_MappedMemory, data, size);
			UnMap();
		}
	}
	void VMABuffer::DestroyBuffer()
	{
		/*
			Double check
		*/

		UnMap();

		if (m_Allocation)
		{
			GraphicsAllocator::FreeBuffer(m_Buffer, m_Allocation);
			m_BufferSize = 0;
			m_Buffer = 0;
			m_Allocation = 0;
		}

	}
	size_t VMABuffer::GetBufferSize() const
	{
		return m_BufferSize;
	}
	const VkBuffer& VMABuffer::GetBuffer()
	{
		return m_Buffer;
	}
	const std::uint32_t& VMABuffer::getDataCount()
	{
		return m_DataCnt;
	}
	void VMABuffer::CreateVertexBuffer(size_t size, bool is_static, void* data)
	{
		VkBufferUsageFlags flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if (is_static)
		{
			MappedStaging(size, flags, GraphicsManager::getInstance().getVkInstance(), data);
		}
		else
		{
			CreateBuffer(size, flags, VMA_MEMORY_USAGE_CPU_TO_GPU, data);
		}


	}
	void VMABuffer::UpdateVertexData(void* data, size_t size, std::uint32_t offset)
	{
		MapData(data, size, offset);
	}
	void VMABuffer::CreateIndexBuffer(size_t indexCnt, bool is_static, std::uint32_t* indices)
	{
		VkBufferUsageFlags flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		size_t Size = static_cast<size_t>(indexCnt * sizeof(std::uint32_t));
		if (is_static)
		{
			MappedStaging(Size, flags, GraphicsManager::getInstance().getVkInstance(), indices);
		}
		else
		{
			CreateBuffer(Size, flags, VMA_MEMORY_USAGE_CPU_TO_GPU, indices);
		}

	}
	void VMABuffer::UpdateIndexData(std::uint32_t* indices, size_t indexCnt, std::uint32_t offset)
	{
		size_t DataInBytes = size_t(sizeof(std::uint32_t) * indexCnt);
		MapData(indices, DataInBytes, offset);
	}
	void VMABuffer::SetDataCnt(std::uint32_t cnt)
	{
		m_DataCnt = cnt;
	}
}