#include "Shader/ShaderLoader.h"
#include "Logger/Logger.h"
namespace TDS
{
    void ShaderLoader::DeserializeShaderReflection(const std::string_view path)
    {
        std::cout << std::filesystem::current_path() << std::endl;
        std::ifstream file(path.data(), std::ios::binary);
        if (!file.is_open())
        {
            TDS_ERROR("Failed to deserialize shader reflection");
            return;
        }
        ShaderMetaData metadata{};
        std::size_t dataSize;
        file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

        for (std::size_t i = 0; i < dataSize; ++i)
        {
            int32_t stageType;
            file.read(reinterpret_cast<char*>(&stageType), sizeof(stageType));
            SHADER_FLAG flag = static_cast<SHADER_FLAG>(stageType);

            auto readString = [&file]() -> std::string
            {
                std::size_t length;
                file.read(reinterpret_cast<char*>(&length), sizeof(length));
                std::string str(length, '\0');
                file.read(str.data(), length);
                return str;
            };

            std::string key = readString();
            std::string shaderName = readString();

            ShaderData shaderData{};
            shaderData.m_StageType = flag;
            shaderData.m_ShaderName = std::move(shaderName);

            file.read(reinterpret_cast<char*>(&shaderData.m_ReflectedData.m_PushConstant), sizeof(PushContantReflect));

            auto readSamplerBufferMap = [&file, &readString](std::map<std::string, SamplerBuffer>& map)
            {
                std::size_t mapSize;
                file.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

                for (std::size_t i = 0; i < mapSize; ++i)
                {
                    std::string key = readString();
                    SamplerBuffer value;
                    file.read(reinterpret_cast<char*>(&value), sizeof(SamplerBuffer));
                    map.emplace(std::move(key), std::move(value));
                }
            };

            readSamplerBufferMap(shaderData.m_ReflectedData.m_ImageSamplers);
            readSamplerBufferMap(shaderData.m_ReflectedData.m_StorageImages);

            std::size_t uniformSize{};
            file.read(reinterpret_cast<char*>(&uniformSize), sizeof(uniformSize));

            for (std::size_t j = 0; j < uniformSize; ++j)
            {
                std::string key = readString();
                Uniform uniform;
                file.read(reinterpret_cast<char*>(&uniform.m_BufferType), sizeof(BUFFER_TYPE));
                file.read(reinterpret_cast<char*>(&uniform.m_BindingPoint), sizeof(uint32_t));
                uniform.m_InstanceName = readString();
                file.read(reinterpret_cast<char*>(&uniform.m_Size), sizeof(size_t));

                std::size_t bufferVariableSize{};
                file.read(reinterpret_cast<char*>(&bufferVariableSize), sizeof(bufferVariableSize));

                for (std::size_t k = 0; k < bufferVariableSize; ++k)
                {
                    BufferVariable bufferVariable;
                    file.read(reinterpret_cast<char*>(&bufferVariable.m_Size), sizeof(size_t));
                    file.read(reinterpret_cast<char*>(&bufferVariable.m_Offset), sizeof(size_t));
                    bufferVariable.m_Name = readString();
                    file.read(reinterpret_cast<char*>(&bufferVariable.m_Var), sizeof(VAR_TYPE));
                    uniform.m_UniformsBuffer.push_back(std::move(bufferVariable));
                }

                shaderData.m_ReflectedData.m_UniformBuffers.emplace(std::move(key), std::move(uniform));
            }

            metadata.m_ShaderDatas.emplace(std::move(key), std::move(shaderData));
        }

        file.close();
        m_reflected = metadata;
        
    }
    void ShaderLoader::LoadShader(std::vector<std::uint32_t>& SPIRV, const std::string_view path)
    {
        std::ifstream in(path.data(), std::ios::in | std::ios::binary);
        //std::cout << std::filesystem::current_path() << std::endl;
        if (in.is_open())
        {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);
            SPIRV.resize(size / sizeof(uint32_t));
            in.read((char*)SPIRV.data(), size);
        }
        in.close();
    }

}