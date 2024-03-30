#include "vulkanTools/VertexTypes.h"
namespace TDS
{
	VertexBufferElement::VertexBufferElement(VAR_TYPE type, std::string_view varName) :m_DataType(type),
		m_offset(0), m_Size(GetVarSize(type)), m_VarName(varName)
	{
	}
	std::uint32_t GetVarSize(VAR_TYPE dataType)
	{
		switch (dataType)
		{
		case VAR_TYPE::Float:
			return 4;
		case VAR_TYPE::VEC2:
			return 4 * 2;
		case VAR_TYPE::VEC3:
			return 4 * 3;
		case VAR_TYPE::VEC4:
			return 4 * 4;
		case VAR_TYPE::MAT3:
			return 4 * 3 * 3;
		case VAR_TYPE::MAT4:
			return 4 * 4 * 4;
		case VAR_TYPE::BYTE:
			return 1;
		case VAR_TYPE::BOOL:
			return 1;
		default:
			return 0;
		}
	}
	VkFormat ConvertToVkFormat(VAR_TYPE type)
	{
		switch (type)
		{
		case VAR_TYPE::Float:
		{
			return VK_FORMAT_R32_SFLOAT;
		}
		case VAR_TYPE::VEC2:
		{
			return VK_FORMAT_R32G32_SFLOAT;
		}
		case VAR_TYPE::MAT3:
		case VAR_TYPE::VEC3:
		{
			return VK_FORMAT_R32G32B32_SFLOAT;
		}
		case VAR_TYPE::VEC4:
		case VAR_TYPE::MAT4:
		{
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		case VAR_TYPE::BOOL:
		{
			return VK_FORMAT_R32_SINT;
		}
		case VAR_TYPE::BYTE:
		{
			return VK_FORMAT_R8G8B8A8_UINT;
		}

		}

		return VK_FORMAT_R32G32B32_SFLOAT;
	}

	
	VertexLayout::VertexLayout() :m_MemberElements{}, m_Stride(0)
	{
	}
	VertexLayout::VertexLayout(const std::initializer_list<VertexBufferElement>& elements)
		:m_MemberElements(elements)
	{
		std::uint32_t offset = 0;
		m_Stride = 0;

		
		for (auto& elems : m_MemberElements)
		{
			elems.m_offset = offset;
			m_Stride += elems.m_Size;
			offset += elems.m_Size;
		}
		
	}
	VertexLayout::VertexLayout(const std::initializer_list<std::pair<int, VertexBufferElement>>& bindingPair)
	{
		std::uint32_t offset = 0;
		m_Stride = 0;
		unsigned index = 0;

		for (auto& pair : bindingPair)
		{
			m_MemberElements.push_back(pair.second);
			auto& newElem = m_MemberElements.back();
			m_BindingToElements[pair.first] = index++;


			
			newElem.m_offset = offset;
			m_Stride += newElem.m_Size;
			offset += newElem.m_Size;
			
			
		}

	}
	VertexLayout::~VertexLayout()
	{
	}
}