#include "Script.hxx"

namespace ScriptAPI
{
	void Script::SetEntityID(TDS::EntityID ID)
	{
		entityID = ID;
		is_Enabled = true;
	}

	void Script::ToggleScript()
	{
		is_Enabled = !is_Enabled;
		if (is_Enabled)
		{
			OnEnable();
		}
		else
		{
			OnDisable();
		}
	}

	bool Script::isScriptEnabled()
	{
		return is_Enabled;
	}

	TransformComponent Script::GetTransformComponent()
	{
		return TransformComponent(entityID);
	}
	ColliderComponent Script::GetColliderComponent()
	{
		return ColliderComponent(entityID);
	}
}