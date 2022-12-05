#pragma once

//#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/ECSB/Behaviour.h"

#include "ISTE/Graphics/Camera.h"

#include <typeinfo>
#include <unordered_map>
#include <functional>

namespace ISTE
{
	class Scene;
	class BehaviourSystem;
	class SceneBuilder;

	struct SimpleCalls
	{
		typedef void(*T)(void*);

		T myConCall = nullptr;
		T myDesCall = nullptr;
	};

	class SceneHandler
	{
	public:
		~SceneHandler();
		SceneHandler();
		void Init();
		void Update(float aTimeDelta);
		void PostLogicUpdate();
		template<typename T>
		ComponentID GetId()
		{
			const char* name = typeid(T).name();
			if (myComponentIds.count(name))
				return myComponentIds[name];

			ComponentID size = (ComponentID)myComponentIds.size();
			myComponentIds.insert({ name, size });

			return size;
		}

		void LoadScene(size_t aIndex, bool aForce = false);
		void LoadScene(std::string aIndex, bool aForce = false);

		//temp function
		void CreateEmptyScene();
		void AddPPEffectsToDatabase();
		//

		inline Scene& GetActiveScene() { return *myActiveScene; }

		inline Camera& GetCamera() { return myCamera; }

		int GetActiveSceneIndex();
		int GetTotalBuildScenes();

		bool IsSceneActive() { return myActiveScene != nullptr; }

		inline const ComponentMask& GetFBM() { return myFBM; }
		inline const ComponentMask& GetFCM() { return myFCM; }

	private:
		friend class EngineRegister;
		friend class GameRegister;
		friend class Scene;

		//temp
		friend class BaseEditor;
		friend class SceneBuilder;
		friend class Inspector;
		friend class AssetsManager;
		friend class ProfilingTool;
		//

		template<typename T>
		void RegisterComponent(size_t aComponentAmount = MAX_ENTITIES)
		{
			myFCM.set(GetId<T>());
			myRegisteredComponents[GetId<T>()] = sizeof(T);
			myComponentIds[typeid(T).name()] = GetId<T>();
			myComponentNames[GetId<T>()] = typeid(T).name();
			myPoolSizes[GetId<T>()] = aComponentAmount;

			SimpleCalls call;
			call.myConCall = [](void* aComponent) {new (aComponent) T(); };
			call.myDesCall = [](void* aComponent) {((T*)aComponent)->~T(); };

			mySimpleCalls.insert({ GetId<T>(), call });
		}

		template<class T>
		void RegisterBehaviour(size_t aComponentAmount = MAX_ENTITIES)
		{
			aComponentAmount;
			if (!std::is_base_of<Behaviour, T>())
			{
				return; //!T was not derived from behaviour!
			}

			myFBM.set(GetId<T>());
			myRegisteredComponents[GetId<T>()] = sizeof(T);
			myComponentIds[typeid(T).name()] = GetId<T>();
			myComponentNames[GetId<T>()] = typeid(T).name();
			myPoolSizes[GetId<T>()] = aComponentAmount;

			SimpleCalls call;
			call.myConCall = [](void* aComponent) {new (aComponent) T(); };
			mySimpleCalls.insert({ GetId<T>(), call });

		}

		inline SceneBuilder* GetSceneBuilder() { return myBuilder; };

		std::unordered_map<std::string, ComponentID> myComponentIds;
		std::unordered_map<ComponentID, std::string> myComponentNames;
		std::unordered_map<ComponentID, size_t> myRegisteredComponents;
		std::unordered_map<ComponentID, size_t> myPoolSizes;

		std::unordered_map<ComponentID, SimpleCalls> mySimpleCalls;

		ComponentMask myFCM;
		ComponentMask myFBM;

		SceneBuilder* myBuilder = nullptr;

		BehaviourSystem* myBehaviourSystem;

		Scene* myActiveScene = nullptr;
		//CU::Database<true> myEnemyBehaviours;
		Camera myCamera;

		bool myWantToLoadBuildScene = false;
		bool myWantToLoadTestScene = false;
		size_t myBuildScene = 0;
		std::string myTestScene = "";
	};
}