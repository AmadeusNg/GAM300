#pragma once
#include <Serialization/SerializerBase.h>
#include <rttr/type.h>

namespace TDS
{
#define DEFINE_WRITE_DATA(TYPE)\
template <>\
void	WriteData(TYPE& data)\
{\
		m_OutStream->write(reinterpret_cast<const char*>(&data), sizeof(TYPE));\
}
#define WRITE_VARIANT_DATA(TYPE, variant)\
{\
		TYPE data = variant.get_value<TYPE>();\
		WriteData(data);\
}\

	class BinarySerializor : public SerializerBase<BinarySerializor>
	{
		private:
			std::ofstream*							m_OutStream = nullptr;
			std::ifstream*							m_InputStream = nullptr;
	
		public:
			BinarySerializor();
			~BinarySerializor();

			ERROR_TYPE		Open(std::string_view fileName, bool read);
			void			Close(bool read);

			template <typename T>
			ERROR_TYPE		RunSerializer(T* data)
			{
				SerializeRecursion(data);
				return ERROR_TYPE::SUCCESS;
			}

			template <typename T>
			ERROR_TYPE		RunDeserializer(T* Data)
			{
				DeserializeRecursion(Data);
				return ERROR_TYPE::SUCCESS;
			}

			bool			WriteVariant(const rttr::variant& var);
			bool			WriteBasicTypes(const rttr::variant& var);
			void			WriteSequential(const rttr::variant& var);
			void			WriteAssociative(const rttr::variant& var);
			void			SerializeRecursion(const rttr::variant& var);

			template <typename T>
			void	ReadBasicType(T& data)
			{
				m_InputStream.read(reinterpret_cast<char*>(&data), sizeof(T));
			}
			rttr::variant	extract_basic_types(const rttr::type t);
			void			DeserializeRecursion(rttr::instance m_inst);
			void			ReadSequential(rttr::variant_sequential_view& view);
			void			ReadAssociative(rttr::variant_associative_view& view);




			template <typename T>
			void			WriteData(T& data)
			{
				(data);
				return;
			}


			DEFINE_WRITE_DATA(bool)
			DEFINE_WRITE_DATA(char)
			DEFINE_WRITE_DATA(std::int8_t)
			DEFINE_WRITE_DATA(std::int16_t)
			DEFINE_WRITE_DATA(std::int32_t)
			DEFINE_WRITE_DATA(std::int64_t)
			DEFINE_WRITE_DATA(std::uint8_t)
			DEFINE_WRITE_DATA(std::uint16_t)
			DEFINE_WRITE_DATA(std::uint32_t)
			DEFINE_WRITE_DATA(std::uint64_t)
			DEFINE_WRITE_DATA(float)
			DEFINE_WRITE_DATA(double)


			template <>
			void		WriteData(std::string& data)
			{
				std::uint16_t length = static_cast<std::uint16_t>(data.length());
				m_OutStream->write(reinterpret_cast<const char*>(&length), sizeof(length));
				m_OutStream->write(data.data(), length);
			}







	};
}