#pragma once
#include "BufferInfo.h"
#include "dotnet/ImportExport.h"
namespace TDS
{
	//Global Buffer Pool IS only IF YOUR BUFFER IS SHARED THROUGHOUT ALL SHADERS, if not please dont use this

	class GlobalBufferPool
	{
	private:
		std::unordered_map<std::uint32_t, std::vector<std::shared_ptr<UBO>>>	m_SharedBufferBindings;
		std::unordered_map<std::string, std::uint32_t>							m_SharedBindingNames;
	public:
		inline static std::shared_ptr<GlobalBufferPool> m_Instance = nullptr;
		static DLL_API std::shared_ptr<GlobalBufferPool>						GetInstance()
		{
			if (m_Instance == nullptr)
			{
				m_Instance = std::make_shared<GlobalBufferPool>();
			}
			return m_Instance;
		}
		DLL_API std::shared_ptr<UBO>											GetBuffer(std::uint32_t binding);
		DLL_API std::vector<std::shared_ptr<UBO>>* GetBufferContainer(std::uint32_t binding);
		void DLL_API															AddToGlobalPool(size_t size, std::uint32_t binding, VkBufferUsageFlags usage,
			std::string_view BufferName, void* data = nullptr);
		//void DLL_API															AddToGlobalPoolByName(std::string_view BufferName, void* data = nullptr);
		DLL_API bool															BindingExist(std::uint32_t binding);
		DLL_API bool															BindingExist(std::string_view name);
		DLL_API std::uint32_t													GetBinding(std::string_view name);
		void DLL_API															Destroy();
	};
}