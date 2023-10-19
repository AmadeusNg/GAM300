#pragma once
#include "ShaderData.h"
#include "dotnet/ImportExport.h"
#define SHADER_DIRECTORY "../assets/shaders/"
#define REFLECTED_BIN "../assets/shaders/AllShaders.bin"
namespace TDS
{


	class ShaderLoader
	{
	public:
		DLL_API void							DeserializeShaderReflection(const std::string_view path);
		DLL_API static void						LoadShader(std::vector<std::uint32_t>& SPIRV, const std::string_view path);
		inline ShaderMetaData&					getReflectedLookUp()
		{
			return m_reflected;
		}

		inline static
			std::shared_ptr<ShaderLoader>		GetInstance()
		{
			if (!m_Instance)
				m_Instance = std::make_shared<ShaderLoader>();

			return m_Instance;
		}
		DLL_API static void AddToGlobalBufferList(std::string_view globalBufferName);

	private:
		inline static
			std::shared_ptr<ShaderLoader>		m_Instance;
		ShaderMetaData							m_reflected;




	};
}