#include "Serialization.hxx"
#include "TypeConversion.hxx"
#include "EngineInterface.hxx"

namespace ScriptAPI
{
	TDS::ScriptValues Serialization::GetValue(System::Object^ object, System::Reflection::FieldInfo^ variable)
	{
		TDS::ScriptValues variableInfo;
		variableInfo.name = toStdString(variable->Name);

		System::String^ fieldType = variable->FieldType->ToString();
		variableInfo.type = toStdString(fieldType);

		if (fieldType->Contains("[]"))
		{

		}
		else if (fieldType->Contains("System"))
			variableInfo.value = variable->GetValue(object) ? toStdString(variable->GetValue(object)->ToString()) : "";
		else if (fieldType == "ScriptAPI.Vector2")
		{
			Vector2 value = safe_cast<Vector2>(variable->GetValue(object));
			variableInfo.vectorValueX = value.X;
			variableInfo.vectorValueY = value.Y;
		}
		else if (fieldType == "ScriptAPI.Vector3")
		{
			Vector3 value = safe_cast<Vector3>(variable->GetValue(object));
			variableInfo.vectorValueX = value.X;
			variableInfo.vectorValueY = value.Y;
			variableInfo.vectorValueZ = value.Z;
		}
		else if (fieldType == "ScriptAPI.Vector4")
		{
			Vector4 value = safe_cast<Vector4>(variable->GetValue(object));
			variableInfo.vectorValueX = value.X;
			variableInfo.vectorValueY = value.Y;
			variableInfo.vectorValueZ = value.Z;
			variableInfo.vectorValueW = value.W;
		}
		else if (fieldType == "ScriptAPI.Quaternion")
		{
			Quaternion value = safe_cast<Quaternion>(variable->GetValue(object));
			variableInfo.vectorValueX = value.X;
			variableInfo.vectorValueY = value.Y;
			variableInfo.vectorValueZ = value.Z;
			variableInfo.vectorValueW = value.W;
		}
		else if (fieldType->Contains("GameObject")) // Other entities
		{
			variableInfo.referenceEntityID = variable->GetValue(object) ? safe_cast<GameObject^>(variable->GetValue(object))->GetEntityID() : 0;
		}
		else if (fieldType->Contains("Component")) // Components
		{
			variableInfo.referenceEntityID = variable->GetValue(object) ? safe_cast<ComponentBase^>(variable->GetValue(object))->GetEntityID() : 0;
		}
		else
		{
			variableInfo.referenceEntityID = variable->GetValue(object) ? safe_cast<Script^>(variable->GetValue(object))->gameObject->GetEntityID() : 0;
			//System::Console::WriteLine("Assuming it is a script");
		}

		return variableInfo;
	}

	void Serialization::SetValue(System::Object^ object, System::Reflection::FieldInfo^ variable, TDS::ScriptValues variableInfo)
	{
		System::TypeCode code;

		System::String^ fieldType = variable->FieldType->ToString();
		System::String^ value = toSystemString(variableInfo.value);

		if (fieldType->Contains("[]"))
		{

		}
		else if (fieldType == "System.Boolean")
			variable->SetValue(object, Convert::ToBoolean(value));
		else if (fieldType == "System.Int16")
			variable->SetValue(object, Convert::ToInt16(value));
		else if (fieldType == "System.Int32")
			variable->SetValue(object, Convert::ToInt32(value));
		else if (fieldType == "System.Int64")
			variable->SetValue(object, Convert::ToInt64(value));
		else if (fieldType == "System.UInt16")
			variable->SetValue(object, Convert::ToUInt16(value));
		else if (fieldType == "System.UInt32")
			variable->SetValue(object, Convert::ToUInt32(value));
		else if (fieldType == "System.UInt64")
			variable->SetValue(object, Convert::ToUInt64(value));
		else if (fieldType == "System.Byte")
			variable->SetValue(object, Convert::ToByte(value));
		else if (fieldType == "System.SByte")
			variable->SetValue(object, Convert::ToSByte(value));
		else if (fieldType == "System.Double")
			variable->SetValue(object, Convert::ToDouble(value));
		else if (fieldType == "System.Single")
			variable->SetValue(object, Convert::ToSingle(value));
		else if (fieldType == "System.Char")
			variable->SetValue(object, Convert::ToChar(value));
		else if (fieldType == "System.String")
			variable->SetValue(object, value);
		
		else if (fieldType == "ScriptAPI.Vector2")
			variable->SetValue(object, Vector2(variableInfo.vectorValueX, variableInfo.vectorValueY));
		else if (fieldType == "ScriptAPI.Vector3")
			variable->SetValue(object, Vector3(variableInfo.vectorValueX, variableInfo.vectorValueY, variableInfo.vectorValueZ));
		else if (fieldType == "ScriptAPI.Vector4")
			variable->SetValue(object, Vector4(variableInfo.vectorValueX, variableInfo.vectorValueY, variableInfo.vectorValueZ, variableInfo.vectorValueW));
		else if (fieldType == "ScriptAPI.Quaternion")
			variable->SetValue(object, Quaternion(variableInfo.vectorValueX, variableInfo.vectorValueY, variableInfo.vectorValueZ, variableInfo.vectorValueW));
		
		else if (fieldType == "ScriptAPI.GameObject") // Other entities
		{
			variable->SetValue(object, EngineInterface::GetGameObject(variableInfo.referenceEntityID));
		}
		else if (fieldType->Contains("Component")) // Components
		{
			ComponentBase^ component = safe_cast<ComponentBase^>(variable->GetValue(object));
			component->SetEntityID(variableInfo.referenceEntityID);
			variable->SetValue(object, component);
		}
		else
		{
			// Assumuing that it is a script
			variable->SetValue(object, EngineInterface::GetScriptReference(variableInfo.referenceEntityID, fieldType));
			//System::Console::WriteLine("what even: " + fieldType);
		}
	}
}