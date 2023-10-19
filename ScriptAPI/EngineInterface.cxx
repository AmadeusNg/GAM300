#include "EngineInterface.hxx"
#include "Debug.hxx"
using namespace System;
using namespace System::Runtime::InteropServices;
#pragma comment (lib, "GAM300Engine.lib")


namespace ScriptAPI
{
    /*!*************************************************************************
    * Loads the managed script dll and adds the scriptlist to all active entities
    ***************************************************************************/
	void EngineInterface::Init()
	{
        using namespace System::IO;
        loadContext = gcnew System::Runtime::Loader::AssemblyLoadContext(nullptr, true);

        // Load assembly
        FileStream^ managedLibFile = File::Open
        (
            "ManagedScripts.dll",
            FileMode::Open, FileAccess::Read, FileShare::Read
        );
        loadContext->LoadFromStream(managedLibFile);
        managedLibFile->Close();

        // Load Assembly
        /*System::Reflection::Assembly::LoadFrom("ManagedScripts.dll");*/

		scripts = gcnew System::Collections::Generic::SortedList<TDS::EntityID,ScriptList^>();
        
        for (auto i : TDS::ecs.getEntities())
        {
            scripts->Add(i, gcnew ScriptList());
        }

        updateScriptTypeList();
		System::Console::WriteLine("Hello Engine Interface Init!");
	}

    /*!*************************************************************************
    * Adds scripts from the managedscript.dll to the entity via name
    ***************************************************************************/
    bool EngineInterface::AddScriptViaName(TDS::EntityID entityId, System::String^ scriptName)
    {
        SAFE_NATIVE_CALL_BEGIN
        if (entityId == TDS::NULLENTITY)
            return false;

        // Remove any whitespaces
        scriptName = scriptName->Trim();

        // Look for the correct script
        System::Type^ scriptType = nullptr;
        for each (System::Type ^ type in scriptTypeList)
        {
            if (type->FullName == scriptName || type->Name == scriptName)
            {
                scriptType = type;
                break;
            }
        }

        // Failed to get any script
        if (scriptType == nullptr)
        {
            return false;
        }

        // Create the script
        // Default construct an object of the specified type. 
        // We have to use this as we can�t use the normal gcnew syntax to create it as 
        // scriptType is a variable that stores a type; it itself is not a type that we can pass to gcnew.
        Script^ script = safe_cast<Script^>(System::Activator::CreateInstance(scriptType));
        script->SetEntityID(entityId);

        // Add script to SortedList
        scripts[entityId]->Add(script);
        return true;
        SAFE_NATIVE_CALL_END

        return false;
    }

    /*!*************************************************************************
    * Calls all script Awake function
    ***************************************************************************/
    void EngineInterface::ExecuteAwake()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            for each (Script ^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    script->Awake();
                SAFE_NATIVE_CALL_END
            }
        }
    }
    /*!*************************************************************************
    * Calls all script OnEnable function
    ***************************************************************************/
    void EngineInterface::ExecuteOnEnable()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            for each (Script ^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    script->OnEnable();
                SAFE_NATIVE_CALL_END
            }
        }
    }

    bool EngineInterface::ToggleScriptViaName(TDS::EntityID entityId, System::String^ scriptName)
    {
        SAFE_NATIVE_CALL_BEGIN
            if (entityId == TDS::NULLENTITY)
                return false;

        // Remove any whitespaces
        scriptName = scriptName->Trim();

        // Look for the correct script
        System::Type^ scriptType = nullptr;
        for each (System::Type ^ type in scriptTypeList)
        {
            if (type->FullName == scriptName || type->Name == scriptName)
            {
                scriptType = type;
                break;
            }
        }

        // Failed to get any script
        if (scriptType == nullptr)
        {
            return false;
        }

        for each (Script ^ script in scripts[entityId])
        {
            if (script->GetType() == scriptType)
            {
                script->ToggleScript();
            }
        }
        return true;
        SAFE_NATIVE_CALL_END

        return false;
    }

    /*!*************************************************************************
    * Calls all script start function
    ***************************************************************************/
    void EngineInterface::ExecuteStart()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            for each (Script ^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    script->Start();
                SAFE_NATIVE_CALL_END
            }
        }
    }

    /*!*************************************************************************
    * Calls the update function for scripts
    ***************************************************************************/
    void EngineInterface::ExecuteUpdate()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            for each (Script^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    if (script->isScriptEnabled())
                    {
                        script->Update();
                    }
                SAFE_NATIVE_CALL_END
            }
        }
    }

    /*!*************************************************************************
    * Calls the late update function for scripts
    ***************************************************************************/
    void EngineInterface::ExecuteLateUpdate()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            for each (Script ^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    script->LateUpdate();
                SAFE_NATIVE_CALL_END
            }
        }
    }

    /*!*************************************************************************
    * Calls the OnDisable function for scripts
    ***************************************************************************/
    void EngineInterface::ExecuteOnDisable()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            for each (Script ^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    script->OnDisable();
                SAFE_NATIVE_CALL_END
            }
        }
    }

    /*!*************************************************************************
    * Calls all script OnDestroy function
    ***************************************************************************/
    void EngineInterface::ExecuteOnDestroy()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            for each (Script ^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    script->OnDestroy();
                SAFE_NATIVE_CALL_END
            }
        }
    }

    void EngineInterface::Reload()
    {
        // Clear all references to types in the script assembly we are going to unload
        scripts->Clear();
        scriptTypeList = nullptr;
        // Unload
        loadContext->Unload();
        loadContext = nullptr;
        // Wait for unloading to finish
        System::GC::Collect();
        System::GC::WaitForPendingFinalizers();
        // Load the assembly again
        Init();
    }

	namespace
    {
        /* Select Many */
        ref struct Pair
        {
            System::Reflection::Assembly^ assembly;
            System::Type^                 type;
        };
        System::Collections::Generic::IEnumerable<System::Type^>^ selectorFunc(System::Reflection::Assembly^ assembly)
        {
            return assembly->GetExportedTypes();
        }
        Pair^ resultSelectorFunc(System::Reflection::Assembly^ assembly, System::Type^ type)
        {
            Pair^ p = gcnew Pair();
            p->assembly = assembly;
            p->type = type;
            return p;
        }
        /* Where */
        bool predicateFunc(Pair^ pair)
        {
            return pair->type->IsSubclassOf(Script::typeid) && !pair->type->IsAbstract;
        }
        /* Select */
        System::Type^ selectorFunc(Pair^ pair)
        {
            return pair->type;
        }
    }

    /*!*************************************************************************
    * Updates script type list
    ***************************************************************************/
    void EngineInterface::updateScriptTypeList()
    {
        using namespace System;
        using namespace System::Reflection;
        using namespace System::Linq;
        using namespace System::Collections::Generic;
        /* Select Many: Types in Loaded Assemblies */
        IEnumerable<Assembly^>^ assemblies = AppDomain::CurrentDomain->GetAssemblies();
        Func<Assembly^, IEnumerable<Type^>^>^ collectionSelector = gcnew Func<Assembly^, IEnumerable<Type^>^>(selectorFunc);
        Func<Assembly^, Type^, Pair^>^ resultSelector = gcnew Func<Assembly^, Type^, Pair^>(resultSelectorFunc);
        IEnumerable<Pair^>^ selectManyResult = Enumerable::SelectMany(assemblies, collectionSelector, resultSelector);
        /* Where: Are concrete Scripts */
        Func<Pair^, bool>^ predicate = gcnew Func<Pair^, bool>(predicateFunc);
        IEnumerable<Pair^>^ whereResult = Enumerable::Where(selectManyResult, predicate);
        /* Select: Select them all */
        Func<Pair^, Type^>^ selector = gcnew Func<Pair^, Type^>(selectorFunc);
        scriptTypeList = Enumerable::Select(whereResult, selector);
    }
}