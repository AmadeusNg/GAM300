/*!*************************************************************************
****
\file ecs.hpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines functions of the ECS
****************************************************************************
***/

namespace TDS
{
    // TYPE ID GENERATOR ==================================================================================
    // --getNewID--
    // Return: unique component ID
    // (for each different component, the template is different, so the generates another overloaded 
    // function with a different type)
    template<typename T>
    template<typename U>
    const IDType TypeIdGenerator<T>::getNewID()
    {
        static const IDType idCounter = mCount++;
        return idCounter;
    }

    // ENTITY =============================================================================================
    // --Entity Constructor--
    // Making a new entity
    // theecs - reference to the ECS class
    inline Entity::Entity() : mID(ecs.getNewID())
    {
        ecs.registerEntity(mID);
    }

    // --add--
    // Adding a new component, with component values
    // Return - pointer to the entity's new component data
    template<typename C>
    inline C* Entity::add()
    {
        return ecs.addComponent<C>(mID);
    }

    // --add--
    // Adding a new component overload, move component data
    // c - component data to move
    // Return - pointer to the entity's new component data
    template<typename C>
    inline C* Entity::add(C&& c)
    {
        return ecs.addComponent<C>(mID, std::forward<C>(c));
    }

    // --getID--
    // Get ID of the entity
    // Return - entityID (from 1 onwards)
    inline EntityID Entity::getID() const
    {
        return mID;
    }

    // COMPONENT ==========================================================================================
    // --setName--
    // Set name of component
    // name - new name of component
    template<class C>
    void Component<C>::setName(std::string name)
    {
        componentName = name;
    }

    // --getName--
    // Get name of component
    // Return - name of component
    template<class C>
    std::string Component<C>::getName() const
    {
        return componentName;
    }

    // --constructData--
    // 7Allocating new memory for the data
    // data - pointer to the new memory for the data
    template<class C>
    void Component<C>::constructData(unsigned char* data) const
    {
        new (data) C();
    }

    // --moveData--
    // Move the data
    // source - source of the data
    // destination - destination of the data to move to
    template<class C>
    void Component<C>::moveData(unsigned char* source, unsigned char* destination) const
    {
        //new (&destination[0]) C(std::move(*reinterpret_cast<C*>(source)));
        new (destination) C(std::move(*reinterpret_cast<C*>(source)));
    }

    // --getSize--
    // Returns the size of the whole component
    // Return - size of the component data
    template<class C>
    std::uint32_t Component<C>::getSize() const
    {
        return sizeof(C);
    }

    // --getTypeID--
    // Returns the unique ID of the component
    // Return - unique ID of the component
    template<class C>
    ComponentTypeID Component<C>::getTypeID()
    {
        return TypeIdGenerator<ComponentBase>::getNewID<C>();
    }

    // SYSTEM =============================================================================================
    // --action--
    // Setting the update function in the mFunc function pointer 
    // func - update function as function pointer 
    template<class... Cs>
    void System<Cs...>::action(InitFunc initFunc, RunFunc runFunc)
    {
        mInitFunc = initFunc;
        mRunFunc = runFunc;
        mFuncSet = true;
    }

    // --System Constructor--
    // Making a new system
    // ecs - THE ecs
    // layer - layer to put the system under
    template<class... Cs>
    System<Cs...>::System(const int layer) : mFuncSet(false)
    {
        ecs.registerSystem(layer, this);
        key = "";
    }

    // --getKey--
    // Getting the archetype ID based on the components given
    // Return - archetype ID
    template<class... Cs>
    ArchetypeID System<Cs...>::getKey()
    {
        if (key == "")
        {
            auto components = {Component<Cs>::getTypeID()...};

            for (std::uint32_t i = 0; i < ecs.getNumberOfComponents(); ++i)
            {
                key += "0";
            }

            for (auto component : components)
            {
                key[component] = '1';
            }
        }

        return key;
    }

    // --doAction--
    // Execute the function pointer that is set previously
    // elapsedMilliseconds - dt
    // archetype - archetype to be selected out for the function pointer to run
    template<class... Cs>
    void System<Cs...>::doAction(const float elapsedMilliseconds, Archetype* archetype)
    {
        if (mFuncSet)
            doAction<0>(elapsedMilliseconds,
                archetype->type,
                archetype->entityIds,
                archetype->componentData);
    }

    // --doAction--
    // Filtering out the entities that satisfies the given archetype
    // elapsedMilliseconds - dt
    // archeTypeIds - archetypeID to be selected out for the function pointer to run
    // entityIDs - list of entities selected
    // t & ts - components to be checked
    template<class... Cs>
    template<std::uint32_t Index, typename T, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs)>
        System<Cs...>::doAction(const float elapsedMilliseconds,
            const ArchetypeID& archeTypeIds,
            const std::vector<EntityID>& entityIDs,
            T& t,
            Ts... ts)
    {
        typedef std::tuple_element<Index, std::tuple<Cs...>>::type IthT;

        ComponentTypeID thisTypeCS = Component<IthT>::getTypeID();

        if (!archeTypeIds[thisTypeCS])
        {
            throw std::runtime_error
            ("System was executed against an incorrect Archetype");
        }

        doAction<Index + 1>(elapsedMilliseconds,
            archeTypeIds,
            entityIDs,
            t,
            ts...,
            reinterpret_cast<IthT*>(&t[thisTypeCS][0]));
    }

    // --doAction--
    // Default function
    // elapsedMilliseconds - dt
    // archeTypeIds - archetypeID to be selected out for the function pointer to run
    // entityIDs - list of entities selected
    // t & ts - components to be checked
    template<class... Cs>
    template<std::uint32_t Index, typename T, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs)>
        System<Cs...>::doAction(const float elapsedMilliseconds,
            const ArchetypeID& archeTypeIds,
            const std::vector<EntityID>& entityIDs,
            T& t,
            Ts... ts)
    {
        archeTypeIds;
        t;
        mRunFunc(elapsedMilliseconds, entityIDs, ts...);
    }

    template<class... Cs>
    void System<Cs...>::initialiseAction()
    {
        if (mFuncSet)
            mInitFunc();
    }

    // ECS ==========================================================================================
    // --ECS Constructor--
    inline ECS::ECS()
    { }

    /*!*************************************************************************
    Returns an instance of the ECS
    ****************************************************************************/
    //inline std::unique_ptr<ECS>& ECS::GetInstance()
    //{
    //    if (m_instance == nullptr)
    //    {
    //        m_instance = std::make_unique<ECS>();
    //    }
    //    return m_instance;
    //}

    // --getNewID--
    // Get new entity ID 
    // Return - new entity ID
    inline EntityID ECS::getNewID()
    {
        return mEntityIdCounter++;
    }

    // --getSystemCount--
    // Get the total number of systems
    // Return - total number of systems
    inline int ECS::getSystemCount()
    {
        return systemCount;
    }

    // --registerComponent--
    // Register a new component
    // name - name of component
    template<class C>
    void ECS::registerComponent(std::string name)
    {
        ComponentTypeID componentTypeId = Component<C>::getTypeID();

        if (mComponentMap.contains(componentTypeId))
        {
            return; // can't re-register a type
        }

        mComponentMap.emplace(componentTypeId, new Component<C>);
        mComponentMap[componentTypeId]->setName(name);

        ++componentCount;
        MemoryManager::GetInstance()->setNumberOfComponent(componentCount);
    }

    // --registerSystem--
    // Register a new system
    // layer - layer group to put system
    // system - declared system base with a system update defined in mFunc function pointer
    inline void ECS::registerSystem(const int layer, SystemBase* system)
    {
        ++systemCount;
        mSystems[layer].emplace_back(system);
    }

    // --registerEntity--
    // Register a new entity
    // entityId - entityID of the new entity
    inline void ECS::registerEntity(const EntityID entityId)
    {
        Record dummyRecord;
        dummyRecord.archetype = nullptr;
        dummyRecord.index = 0;
        dummyRecord.is_Enabled = true;
        mEntityArchetypeMap[entityId] = dummyRecord;
    }

    // --addComponent--
    // Add a component to the entity, and (optionally) put in the values for each variable in the component
    // entityId - entityID of the entity
    // Return - pointer to the entity's component data

    // args - values of the variables in the component (IN ORDER)
    //template<typename C, typename... Args>
    //inline C* ECS::addComponent(const EntityID& entityID, Args&&... args)

    template<typename C>
    inline C* ECS::addComponent(const EntityID& entityID)
    {
        // Getting the component ID
        ComponentTypeID componentID = Component<C>::getTypeID();

        // Getting the component size
        const std::uint32_t& componentSize = mComponentMap[componentID]->getSize();

        // Getting the record of the entity
        // (even if there is no such records, a new record will be made)
        Record& record = mEntityArchetypeMap[entityID];
        // Old archetype of the entity
        Archetype* oldArchetype = record.archetype;

        Archetype* newArchetype = nullptr;

        ArchetypeID newArchetypeId(componentCount, '0');
        newArchetypeId[componentID] = '1';

        // Checking if an old archetype exist
        if (oldArchetype)
        {
            // If the old archetype has the component to be added 
            // Return false since there is already such component
            if (oldArchetype->type[componentID] == '1')
            {
                return nullptr;
            }

            // Copy the ArchetypeID to add in the new component
            newArchetypeId = oldArchetype->type;
            newArchetypeId[componentID] = '1';

            // Getting the archetype pointer to new archetype
            newArchetype = getArchetype(newArchetypeId);
            ArchetypeID oldArchetypeId = oldArchetype->type;

            EntityID lastEntity = 0;
            std::uint32_t lastEntityIndex = 0;
            if (oldArchetype->entityIds.size() > 1)
            {
                lastEntity = oldArchetype->entityIds.back();
                Record& lastEntityRecord = mEntityArchetypeMap[lastEntity];
                lastEntityIndex = lastEntityRecord.index;
            }

            for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(oldArchetype->type.size()); ++i)
            {
                if (oldArchetypeId[i] == '0')
                {
                    continue;
                }
                std::uint32_t newIndex = static_cast<std::uint32_t>(newArchetype->entityIds.size());

                const ComponentBase* const currentComponent = mComponentMap[i];

                auto currentComponentSize = static_cast<std::uint32_t>(currentComponent->getSize());

                // 1. Add component in new archetype & transfer data over
                unsigned char* oldComponentPointer = MemoryManager::GetInstance()->getComponentData(oldArchetypeId, i, currentComponentSize, record.index);
                unsigned char* newComponentPointer = MemoryManager::GetInstance()->addComponentData(newArchetypeId, i, currentComponentSize, newIndex);

                oldArchetype->componentDataSize[i] -= currentComponentSize;
                newArchetype->componentDataSize[i] += currentComponentSize;

                currentComponent->moveData(oldComponentPointer, newComponentPointer);

                // 2. Shift last component in old archetype to replace
                if (lastEntity)
                {
                    unsigned char* lastEntityOldComponentPointer = MemoryManager::GetInstance()->getComponentData(oldArchetypeId, i, currentComponentSize, lastEntityIndex);
                    unsigned char* lastEntityNewComponentPointer = MemoryManager::GetInstance()->getComponentData(oldArchetypeId, i, currentComponentSize, record.index);

                    currentComponent->moveData(lastEntityOldComponentPointer, lastEntityNewComponentPointer);

                    MemoryManager::GetInstance()->removeComponentEndData(oldArchetypeId, i, currentComponentSize);
                }

                //MemoryManager::GetInstance()->moveComponentData<C>(oldArchetypeId, newArchetypeId, i, componentSize, record.index);
            }

            if (lastEntity)
            {
                mEntityArchetypeMap[lastEntity].index = record.index;
            }

            // Change the index of the entity shifted in the old archetype to fit into the moved space
            // (basically the last entityID in the old archetype)
            // So, more the entityID in the old archetype as well
            std::vector<EntityID>::iterator willBeRemoved = std::find(oldArchetype->entityIds.begin(), oldArchetype->entityIds.end(), entityID);

            if (willBeRemoved != oldArchetype->entityIds.end() - 1)
            {
                oldArchetype->entityIds.emplace(willBeRemoved + 1, oldArchetype->entityIds.back());
                oldArchetype->entityIds.erase(oldArchetype->entityIds.end() - 1);

                willBeRemoved = std::find(oldArchetype->entityIds.begin(), oldArchetype->entityIds.end(), entityID);
            }

            // Remove entity ID from old archetype
            oldArchetype->entityIds.erase(willBeRemoved);
        }
        else
        {
            newArchetype = getArchetype(newArchetypeId);
        }

        // Add entity ID to new archetype
        newArchetype->entityIds.emplace_back(entityID);
        // Change index
        record.index = static_cast<uint32_t>(newArchetype->entityIds.size() - 1);
        // Change archetype pointer
        record.archetype = newArchetype;

        record.archetype->componentDataSize[componentID] += componentSize;
        
        return new(MemoryManager::GetInstance()->addComponentData(newArchetypeId, componentID, componentSize, record.index)) C();
    }

    inline void ECS::addComponentsByArchetype(const EntityID& entityID, ArchetypeID archetype)
    {
        // Getting the record of the entity
        // (even if there is no such records, a new record will be made)
        Record& record = mEntityArchetypeMap[entityID];

        if (record.archetype)
        {
            return;     // only accept new entities
        }

        record.archetype = getArchetype(archetype);
        record.archetype->entityIds.emplace_back(entityID);
        record.index = static_cast<std::uint32_t>(record.archetype->entityIds.size() - 1);

        for (std::uint32_t componentID = 0; componentID < archetype.size(); ++componentID)
        {
            if (archetype[componentID] == '0')
            {
                continue;
            }

            const ComponentBase* const currentComponent = mComponentMap[componentID];

            unsigned char* newComponentPointer = MemoryManager::GetInstance()->addComponentData(archetype, componentID, currentComponent->getSize(), record.index);

            currentComponent->constructData(newComponentPointer);

            record.archetype->componentDataSize[componentID] += currentComponent->getSize();
        }
    }

    // --setComponentSize--
    // Setting component size
    // archetypeID - archetypeID to find component
    // componentID - componentID to set memory
    // componentSize - total size of memory for component
    // Return - pointer to the entity's component data
    inline void ECS::setComponentSize(const ArchetypeID& archetypeID, ComponentTypeID componentID, std::uint32_t componentSize)
    {
        MemoryManager::GetInstance()->reserveComponentSpace(archetypeID, componentID, componentSize);
    }

    // --removeComponent--
    // Remove a component from the entity
    // entityId - entityID of the entity
    template<class C>
    inline void ECS::removeComponent(const EntityID& entityID)
    {
        ComponentTypeID compTypeId = Component<C>::getTypeID();

        if (!mEntityArchetypeMap.contains(entityID))
            return; // entity doesn't exist

        Record& record = mEntityArchetypeMap[entityID];

        Archetype* oldArchetype = record.archetype;
        ArchetypeID oldArchetypeId = oldArchetype->type;

        if (!oldArchetype)
            return; // there's no components anyway

        if (oldArchetypeId[compTypeId] == '0')
        {
            // this entity doesn't have this component
            return;
        }

        // find the new archetypeId by removing the old ComponentTypeId
        ArchetypeID newArchetypeId = oldArchetype->type;
        newArchetypeId[compTypeId] = '0';

        Archetype* newArchetype = getArchetype(newArchetypeId);

        EntityID lastEntity = 0;
        std::uint32_t lastEntityIndex = 0;
        if (oldArchetype->entityIds.size() > 1)
        {
            lastEntity = oldArchetype->entityIds.back();
            Record& lastEntityRecord = mEntityArchetypeMap[lastEntity];
            lastEntityIndex = lastEntityRecord.index;
        }

        // For each component in new archetype
        for (std::uint32_t j = 0; j < static_cast<std::uint32_t>(oldArchetypeId.size()); ++j)
        {
            // Not a component
            if (oldArchetypeId[j] == '0')
            {
                continue;
            }

            // New component base pointer 
            const ComponentBase* const currentComponent = mComponentMap[j];
            // Size of new component base pointer
            const std::uint32_t currentComponentSize = currentComponent->getSize();

            std::uint32_t newIndex = static_cast<std::uint32_t>(newArchetype->entityIds.size());

            // If this is not the component to delete, move component data from the old archetype to the new archetype
            if (j != Component<C>::getTypeID())
            {
                unsigned char* oldComponentPointer = MemoryManager::GetInstance()->getComponentData(oldArchetypeId, j, currentComponentSize, record.index);
                unsigned char* newComponentPointer = MemoryManager::GetInstance()->addComponentData(newArchetypeId, j, currentComponentSize, newIndex);

                currentComponent->moveData(oldComponentPointer, newComponentPointer);
            }

            oldArchetype->componentDataSize[j] -= currentComponentSize;
            newArchetype->componentDataSize[j] += currentComponentSize;

            if (lastEntity)
            {
                // Then move the last entity of the old archetype to replace (if there is another entity in the archetype)
                unsigned char* lastEntityOldComponentPointer = MemoryManager::GetInstance()->getComponentData(oldArchetypeId, j, currentComponentSize, lastEntityIndex);
                unsigned char* lastEntityNewComponentPointer = MemoryManager::GetInstance()->getComponentData(oldArchetypeId, j, currentComponentSize, record.index);

                currentComponent->moveData(lastEntityOldComponentPointer, lastEntityNewComponentPointer);

                MemoryManager::GetInstance()->removeComponentEndData(oldArchetypeId, j, currentComponentSize);
            }
        }

        if (lastEntity)
        {
            mEntityArchetypeMap[lastEntity].index = record.index;
        }

        std::vector<EntityID>::iterator willBeRemoved = std::find(oldArchetype->entityIds.begin(), oldArchetype->entityIds.end(), entityID);

        if (willBeRemoved != oldArchetype->entityIds.end() - 1)
        {
            oldArchetype->entityIds.emplace(willBeRemoved + 1, oldArchetype->entityIds.back());
            oldArchetype->entityIds.erase(oldArchetype->entityIds.end() - 1);

            willBeRemoved = std::find(oldArchetype->entityIds.begin(), oldArchetype->entityIds.end(), entityID);
        }

        // Remove entity ID from old archetype
        oldArchetype->entityIds.erase(willBeRemoved);

        newArchetype->entityIds.emplace_back(entityID);
        record.index = static_cast<uint32_t>(newArchetype->entityIds.size() - 1);
        record.archetype = newArchetype;
    }

    // --getComponent--
    // Get a component data from the entity
    // entityId - entityID of the entity
    // Return - pointer to the component data
    template<class C>
    inline C* ECS::getComponent(const EntityID& entityId)
    {
        ComponentTypeID compTypeId = Component<C>::getTypeID();

        if (!mEntityArchetypeMap.contains(entityId))
            return nullptr; // it doesn't exist

        Record& record = mEntityArchetypeMap[entityId];

        mArchetypes;

        if (!record.archetype)
            return nullptr; // there's no components anyway

        if (record.archetype->type[compTypeId] == '0')
            return nullptr; // this entity doesn't have this component

        //return reinterpret_cast<C*>(record.archetype->componentData[compTypeId] + record.index * sizeof(C));
        return reinterpret_cast<C*>(MemoryManager::GetInstance()->getComponentData(record.archetype->type, compTypeId, sizeof(C), record.index));
    }

    // --removeEntity--
    // Remove the entity, by removing all its component and data
    // entityId - entityID of the entity
    inline void ECS::removeEntity(const EntityID& entityID)
    {
        //if (!mEntityArchetypeMap.contains(entityID))
        //    return; // it doesn't exist

        Record& record = mEntityArchetypeMap[entityID];

        Archetype* oldArchetype = record.archetype;

        if (!oldArchetype)
        {
            mEntityArchetypeMap.erase(entityID);
            return; // didnt have components
        }

        ArchetypeID oldArchetypeID = oldArchetype->type;

        EntityID lastEntity = 0;
        std::uint32_t lastEntityIndex = 0;
        if (oldArchetype->entityIds.size() > 1)
        {
            lastEntity = oldArchetype->entityIds.back();
            Record& lastEntityRecord = mEntityArchetypeMap[lastEntity];
            lastEntityIndex = lastEntityRecord.index;
        }

        for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(oldArchetypeID.size()); ++i)
        {
            if (oldArchetypeID[i] == '0')
            {
                continue;
            }

            const ComponentBase* const currentComponent = mComponentMap[(unsigned int)i];

            const std::uint32_t& currentComponentSize = currentComponent->getSize();

            if (lastEntity)
            {
                // Then move the last entity of the old archetype to replace (if there is another entity in the archetype)
                unsigned char* lastEntityOldComponentPointer = MemoryManager::GetInstance()->getComponentData(oldArchetypeID, i, currentComponentSize, lastEntityIndex);
                unsigned char* lastEntityNewComponentPointer = MemoryManager::GetInstance()->getComponentData(oldArchetypeID, i, currentComponentSize, record.index);

                currentComponent->moveData(lastEntityOldComponentPointer, lastEntityNewComponentPointer);
            }
        }

        if (lastEntity)
        {
            mEntityArchetypeMap[lastEntity].index = record.index;
        }

        std::vector<EntityID>::iterator willBeRemoved = std::find(oldArchetype->entityIds.begin(), oldArchetype->entityIds.end(), entityID);

        if (willBeRemoved != oldArchetype->entityIds.end() - 1)
        {
            oldArchetype->entityIds.emplace(willBeRemoved + 1, oldArchetype->entityIds.back());
            oldArchetype->entityIds.erase(oldArchetype->entityIds.end() - 1);

            willBeRemoved = std::find(oldArchetype->entityIds.begin(), oldArchetype->entityIds.end(), entityID);
        }

        oldArchetype->entityIds.erase(willBeRemoved);

        mEntityArchetypeMap.erase(entityID);
    }

    // --removeAllEntities--
    // Remove the all of entities
    inline void ECS::removeAllEntities()
    {
        std::vector<EntityID> entities = getEntities();

        for (auto& i : entities)
        {
            removeEntity(i);
        }

        mEntityIdCounter = 1;
    }

    // --setIDCounter--
    // Set the entity ID counter
    // counter - number to set the entity id counter
    inline void ECS::setIDCounter(int counter)
    {
        mEntityIdCounter = counter;
    }

    // --initializeSystems--
    // Initializes all the systems of a certain layer
    // layer - layer with all the systems to run
    inline void ECS::initializeSystems(const int layer)
    {
        for (SystemBase* system : mSystems[layer])
        {
            system->initialiseAction();
        }
    }

    // --runSystems--
    // Run all the systems of a certain layer
    // layer - layer with all the systems to run
    // elapsedMilliseconds - dt
    inline void ECS::runSystems(const int layer, const float elapsedMilliseconds)
    {
        for (SystemBase* system : mSystems[layer])
        {
            const ArchetypeID& key = system->getKey();

            for (Archetype* archetype : mArchetypes)
            {
                bool check = true;
                for (int i = 0; i < key.length(); ++i)
                {
                    if (key[i] == '0')
                    {
                        continue;
                    }

                    if (archetype->type[i] == '0')
                    {
                        check = false;
                        break;
                    }
                }

                if (check)
                {
                    // this archetype has all the types required by the system
                    // so we can pull it's relevant data, reinterpret them as
                    // their correct types, and call the Func in the system
                    system->doAction(elapsedMilliseconds, archetype);
                }
            }
        }
    }

    // --getArchetype--
    // Gets the archetype class pointer of the given archetype ID
    // id - archetype ID
    // Return - pointer to found archetype class / if not found, pointer to new archetype class
    inline Archetype* ECS::getArchetype(const ArchetypeID& id)
    {
        // Finding for archetype
        for (Archetype* archetype : mArchetypes)
        {
            if (archetype->type == id)
                return archetype;
        }

        return addArchetype(id);
    }

    // --getAllArchetypes--
    // Gets the total number of archetypes
    // Return - vector of archetypes
    inline std::vector<Archetype*> ECS::getAllArchetypes()
    {
        return mArchetypes;
    }

    // --getArchetypeID--
    // Gets archetype ID
    // id - entityID
    // Return - archetype ID
    inline ArchetypeID ECS::getArchetypeID(EntityID& id)
    {
        return mEntityArchetypeMap[id].archetype->type;
    }

    // --addArchetype--
    // Adds a new archetype ID
    // id - archetypeID
    inline Archetype* ECS::addArchetype(const ArchetypeID& id, bool commit)
    {
        // Archetype doesn't exist, so create a new one
        Archetype* newArchetype = new Archetype;
        newArchetype->type = id;
        mArchetypes.emplace_back(newArchetype);

        MemoryManager::GetInstance()->newBook(id);

        // Add an empty array for each component in the type
        for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(id.size()); ++i)
        {
            if (id[i] == '1')
            {
                newArchetype->componentData.emplace_back(MemoryManager::GetInstance()->newPage(id, i));

                if (commit)
                {
                    MemoryManager::GetInstance()->reserveComponentSpace(id, i, mComponentMap[i]->getSize());
                }
            }
            else
            {
                newArchetype->componentData.emplace_back(nullptr);
            }
            newArchetype->componentDataSize.emplace_back(0);
        }
        if (commit)
        {
            commitArchetype(id);
        }

        return newArchetype;
    }

    // --commitArchetype--
    // Adds a new archetype ID
    // id - archetypeID
    inline void ECS::commitArchetype(const ArchetypeID& id)
    {
        MemoryManager::GetInstance()->commitBook(id);
    }

    // --~ECS--
    // Decontructing ECS
    inline void ECS::destroy()
    {
        for (Archetype* archetype : mArchetypes)
        {
            MemoryManager::GetInstance()->freeBook(archetype->type);

            delete archetype;
        }
        for (ComponentTypeIDBaseMap::value_type& p : mComponentMap)
            delete p.second;
    }

    // --getAllRegisteredComponents--
    // Getting all registered components
    // Return - list of names of all the components
    inline std::vector<std::string> ECS::getAllRegisteredComponents()
    {
        std::vector<std::string> componentNames;
        for (auto i : mComponentMap)
        {
            componentNames.emplace_back((i.second)->getName());
        }
        return componentNames;
    }

    // --getNumberOfComponents--
    // Getting number of components
    // Return - total number of components
    inline std::uint32_t ECS::getNumberOfComponents()
    {
        return componentCount;
    }

    // --getEntities--
    // Getting all registered entities
    // Return - list of entityIDs that are valid (and not destroyed)
    inline std::vector<EntityID> ECS::getEntities()
    {
        std::vector<EntityID> entityIDs;
        for (auto& i : mEntityArchetypeMap)
        {
            entityIDs.emplace_back(i.first);
        }
        return entityIDs;
    }

    inline bool ECS::getEnabledEntity(const EntityID& entityId)
    {
        return mEntityArchetypeMap[entityId].is_Enabled;
    }

    inline void ECS::toggleEnabledEntity(const EntityID& entityId)
    {
        mEntityArchetypeMap[entityId].is_Enabled = !mEntityArchetypeMap[entityId].is_Enabled;
    }

    //inline std::unique_ptr<ECS>& getECS()
    //{
    //    return ECS::GetInstance();
    //}

    // --getEntityComponents--
    // Get components of a certain entity
    // entityId - entityID given
    // Return - list of names of the components the entity has
    inline std::vector<std::string> ECS::getEntityComponents(const EntityID& entityId)
    {
        std::vector<std::string> components;

        Record& record = mEntityArchetypeMap[entityId];

        if (!record.archetype)
            return components; // there's no components anyway    

        for (int i = 0; i < record.archetype->type.size(); ++i)
        {
            if (!(record.archetype->type[i]))
            {
                continue;
            }

            auto it = mComponentMap.find(i);
            components.emplace_back(it->second->getName());
        }
        return components;
    }

    // --getEntityComponentsBase--
    // Get components of a certain entity
    // entityId - entityID given
    // Return - list of names of the components the entity has
    inline std::vector<IComponent*> ECS::getEntityComponentsBase(const EntityID& entityId)
    {
        std::vector<IComponent*> components;

        //if (!mEntityArchetypeMap.contains(entityId))
        //    return components; // it doesn't exist

        Record& record = mEntityArchetypeMap[entityId];

        if (!record.archetype)
            return components; // there's no components anyway    

        for (int i = 0; i < record.archetype->type.size(); ++i)
        {
            if (record.archetype->type[i] == '0')
            {
                continue;
            }

            const ComponentBase* const currentComponent = mComponentMap[i];

            components.emplace_back(reinterpret_cast<IComponent*>(MemoryManager::GetInstance()->getComponentData(record.archetype->type, i, currentComponent->getSize(), record.index)));
        }

        return components;
    }

    // --getEntitiesWith--
    // Get entities with a certain component in the angle bracket
    // Return - list of entityIDs
    template<class C>
    inline std::vector<EntityID> ECS::getEntitiesWith()
    {
        std::vector<EntityID> entitiesList;

        for (auto i : getEntities())
        {
            C* theType = getComponent<C>(i);
            if (theType)
            {
                entitiesList.emplace_back(i);
            }
        }

        return entitiesList;
    }
}
