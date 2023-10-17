#include "vulkanTools/CommandManager.h"
#include "vulkanTools/VulkanHelper.h"
#include "vulkanTools/vulkanInstance.h"
#include "Rendering/GraphicsManager.h"
#include "Logger/Logger.h"
namespace TDS
{
	CommandManager::CommandManager()
	{
	}
	CommandManager::~CommandManager()
	{
		Shutdown();
	}
	void CommandManager::Shutdown()
	{
		DestroyAllPools();
	}
	void CommandManager::Init()
	{
		m_CommandPools.resize(2);
		GeneratePool(m_CommandPools[POOLTYPE::GRAPHICS], POOLTYPE::GRAPHICS);
		GeneratePool(m_CommandPools[POOLTYPE::COMPUTE], POOLTYPE::COMPUTE);
	}
	void CommandManager::DestroyAllPools()
	{
		for (auto& pool : m_CommandPools)
		{
			//DestroyPool(pool);
			ResetCommandBuffers(pool, true);
			DestroyPool(pool);
		}
		
		
		m_CommandPools.clear();
	}

	bool CommandManager::CreateSingleUseCommandBuffer(CommandBufferInfo& info)
	{
		info.m_BeginOnCreate = true;
		info.m_GenerateNewPool = true;
		return CreateCommandBuffer(info);
	}
	bool CommandManager::CreateCommandBuffer(CommandBufferInfo& info)
	{
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();

		VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocInfo.level = info.m_ExecuteType == EXECUTE_TYPE::MAIN_COMMAND_EXECUTE ?
			VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		if (allocInfo.commandPool == nullptr)
		{
			if (info.m_GenerateNewPool)
			{
				GeneratePool(info.m_CommandBuffer.m_CommandPool, info.m_PoolType);
			}
			else
			{
				info.m_CommandBuffer.m_CommandPool = m_CommandPools[info.m_PoolType];
			}
		}

		allocInfo.commandPool = info.m_CommandBuffer.m_CommandPool;
		allocInfo.commandBufferCount = 1;
		VK_ASSERT(vkAllocateCommandBuffers(instance.getVkLogicalDevice(), &allocInfo, &info.m_CommandBuffer.m_CmdBuffer), "");
		info.m_CommandBuffer.m_State = COMMAND_BUFFER_STATE::INITIAL;

		if (info.m_Fence == nullptr)
		{
			VkFenceCreateInfo fenceCI = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			fenceCI.flags = info.m_PoolType == POOLTYPE::GRAPHICS ? 0 : VK_FENCE_CREATE_SIGNALED_BIT;
			VK_ASSERT(vkCreateFence(instance.getVkLogicalDevice(), &fenceCI, nullptr, &info.m_Fence), "");
		}

		if (info.m_BeginOnCreate)
			BeginExecution(info);
		return true;
	}
	bool CommandManager::CreateCommandBatch(std::vector<VkCommandBuffer>& buffers, POOLTYPE type, EXECUTE_TYPE executeType)
	{

		if (m_CommandPools[type])
		{
			VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
			VkCommandBufferAllocateInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };

			commandBufferInfo.commandPool = m_CommandPools[type];
			commandBufferInfo.level = executeType == EXECUTE_TYPE::MAIN_COMMAND_EXECUTE ?
				VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			commandBufferInfo.commandBufferCount = std::uint32_t(buffers.size());
			VK_ASSERT(vkAllocateCommandBuffers(instance.getVkLogicalDevice(), &commandBufferInfo, buffers.data()), "");

			return true;
		}
		return false;
	}
	void CommandManager::BeginExecution(CommandBufferInfo& info)
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		//This means that it is single use
		if (info.m_BeginOnCreate)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		else
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (info.m_ExecuteType == EXECUTE_TYPE::SECONDARY_COMMAND_EXECUTE)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		info.m_CommandBuffer.m_State = COMMAND_BUFFER_STATE::RECORDING;

		VK_ASSERT(vkBeginCommandBuffer(info.m_CommandBuffer.m_CmdBuffer, &beginInfo), "");
	}
	void CommandManager::EndExecution(CommandBufferInfo& info)
	{
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
		VK_ASSERT(vkEndCommandBuffer(info.m_CommandBuffer.m_CmdBuffer), "");
		info.m_CommandBuffer.m_State = COMMAND_BUFFER_STATE::RECORDED;


		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &info.m_CommandBuffer.m_CmdBuffer;
		if (info.m_PoolType == POOLTYPE::GRAPHICS)
		{
			//m_Mutex->unlock();

			VK_ASSERT(vkQueueSubmit(instance.getGraphicsQueue(), 1, &submitInfo, info.m_Fence), "");
			VK_ASSERT(vkWaitForFences(instance.getVkLogicalDevice(), 1, &info.m_Fence, VK_TRUE, UINT64_MAX), "");
			vkDestroyFence(instance.getVkLogicalDevice(), info.m_Fence, nullptr);

			//m_Mutex->lock();
		}
		else if (info.m_PoolType == POOLTYPE::COMPUTE)
		{
			//Should not be temp fence, computation might be consistent
			VK_ASSERT(vkResetFences(instance.getVkLogicalDevice(), 1, &info.m_Fence), "");
			VK_ASSERT(vkQueueSubmit(instance.getGraphicsQueue(), 1, &submitInfo, info.m_Fence), "");
			VK_ASSERT(vkWaitForFences(instance.getVkLogicalDevice(), 1, &info.m_Fence, VK_TRUE, UINT64_MAX), "");
			/*vkDestroyFence(instance.getVkLogicalDevice(), info.m_Fence, nullptr);*/
		}
		DestroyCommandBuffer(info.m_CommandBuffer.m_CmdBuffer, info.m_CommandBuffer.m_CommandPool);
		if (info.m_GenerateNewPool)
			DestroyPool(info.m_CommandBuffer.m_CommandPool);






	}
	VkCommandPool CommandManager::GetPool(POOLTYPE type)
	{
		return m_CommandPools[type];
	}
	void CommandManager::ResetPool(POOLTYPE pool)
	{
		ResetCommandBuffers(m_CommandPools[pool]);
	}
	void CommandManager::DestroyCommandBuffer(VkCommandBuffer& hdl, POOLTYPE poolType)
	{
		DestroyCommandBuffer(hdl, m_CommandPools[poolType]);
	}
	void CommandManager::DestroyBatch(std::vector<VkCommandBuffer>& buffers, POOLTYPE poolType)
	{
		if (m_CommandPools[poolType] && buffers.size() > 0)
		{
			VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
			vkFreeCommandBuffers(instance.getVkLogicalDevice(), m_CommandPools[poolType], static_cast<uint32_t>(buffers.size()), buffers.data());
		}

	}
	void CommandManager::GeneratePool(VkCommandPool& pool, POOLTYPE poolType)
	{
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
		VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		if (poolType == POOLTYPE::GRAPHICS)
		{
			poolInfo.queueFamilyIndex = instance.GetGraphicsQueueIndex();
		}
		else if (poolType == POOLTYPE::COMPUTE)
		{
			poolInfo.queueFamilyIndex = instance.GetComputeQueueIndex();
		}
		else if (poolType == POOLTYPE::TRANSFER)
		{
			TDS_ERROR("Transfer queue not supported now!\n");
		}

		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_ASSERT(vkCreateCommandPool(instance.getVkLogicalDevice(), &poolInfo, 0, &pool), "");

	}
	void CommandManager::DestroyPool(VkCommandPool& pool)
	{
		if (pool)
		{
			VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
			vkDestroyCommandPool(instance.getVkLogicalDevice(), pool, 0);
		}
	}
	void CommandManager::DestroyCommandBuffer(VkCommandBuffer& hdl, VkCommandPool& commandPool)
	{
		if (hdl && commandPool)
		{
			VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
			vkFreeCommandBuffers(instance.getVkLogicalDevice(), commandPool, 1, &hdl);
		}

	}
	void CommandManager::ResetCommandBuffers(VkCommandPool& pool, bool ReleaseMemory)
	{
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
		VkCommandPoolResetFlags resetFlags = ReleaseMemory ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0;
		VK_ASSERT(vkResetCommandPool(instance.getVkLogicalDevice(), pool, resetFlags), "");
	}
}