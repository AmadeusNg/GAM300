#pragma once
#include "Shader/ShaderData.h"
#include <vulkan/vulkan.h>
#include "MathCommon.h"
namespace TDS
{


	struct VertexBufferElement
	{
		VertexBufferElement(VAR_TYPE type, std::string_view varName);

		VAR_TYPE		m_DataType;
		std::uint32_t	m_offset;
		std::uint32_t	m_Size;
		std::string		m_VarName;

	};

	std::uint32_t GetVarSize(VAR_TYPE dataType);

	VkFormat ConvertToVkFormat(VAR_TYPE type);

	struct VertexLayout
	{
		VertexLayout();
		VertexLayout(const std::initializer_list<VertexBufferElement>& elements);
		VertexLayout(const std::initializer_list<std::pair<int, VertexBufferElement>>& bindingPair);
		~VertexLayout();
		std::vector<VertexBufferElement>					m_MemberElements;
		std::unordered_map<std::uint32_t, std::uint32_t>	m_BindingToElements;
		std::uint32_t										m_Stride;

	};



	struct VertexBufferInfo
	{
		bool			m_InstanceInput = false;
		VertexLayout	m_Layout{};
		std::uint32_t	m_Stride = 0;
	};

	struct ModelVertex
	{
		Vec3			m_Pos,
			m_Normals;
		Vec2			m_UV;
	};

}