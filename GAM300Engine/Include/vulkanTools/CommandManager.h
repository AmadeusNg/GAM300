#pragma once
#pragma once
#include <vulkan/vulkan.h>
#include "dotnet/ImportExport.h"
namespace TDS
{
	enum POOLTYPE : std::int32_t
	{
		GRAPHICS = 0,
		COMPUTE,
		TRANSFER
	};
	enum class EXECUTE_TYPE
	{
		MAIN_COMMAND_EXECUTE,
		SECONDARY_COMMAND_EXECUTE
	};

	enum class COMMAND_BUFFER_STATE
	{
		INITIAL = 0,
		RECORDING,
		RECORDED,
		SUBMITTED,
		INVALID
	};

	struct CommandBufferHandle
	{
		VkCommandBuffer			m_CmdBuffer = nullptr;
		VkCommandPool			m_CommandPool = nullptr;
		VkFence					m_UsageFence = nullptr;
		COMMAND_BUFFER_STATE	m_State = COMMAND_BUFFER_STATE::INVALID;
	};

	struct CommandBufferInfo
	{
		bool					m_GenerateNewPool = true;
		bool					m_BeginOnCreate = true;
		POOLTYPE				m_PoolType = POOLTYPE::GRAPHICS;
		CommandBufferHandle		m_CommandBuffer;
		EXECUTE_TYPE			m_ExecuteType = EXECUTE_TYPE::MAIN_COMMAND_EXECUTE;
		VkFence					m_Fence = nullptr;
	};


	/*
		This is for your single use mostly.
		Only use this for copy commands.
		Or like what we gonna do is use this for off screen rendering. Where you rendering operation have to be sequential
		For direct image rendering which clearly uses double buffering, DO NOT USE THIS.

	*/

	class DLL_API CommandManager
	{
	public:
		CommandManager();
		~CommandManager();

		void								Shutdown();
		void								Init();
		void								DestroyAllPools();
		bool								CreateSingleUseCommandBuffer(CommandBufferInfo& info);
		bool								CreateCommandBuffer(CommandBufferInfo& info);
		bool								CreateCommandBatch(std::vector<VkCommandBuffer>& buffers, POOLTYPE type, EXECUTE_TYPE executeType = EXECUTE_TYPE::MAIN_COMMAND_EXECUTE);
		void								BeginExecution(CommandBufferInfo& info);
		void								EndExecution(CommandBufferInfo& info);
		VkCommandPool						GetPool(POOLTYPE type);
		void								ResetPool(POOLTYPE poolType);
		void								DestroyCommandBuffer(VkCommandBuffer& hdl, POOLTYPE poolType);
		void								DestroyBatch(std::vector<VkCommandBuffer>& buffers, POOLTYPE poolType);
		void								GeneratePool(VkCommandPool& pool, POOLTYPE poolType);
	private:
		std::vector<VkCommandPool>			m_CommandPools;

		void								DestroyPool(VkCommandPool& pool);
		void								DestroyCommandBuffer(VkCommandBuffer& hdl, VkCommandPool& commandPool);
		void								ResetCommandBuffers(VkCommandPool& pool, bool ReleaseMemory = false);




	};


}