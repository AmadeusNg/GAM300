/*!*************************************************************************
****
\file serialization.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines the functions in the Serializer class
****************************************************************************
***/

#include <fstream>
#include <sstream>

#include "sceneManager/serialization.h"

namespace TDS
{
	/*!*************************************************************************
	Funtion for Deserializing to JSON file
	****************************************************************************/
	bool Serializer::DeserializeFromFile(const std::string& filePath)
	{
		std::cout << std::filesystem::current_path().string() << std::endl;
		std::ifstream ifs(filePath, std::ios::in);
		std::stringstream buffer;
		buffer << ifs.rdbuf();
		ifs.close();

		return Deserialize(buffer.str());
	}
	
	/*!*************************************************************************
	Funtion for Serializing to JSON file
	****************************************************************************/
	bool Serializer::SerializeToFile(const std::string& filePath)
	{
		std::string str = Serialize();
		std::ofstream ofs(filePath);
		ofs << str;
		ofs.flush();
		ofs.close();

		return true;
	}

	/*!*************************************************************************
	Json Serializer for calling Derived class Serialize function
	****************************************************************************/
	std::string Serializer::Serialize() const
	{
		rapidjson::StringBuffer ss;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(ss);

		if (Serialize(&writer))
		{
			return ss.GetString();
		}

		return "";
	}

	/*!*************************************************************************
	Json Deserializer for calling Derived class Deserialize function
	****************************************************************************/
	bool Serializer::Deserialize(const std::string& stringbuffer)
	{
		rapidjson::Document doc;
		doc.Parse(stringbuffer.c_str());

		if (!InitDocument(stringbuffer, doc))
		{
			return false;
		}

		Deserialize(doc, doc);
		return true;
	}

	/*!*************************************************************************
	Checks if Document can be Read
	****************************************************************************/
	bool Serializer::InitDocument(const std::string& stringbuffer, rapidjson::Document& doc)
	{
		if (stringbuffer.empty())
			return false;

		std::string validJson(stringbuffer);

		return !doc.Parse(validJson.c_str()).HasParseError() ? true : false;
	}
}
