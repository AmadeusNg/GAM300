#include "Serialization/JSONSerializer.h"
#include "Logger/Logger.h"

namespace TDS
{
	JSONSerializer::JSONSerializer()
	{
	}
	JSONSerializer::~JSONSerializer()
	{
	}
	JSONSerializer::ERROR_TYPE JSONSerializer::Open(std::string_view path, bool read)
	{
		if (!read)
		{
			m_OutStream = new std::ofstream(path.data());


			if (!*m_OutStream)
			{
				return ERROR_TYPE::FAILED;
			}
			
			m_StringBuffer = new rapidjson::StringBuffer();
			m_Writer = new rapidjson::PrettyWriter(*m_StringBuffer);
	
		}
		else
		{

			m_InputStream = new std::ifstream(path.data());

			if (!*m_InputStream)
			{
				return ERROR_TYPE::FAILED;
			}
			std::string jsonFile{};
			std::string line{};

			while (std::getline(*m_InputStream, line)) jsonFile += line;

			m_InputStream->close();
			m_Document = new rapidjson::Document();
			m_Document->Parse(jsonFile.c_str());
			if (m_Document->HasParseError())
			{
				return ERROR_TYPE::FAILED;
			}
			
		}

		return ERROR_TYPE::SUCCESS;
		
	}
	void JSONSerializer::Close(bool read)
	{
		if (read == false)
		{
			*m_OutStream << std::string(m_StringBuffer->GetString());
			m_StringBuffer->Clear();
			m_OutStream->close();
			delete m_OutStream;
			delete m_StringBuffer;
			delete m_Writer;
		}
		else
		{
			m_Document->Clear();
			m_InputStream->close();
			delete m_InputStream;
			delete m_Document;
		}
	}


	bool JSONSerializer::WriteVariant(const rttr::variant& var)
	{
		rttr::variant currVar = var;
		rttr::type t = var.get_type();
		while (t.is_wrapper())
		{
			t = t.get_wrapped_type();
			currVar = currVar.extract_wrapped_value();
		}
		if (WritePair(currVar))
		{
		}
		else if (WriteBasicTypes(currVar))
		{
		}
		else if (currVar.is_sequential_container())
		{
			WriteSequential(currVar);
		}
		else if (currVar.is_associative_container())
		{
			WriteAssociative(currVar);
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

	bool JSONSerializer::WriteBasicTypes(const rttr::variant& var)
	{
		rttr::type t = var.get_type();
		if (t.is_arithmetic())
		{
			if (t == rttr::type::get<bool>())
				m_Writer->Bool(var.to_bool());
			else if (t == rttr::type::get<char>())
				m_Writer->Bool(var.to_bool());
			else if (t == rttr::type::get<unsigned char>())
				m_Writer->Uint(var.to_uint8());
			else if (t == rttr::type::get<int8_t>())
				m_Writer->Int(var.to_int8());
			else if (t == rttr::type::get<int16_t>())
				m_Writer->Int(var.to_int16());
			else if (t == rttr::type::get<int32_t>())
				m_Writer->Int(var.to_int32());
			else if (t == rttr::type::get<int64_t>())
				m_Writer->Int64(var.to_int64());
			else if (t == rttr::type::get<uint8_t>())
				m_Writer->Uint(var.to_uint8());
			else if (t == rttr::type::get<uint16_t>())
				m_Writer->Uint(var.to_uint16());
			else if (t == rttr::type::get<uint32_t>())
				m_Writer->Uint(var.to_uint32());
			else if (t == rttr::type::get<uint64_t>())
				m_Writer->Uint64(var.to_uint64());
			else if (t == rttr::type::get<float>())
				m_Writer->Double(var.to_double());
			else if (t == rttr::type::get<double>())
				m_Writer->Double(var.to_double());

			return true;

		}
		else if (t.is_enumeration())
		{
			bool ok = false;
			auto result = var.to_string(&ok);
			if (ok)
			{
				m_Writer->String(var.to_string().c_str());

			}
			else
			{
				TDS_WARN("Enum type + {} is not reflective \n", t.get_name().to_string());
				m_Writer->Null();
			}

			return true;
		}
		else if (t == rttr::type::get<std::string>())
		{
			m_Writer->String(var.to_string().c_str());
			return true;
		}
		return false;
	}

	bool JSONSerializer::WriteBasicTypes(const rttr::type& t, const rttr::variant& var)
	{
		if (t.is_arithmetic())
		{
			if (t == rttr::type::get<bool>())
				m_Writer->Bool(var.to_bool());
			else if (t == rttr::type::get<char>())
				m_Writer->Bool(var.to_bool());
			else if (t == rttr::type::get<int8_t>())
				m_Writer->Int(var.to_int8());
			else if (t == rttr::type::get<int16_t>())
				m_Writer->Int(var.to_int16());
			else if (t == rttr::type::get<int32_t>())
				m_Writer->Int(var.to_int32());
			else if (t == rttr::type::get<int64_t>())
				m_Writer->Int64(var.to_int64());
			else if (t == rttr::type::get<uint8_t>())
				m_Writer->Uint(var.to_uint8());
			else if (t == rttr::type::get<uint16_t>())
				m_Writer->Uint(var.to_uint16());
			else if (t == rttr::type::get<uint32_t>())
				m_Writer->Uint(var.to_uint32());
			else if (t == rttr::type::get<uint64_t>())
				m_Writer->Uint64(var.to_uint64());
			else if (t == rttr::type::get<float>())
				m_Writer->Double(var.to_double());
			else if (t == rttr::type::get<double>())
				m_Writer->Double(var.to_double());

			return true;
		}
		else if (t.is_enumeration())
		{
			bool ok = false;
			auto result = var.to_string(&ok);
			if (ok)
			{
				m_Writer->String(var.to_string().c_str());
			}
			else
			{
				ok = false;
				auto value = var.to_uint64(&ok);
				if (ok)
					m_Writer->Uint64(value);
				else
					m_Writer->Null();
			}

			return true;
		}
		else if (t == rttr::type::get<std::string>())
		{
			m_Writer->String(var.to_string().c_str());
			return true;
		}

		return false;
	}

	void JSONSerializer::WriteSequential(const rttr::variant& var)
	{
		m_Writer->StartArray();
		rttr::variant_sequential_view view = var.create_sequential_view();

		for (auto& x : view)
		{
			if (x.is_sequential_container())
				WriteSequential(x);
			
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
		m_Writer->EndArray();
	}

	void JSONSerializer::SerializeRecursion(const rttr::variant& var)
	{

		m_Writer->StartObject();
		rttr::type t = var.get_type();


		for (auto& x : t.get_properties(rttr::filter_item::non_public_access | rttr::filter_item::public_access | rttr::filter_item::instance_item))
		{
			rttr::variant prop = x.get_value(var);

			if (!prop.is_valid())
			{
				TDS_WARN("Failed to get value of property {} \n", x.get_name().to_string());
				continue;
			}

			m_Writer->Key(x.get_name().to_string().c_str());
			if (!WriteVariant(prop))
				TDS_WARN("Failed to Serialize {} : \n", x.get_name().to_string());
			

		}
		m_Writer->EndObject();
	}

	void JSONSerializer::WriteAssociative(const rttr::variant& var)
	{
		rttr::variant_associative_view view = var.create_associative_view();
		const std::string keystring("key");
		const std::string valuestring("value");

		m_Writer->StartArray();
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
				m_Writer->StartObject();

				m_Writer->String(keystring.data(), static_cast<rapidjson::SizeType>(keystring.length()), false);
				WriteVariant(x.first);

				m_Writer->String(valuestring.data(), static_cast<rapidjson::SizeType>(valuestring.length()), false);
				WriteVariant(x.second);

				m_Writer->EndObject();
			}
		}
		m_Writer->EndArray();
	}

	rttr::variant JSONSerializer::extract_basic_types(rapidjson::Value& val)
	{
		switch (val.GetType())
		{
		case rapidjson::kStringType:
			return std::string(val.GetString());
			break;
		case rapidjson::kFalseType:
		case rapidjson::kTrueType:
		{
			return val.GetBool();
			break;
		}
		case rapidjson::kNumberType:
		{
			if (val.IsInt())
			{
				return val.GetInt();
			}
			else if (val.IsDouble())
			{
				return val.GetDouble();
			}
			else if (val.IsUint())
			{
				return val.GetUint();
			}
			else if (val.IsInt64())
			{
				return val.GetInt64();
			}
			else if (val.IsUint64())
			{
				return val.GetUint64();
			}
			break;
		}
		case rapidjson::kNullType: break;
		case rapidjson::kObjectType:
		case rapidjson::kArrayType:
			return rttr::variant();


		}
		return rttr::variant();
	}

	void JSONSerializer::DeserializeRecursion(rttr::instance inst, rapidjson::Value& json_obj)
	{
		rttr::type t = inst.get_derived_type();
		for (auto& x : t.get_properties(rttr::filter_item::non_public_access | rttr::filter_item::public_access | rttr::filter_item::instance_item))
		{

			rapidjson::Value::MemberIterator itr = json_obj.FindMember(x.get_name().data());

			if (itr == json_obj.MemberEnd())
			{
				continue;
			}
			const rttr::type value_type = x.get_type();
			rapidjson::Value& jsonValue = itr->value;
			switch (jsonValue.GetType())
			{
				case rapidjson::kArrayType:
				{
					rttr::variant var = x.get_value(inst);
					if (value_type.is_sequential_container())
					{
						ReadSequential(var, jsonValue);
					}
					else if (value_type.is_associative_container())
					{
						ReadAssociative(var, jsonValue);
					}
					x.set_value(inst, var);
					break;
				}
				case rapidjson::kObjectType:
				{
					rttr::variant var = x.get_value(inst);
					DeserializeRecursion(var, jsonValue);
					x.set_value(inst, var);
					break;
				}
				default:
				{
					rttr::variant extractedVal = extract_basic_types(jsonValue);
					if (extractedVal.convert(value_type))
						x.set_value(inst, extractedVal);
				}
			}
		}
	}

	void JSONSerializer::ReadSequential(rttr::variant& var, rapidjson::Value& json_array_value)
	{
		rttr::variant_sequential_view m_view = var.create_sequential_view();
		m_view.set_size(json_array_value.Size());
		for (rapidjson::SizeType i = 0; i < json_array_value.Size(); ++i)
		{
			if (json_array_value[i].IsArray())
			{
				rttr::variant internalVariant = m_view.get_value(i);
				ReadSequential(internalVariant, json_array_value[i]);
			}
			else if (json_array_value[i].IsObject())
			{
				rttr::variant tempObj = m_view.get_value(i);
				rttr::variant wrappedObj = tempObj.extract_wrapped_value();
				DeserializeRecursion(wrappedObj, json_array_value[i]);
				m_view.set_value(i, wrappedObj);
			}
			else
			{
				rttr::variant extractedVar = extract_basic_types(json_array_value[i]);
				if (extractedVar.convert(m_view.get_rank_type(1)))
					m_view.set_value(i, extractedVar);
			}
		}
	}

	void JSONSerializer::ReadAssociative(rttr::variant& var, rapidjson::Value& json_map)
	{
		for (rapidjson::SizeType i = 0; i < json_map.Size(); ++i)
		{
			if (json_map[i].IsObject())
			{
				rapidjson::Value::MemberIterator keyItr = json_map[i].FindMember("key");
				rapidjson::Value::MemberIterator valItr = json_map[i].FindMember("value");
				if (keyItr != json_map[i].MemberEnd() &&
					valItr != json_map[i].MemberEnd())
				{
					rttr::variant extractedKey = extract_basic_types(keyItr->value);
					if (!extractedKey.convert(var.create_associative_view().get_key_type()))
					{
						if (keyItr->value.IsObject())
						{
							rttr::constructor ctor = var.create_associative_view().get_key_type().get_constructor();
							for (auto& ctor1 : var.create_associative_view().get_key_type().get_constructors())
							{
								if (ctor1.get_instantiated_type() == var.create_associative_view().get_key_type())
								{
									ctor = ctor1;
								}
							}
							extractedKey = ctor.invoke();
							DeserializeRecursion(extractedKey, keyItr->value);
						}
					}
					rttr::variant extractedVal = extract_basic_types(valItr->value);
					if (!extractedVal.convert(var.create_associative_view().get_key_type()))
					{
						if (valItr->value.IsObject())
						{
							rttr::constructor ctor = var.create_associative_view().get_key_type().get_constructor();
							for (auto& ctor1 : var.create_associative_view().get_key_type().get_constructors())
							{
								if (ctor1.get_instantiated_type() == var.create_associative_view().get_key_type())
								{
									ctor = ctor1;
								}
							}
							extractedVal = ctor.invoke();
							DeserializeRecursion(extractedVal, valItr->value);
						}
					}
					var.create_associative_view().insert(extractedKey, extractedVal);

				}
				else
				{
					rttr::variant extract_val = extract_basic_types(json_map[i]);
					if (extract_val && extract_val.convert(var.create_associative_view().get_key_type()))
					{
						var.create_associative_view().insert(extract_val);
					}
				}
			}
		}
	}

	bool JSONSerializer::WritePair(const rttr::variant& var)
	{
		if (var.get_type().get_name() == "pair1")
		{
			//Get the first and second element of the pair
			rttr::variant first = var.get_value<std::pair<unsigned char, unsigned char>>().first;
			rttr::variant second = var.get_value<std::pair<unsigned char, unsigned char>>().second;
			//Write the first element
			m_Writer->StartObject();
			m_Writer->Key("Key");
			bool WriteKey = WriteVariant(first);
			//Write the second element
			m_Writer->Key("Value");
			bool WriteValue = WriteVariant(second);
			if (!WriteKey || !WriteValue)
			{
				TDS_ERROR("Failed to write pairs\n");
			}
			m_Writer->EndObject();
			return true;
		}
		else if (var.get_type().get_name() == "pair2")
		{
			//Get the first and second element of the pair
			rttr::variant first = var.get_value<std::pair<unsigned char, std::string>>().first;
			rttr::variant second = var.get_value<std::pair<unsigned char, std::string>>().second;
			//Write the first element
			m_Writer->StartObject();
			m_Writer->Key("Key");
			SerializeRecursion(first);
			//Write the second element
			m_Writer->Key("Value");
			SerializeRecursion(second);
			m_Writer->EndObject();
			return true;
		}
		return false;
	}



}