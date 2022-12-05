#pragma once

//#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/ECSB/ComponentPool.h"
#include "ISTE/ECSB/BehaviourHandle.h"
#include "SceneHandler.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Context.h"
//#include "ISTE/ECSB/Behaviour.h"
//#include "../Navigation/Legacy/NavMesh/Astar.hpp" // Legacy
#include "ISTE/Navigation/NavMeshDefines.h"
#include "ISTE/Navigation/NavMeshGrid.h"

#include "ISTE/CU/Database.h"

#include <assert.h>
#include <unordered_map>

namespace ISTE
{
	struct Entity
	{
		EntityID myId;

		std::string myName = "UnNamed_Entity";
		std::string myTag = "";
		//should probably be a int or sumthing instead
		std::string myLayer = "";

		bool myIsActive = true;
		bool myIsStatic = false;
		//probs a better way to do this
		bool myWasCulled = false;

		EntityID myParent = INVALID_ENTITY;
		//should probably consider using something better then a vector here
		std::vector<EntityID> myChildren;

		ComponentMask myMask;
		BehaviourHandle myBehaviourHandle;

		//temp, should also be ifdefed
		bool myIsEditorCreated = false;

		//this is also kinda ugly
		std::string myPrefabConnection = "";

	};

	//should consider cueing certen things like updates to systems, this could help for threading as well 

	class Scene
	{
	public:
		~Scene();
		EntityID NewEntity();
		void DestroyEntity(EntityID aId);
		void DeactivateEntity(EntityID aId, bool aCullArg = false);
		void ActivateEntity(EntityID aId, bool aCullArg = false);
		void Update(float aTimeDelta);

		void SetParent(EntityID aChildID, EntityID aParentID, bool aTakeToParentSpace = true);
		void UnParent(EntityID aChildID, EntityID aParentID);

		inline const std::string& GetName() const { return myName; }

		inline bool IsEntityActive(EntityID aId) 
		{ 
			if (!IsEntityIDValid(aId))
				return false;

			return myEntities[GetEntityIndex(aId)].myIsActive;
		}

		inline bool IsEntityIDValid(EntityID aId)
		{
			if (!IsEntityIndexValid(aId) || GetEntityIndex(aId) >= myEntities.size())
				return false;

			return myEntities[GetEntityIndex(aId)].myId == aId;
		}

		template<typename T>
		T* AssignComponent(EntityID aId)
		{ 
			if (!IsEntityIDValid(aId))
				return nullptr;

			ComponentID compId = mySceneHandler->GetId<T>();

			assert(myComponentPools.count(compId));

			EntityIndex index = GetEntityIndex(aId);

			assert(myComponentPools[compId]->MapIndex(index));

			T* comp = new (myComponentPools[compId]->Get(index)) T();

			myEntities[index].myMask.set(compId);

			if(myEntities[index].myIsActive)
				Context::Get()->mySystemManager->EntitySignatureChanged(aId, myEntities[index].myMask, (myEntities[index].myParent == INVALID_ENTITY));

			return comp;

		}
		template<typename T>
		T* AssignBehaviour(EntityID aId)
		{

			if (!IsEntityIDValid(aId))
				return nullptr;

			T* comp = AssignComponent<T>(aId);

			ComponentID compId = mySceneHandler->GetId<T>();

			EntityIndex index = GetEntityIndex(aId);
			((Behaviour*)comp)->myHostId = aId;
			((Behaviour*)comp)->myComponentId = compId;
			myEntities[index].myBehaviourHandle.AddBehaviour<T>(comp);

			return comp;
		}

		template<typename T>
		void RemoveComponent(EntityID aId)
		{

			if (!IsEntityIDValid(aId))
				return;

			ComponentID compId = mySceneHandler->GetId<T>();

			assert(myComponentPools.count(compId));

			RemoveComponentArg arg;
			arg.myId = aId;
			arg.myComponentId = compId;
			myComponentsRemoved.push_back(arg);

			//EntityIndex index = GetEntityIndex(aId);

			//void* cmp = myComponentPools[compId]->RemoveMapping(index);

			//if(mySceneHandler->myFCM.test(compId) && cmp != nullptr)
			//	mySceneHandler->mySimpleCalls.at(compId).myDesCall(cmp);

			//myEntities[index].myMask.set(compId, false);

			//if (myEntities[index].myIsActive)
			//	Context::Get()->mySystemManager->EntitySignatureChanged(aId, myEntities[index].myMask, (myEntities[index].myParent == INVALID_ENTITY));
		}

		template<typename T>
		void RemoveBehaviour(EntityID aId)
		{
			if (!IsEntityIDValid(aId))
				return;

			RemoveComponentArg arg;
			arg.myId = aId;
			arg.myComponentId = mySceneHandler->GetId<T>();
			myBehavioursRemoved.push_back(arg);

			//RemoveComponent<T>(aId);

			//EntityIndex index = GetEntityIndex(aId);
			////((Behaviour*)comp)->myHostId = INVALID_ENTITY;
			//myEntities[index].myBehaviourHandle.GetBehaviour<T>()->~T();
			//myEntities[index].myBehaviourHandle.RemoveBehaviour<T>();
		}

		template<typename T>
		T* GetComponent(EntityID aId)
		{
			if (!IsEntityIDValid(aId))
				return nullptr;

			ComponentID compId = mySceneHandler->GetId<T>();

			assert(myComponentPools.count(compId));

			EntityIndex index = GetEntityIndex(aId);
			
			return (T*)myComponentPools[compId]->Get(index);
		}

		inline void* GetComponent(ComponentID aComponentId, EntityID aId)
		{
			if (!IsEntityIDValid(aId))
				return nullptr;

			assert(myComponentPools.count(aComponentId));

			EntityIndex index = GetEntityIndex(aId);

			return myComponentPools[aComponentId]->Get(index);
		}

		inline void* AssignComponent(ComponentID aComponentId, EntityID aId)
		{
			if (!IsEntityIDValid(aId))
				return nullptr;

			assert(myComponentPools.count(aComponentId));

			EntityIndex index = GetEntityIndex(aId);

			assert(myComponentPools[aComponentId]->MapIndex(index));

			void* comp = myComponentPools[aComponentId]->Get(index);

			mySceneHandler->mySimpleCalls.at(aComponentId).myConCall(comp);

			myEntities[index].myMask.set(aComponentId);

			if (myEntities[index].myIsActive)
				Context::Get()->mySystemManager->EntitySignatureChanged(aId, myEntities[index].myMask, (myEntities[index].myParent == INVALID_ENTITY));

			return comp;
		}

		inline void* AssignBehaviour(ComponentID aComponentId, EntityID aId)
		{
			if (!IsEntityIDValid(aId))
				return nullptr;

			void* comp = AssignComponent(aComponentId, aId);

			//mySceneHandler->mySimpleCalls.at(aComponentId).myConCall(comp);

			EntityIndex index = GetEntityIndex(aId);
			((Behaviour*)comp)->myHostId = aId;
			((Behaviour*)comp)->myComponentId = aComponentId;
			myEntities[index].myBehaviourHandle.AddBehaviour(comp);

			return comp;
		}

		inline void RemoveComponent(ComponentID aComponentId, EntityID aId)
		{

			if (!IsEntityIDValid(aId))
				return;

			assert(myComponentPools.count(aComponentId));

			RemoveComponentArg arg;
			arg.myId = aId;
			arg.myComponentId = aComponentId;
			myComponentsRemoved.push_back(arg);

			//EntityIndex index = GetEntityIndex(aId);

			//void* cmp = myComponentPools[aComponentId]->RemoveMapping(index);

			//if (mySceneHandler->myFCM.test(aComponentId) && cmp != nullptr)
			//	mySceneHandler->mySimpleCalls.at(aComponentId).myDesCall(cmp);

			//myEntities[index].myMask.set(aComponentId, false);

			//if (myEntities[index].myIsActive)
			//	Context::Get()->mySystemManager->EntitySignatureChanged(aId, myEntities[index].myMask, (myEntities[index].myParent == INVALID_ENTITY));
		}

		inline void RemoveBehaviour(ComponentID aComponentId, EntityID aId)
		{
			if (!IsEntityIDValid(aId))
				return;

			RemoveComponentArg arg;
			arg.myId = aId;
			arg.myComponentId = aComponentId;
			myBehavioursRemoved.push_back(arg);
			//RemoveComponent(aComponentId,aId);

			//EntityIndex index = GetEntityIndex(aId);

			//std::vector<Behaviour*>& b = myEntities[index].myBehaviourHandle.GetEverything();

			//int count = 0;
			//for (auto& behaviour : b)
			//{
			//	if (behaviour->myComponentId == aComponentId)
			//	{
			//		behaviour->~Behaviour();
			//		b.erase(b.begin() + count);
			//		break;
			//	}
			//	count++;
			//}

		}

		inline BehaviourHandle& GetBehaviourHandle(EntityID aId)
		{
			return myEntities[GetEntityIndex(aId)].myBehaviourHandle;
		}

		template <typename T>
		ComponentPool& GetComponentPool()
		{
			ComponentID componentId = mySceneHandler->GetId<T>();

			return *myComponentPools[componentId];
		}

		ComponentPool& GetComponentPool(ComponentID aComponentId)
		{
			return *myComponentPools[aComponentId];
		}

		template<typename T>
		void CreateComponentPool(size_t aSize = MAX_ENTITIES)
		{
			ComponentID compId = mySceneHandler->GetId<T>();
			size_t compSize = sizeof(T);

			myComponentPools.insert({ compId, new ComponentPool(compSize, aSize, MAX_ENTITIES) });
		}

		void CreateComponentPool(ComponentID aId, size_t aSize = MAX_ENTITIES);

		inline void SetEntityName(EntityID aId, std::string aName)
		{
			if (!IsEntityIDValid(aId))
				return;

			myEntities[GetEntityIndex(aId)].myName = aName;
		}
		inline std::string GetEntityName(EntityID aId)
		{
			if (!IsEntityIDValid(aId))
				return "";

			return myEntities[GetEntityIndex(aId)].myName;
		}

		void SetPlayerId(EntityID aId)
		{
			myPlayerID = aId;
		}

		const EntityID GetPlayerId()
		{
			return myPlayerID;
		}

		void SetBossId(EntityID aId)
		{
			myBossID = aId;
		}

		const EntityID GetBossId()
		{
			return myBossID;
		}

		inline const Entity& GetEntity(EntityID aId) 
		{ 
			return myEntities[GetEntityIndex(aId)];
		}

		//Astar& GetAstar()
		//{
		//	return *myAstar;
		//}

		inline CU::Database<true>& GetDatabase() { return myDatabase; }
		inline NavMesh::NavMesh& GetNavMesh() { return myNavMesh; }
		inline NavMesh::Grid& GetNavMeshGrid() { return myNavMeshGrid; }

		inline void SetTag(EntityID aId, std::string aTag) { myEntities[GetEntityIndex(aId)].myTag = aTag; }
		inline std::string GetTag(EntityID aId) { if (!IsEntityIDValid(aId)) return "";  return myEntities[GetEntityIndex(aId)].myTag; }

		inline void SetLayer(EntityID aId, std::string aLayer) { myEntities[GetEntityIndex(aId)].myLayer = aLayer; }
		inline std::string GetLayer(EntityID aId) { return myEntities[GetEntityIndex(aId)].myLayer; }

		inline EntityID GetParent(EntityID aId) { if (!IsEntityIDValid(aId)) return INVALID_ENTITY;  return myEntities[GetEntityIndex(aId)].myParent; }
		inline const std::vector<EntityID>& GetChildren(EntityID aId) { if (!IsEntityIDValid(aId)) return std::vector<EntityID>(); return myEntities[GetEntityIndex(aId)].myChildren; }
		inline size_t GetEntityCount() { return myEntities.size() - myFreeIndexes.size(); }
		inline EntityID GetAssociatedID(EntityIndex aIndex)
		{
			if (aIndex == (EntityIndex)-1 || aIndex >= myEntities.size())
				return INVALID_ENTITY;

			return myEntities[aIndex].myId;
		}
		bool& GetUpdateBehaviours() { return myUpdateBehaviours; }
		void LoadBehaviours();
		CU::Database<true>& GetBehaviourDatabase() { return myBehaviours; }

		inline const std::vector<Entity>& GetEntities() const { return myEntities; }
	private:
		friend class SceneHandler;
		friend class SceneBuilder;
		friend class BaseEditor;
		friend class AssetsManager;
		friend class ProfilingTool;

		//staging
		struct DeactivationArg
		{
			EntityID myId;
			bool myCullArg;
		};

		struct RemoveComponentArg
		{
			ComponentID myComponentId;
			EntityID myId;
		};

		void PostUpdateCalls();
		void PostDestroy(EntityID aID);
		void PostDeactivated(DeactivationArg aArg);
		void PostComponentRemove(RemoveComponentArg aArg);
		void PostBehaviourRemove(RemoveComponentArg aArg);

		std::vector<EntityID> myEntitiesToDestroy;
		std::vector<DeactivationArg> myEntitiesDeactivated;
		std::vector<RemoveComponentArg> myComponentsRemoved;
		std::vector<RemoveComponentArg> myBehavioursRemoved;

		std::vector<Entity> myEntities;
		std::vector<int> myFreeIndexes;
		std::list<std::vector<int>*> myTemporaryStorage;

		std::unordered_map<ComponentID, ComponentPool*> myComponentPools;
		SceneHandler* mySceneHandler = nullptr;

		CU::Database<true> myDatabase;
		CU::Database<true> myBehaviours;

		EntityID myPlayerID = INVALID_ENTITY;

		EntityID myBossID = INVALID_ENTITY;

		size_t myId = 0;
		std::string myName = "";
		std::string myPath = "";
		
		//Astar* myAstar = nullptr; // Legacy

		NavMesh::NavMesh myNavMesh;
		NavMesh::Grid myNavMeshGrid;

		bool myUpdateBehaviours = false;

	};
}