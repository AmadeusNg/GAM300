#include "Serialization/BinarySerializor.h"
#include "Logger/Logger.h"

namespace TDS
{
	BinarySerializor::BinarySerializor():m_OutStream(), m_InputStream()
	{
	}
	BinarySerializor::~BinarySerializor()
	{
	}
	BinarySerializor::ERROR_TYPE BinarySerializor::Open(std::string_view fileName, bool read)
	{

		if (read)
		{
			m_InputStream = new std::ifstream(fileName.data(), std::ios::binary);

			if (!*m_InputStream)
			{
				return ERROR_TYPE::FAILED;
			}
		}
		else
		{
			m_OutStream = new std::ofstream(fileName.data(), std::ios::binary);
			if (!*m_OutStream)
			{
				return ERROR_TYPE::FAILED;
			}
		}

		return ERROR_TYPE::SUCCESS;
	}
	void BinarySerializor::Close(bool read)
	{
		if (read == false)
		{
			m_OutStream->close();
			delete m_OutStream;
		}
		else
		{
			m_InputStream->close();
			delete m_InputStream;
		}
	}
	bool BinarySerializor::WriteVariant(const rttr::variant& var)
	{
		rttr::variant currVar = var;
		rttr::type t = var.get_type();
		while (t.is_wrapper())
		{
			t = t.get_wrapped_type();
			currVar = currVar.extract_wrapped_value();
		}

		if (WriteBasicTypes(currVar))
		{
		}
		else if (currVar.is_sequential_container())
		{
			WriteSequential(currVar);
		}
		else if (currVar.is_associative_container())
		{
			TDS_ERROR("Binary format doesnt support associative containers!\n");
		}
		else
		{
			auto child_props = t.get_properties();
			if (!child_props.empty())
			{
				SerializeRecursion(var);
			}
			else
			{
				TDS_ERROR("{} is an unknown type\n", currVar.get_type().get_name().to_string());
				return false;
			}
		}
		return true;
	}
	bool BinarySerializor::WriteBasicTypes(const rttr::variant& var)
	{
		rttr::type t = var.get_type();
		if (t.is_arithmetic())
		{
			if (t == rttr::type::get<bool>())
				WRITE_VARIANT_DATA(bool, var)
			else if (t == rttr::type::get<char>())
				WRITE_VARIANT_DATA(char, var)
			else if (t == rttr::type::get<unsigned char>())
				WRITE_VARIANT_DATA(unsigned, var)
			else if (t == rttr::type::get<int8_t>())
				WRITE_VARIANT_DATA(std::int8_t, var)
			else if (t == rttr::type::get<int16_t>())
				WRITE_VARIANT_DATA(std::int16_t, var)
			else if (t == rttr::type::get<int32_t>())
				WRITE_VARIANT_DATA(std::int32_t, var)
			else if (t == rttr::type::get<int64_t>())
				WRITE_VARIANT_DATA(std::int64_t, var)
			else if (t == rttr::type::get<uint8_t>())
				WRITE_VARIANT_DATA(std::uint8_t, var)
			else if (t == rttr::type::get<uint16_t>())
				WRITE_VARIANT_DATA(std::uint16_t, var)
			else if (t == rttr::type::get<uint32_t>())
				WRITE_VARIANT_DATA(std::uint32_t, var)
			else if (t == rttr::type::get<uint64_t>())
				WRITE_VARIANT_DATA(std::uint64_t, var)
			else if (t == rttr::type::get<float>())
				WRITE_VARIANT_DATA(float, var)
			else if (t == rttr::type::get<double>())
				WRITE_VARIANT_DATA(double, var)

			return true;

		}
		else if (t.is_enumeration())
		{
			WRITE_VARIANT_DATA(int, var)

			return true;
		}
		else if (t == rttr::type::get<std::string>())
		{
			std::string data = var.to_string();
			WriteData(data);
			return true;
		}
		return false;
	}
	void BinarySerializor::WriteSequential(const rttr::variant& var)
	{
		rttr::variant_sequential_view view = var.create_sequential_view();

		std::uint16_t length = static_cast<std::uint16_t>(view.get_size());
		m_OutStream->write(reinterpret_cast<const char*>(&length), sizeof(length));
		for (auto& x : view)
		{
			if (x.is_sequential_container())
			{
				if (x.is_sequential_container())
					WriteSequential(x);
			}
			else
			{
				rttr::variant wrappedval = x.extract_wrapped_value();
				rttr::type wrappedType = wrappedval.get_type();
				if (wrappedval.get_type().is_arithmetic() || wrappedval.get_type() == rttr::type::get<std::string>() || wrappedval.get_type().is_enumeration())
					WriteBasicTypes(wrappedval);
				else
					SerializeRecursion(wrappedval);
			}

		}
	}
	void BinarySerializor::WriteAssociative(const rttr::variant& var)
	{
		rttr::variant_associative_view view = var.create_associative_view();
		const std::string keystring("key");
		const std::string valuestring("value");

		if (view.is_key_only_type())
		{
			for (auto& x : view)
			{
				WriteVariant(x.first);
			}
		}
		else
		{
			for (auto& x : view)
			{
				WriteVariant(x.first);

				WriteVariant(x.second);

			}
		}

	}
	void BinarySerializor::SerializeRecursion(const rttr::variant& var)
	{
		rttr::type t = var.get_type();


		for (auto& x : t.get_properties(rttr::filter_item::non_public_access | rttr::filter_item::public_access | rttr::filter_item::instance_item))
		{
			rttr::variant prop = x.get_value(var);

			if (!prop.is_valid())
			{
				TDS_WARN("Failed to get value of property {} \n", x.get_name().to_string());
				continue;
			}

			if (!WriteVariant(prop))
				TDS_WARN("Failed to Serialize {} : \n", x.get_name().to_string());


		}
	}
	rttr::variant BinarySerializor::extract_basic_types(const rttr::type t)
	{
		if (t.is_arithmetic())
		{
			if (t == rttr::type::get<bool>())
			{
				bool data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<char>())
			{
				char data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<unsigned char>())
			{
				unsigned char data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<int8_t>())
			{
				int8_t data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<int16_t>())
			{
				int16_t data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<int32_t>())
			{
				int32_t data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<int64_t>())
			{
				int64_t data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<uint8_t>())
			{
				uint8_t data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<uint16_t>())
			{
				uint16_t data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<uint32_t>())
			{
				uint32_t data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<uint64_t>())
			{
				uint64_t data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<float>())
			{
				float data{};
				WriteData(data);
				return data;
			}
			else if (t == rttr::type::get<double>())
			{
				double data{};
				WriteData(data);
				return data;
			}

			return rttr::variant();

		}
		else if (t.is_enumeration())
		{
			int data{};
			WriteData(data);
			return data;

		}
		else if (t == rttr::type::get<std::string>())
		{
			std::uint16_t length;
			std::string str{};
			m_InputStream->read(reinterpret_cast<char*>(&length), sizeof(length));
			str.resize(length);
			m_InputStream->read(&str[0], length);
			return str;
		}
		return rttr::variant();
	}
	void BinarySerializor::DeserializeRecursion(rttr::instance m_inst)
	{
		rttr::instance obj = m_inst.get_type().get_raw_type().is_wrapper() ? m_inst.get_wrapped_instance() : m_inst;
		const auto prop_list = obj.get_derived_type().get_properties();

		for (auto prop : prop_list)
		{
			const rttr::type value_t = prop.get_type();
			
			rttr::variant var{};
			if (value_t.is_sequential_container())
			{
				var = prop.get_value(obj);
				auto view = var.create_sequential_view();
				ReadSequential(view);
				prop.set_value(obj, var);
			}
			else if (value_t.is_arithmetic() || value_t == rttr::type::get<std::string>() || value_t.is_enumeration())
			{
				
				rttr::variant var = extract_basic_types(value_t);
				if (var.convert(value_t))
					prop.set_value(obj, var);
			}
			else
			{
				rttr::variant var = prop.get_value(obj);
				DeserializeRecursion(var);
				prop.set_value(obj, var);
			}
			
			
		}
	}
	void BinarySerializor::ReadSequential(rttr::variant_sequential_view& view)
	{
		std::uint32_t size{};
		m_InputStream->read(reinterpret_cast<char*>(&size), sizeof(size));
		view.set_size(size);
		const rttr::type array_value_type = view.get_rank_type(1);
		for (size_t i = 0; i < size; ++i)
		{
			auto variant = view.get_value(i);
			rttr::type value_type = view.get_type();
			if (variant.is_sequential_container())
			{
				TDS_ERROR("Binary format doesnt support double containers!\n");
			}
			else if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() || value_type.is_enumeration())
			{
				rttr::variant extracted_value = extract_basic_types(value_type);
				if (extracted_value.convert(array_value_type))
					view.set_value(i, extracted_value);
			}
			else
			{
				rttr::variant tempObj = view.get_value(i);
				rttr::variant wrappedObj = tempObj.extract_wrapped_value();
				DeserializeRecursion(wrappedObj);
				view.set_value(i, wrappedObj);
			}
		}
	}
}