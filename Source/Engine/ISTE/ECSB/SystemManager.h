#pragma once

#include "ECSDefines.hpp"
#include "System.h"

#include <typeinfo>
#include <unordered_map>

namespace ISTE
{
	class SystemManager
	{
	public:
		~SystemManager();

		template<typename T>
		T* RegisterSystem()
		{
			const char* t = typeid(T).name();

			if (mySystems.find(t) != mySystems.end())
				return (T*)mySystems[t];

			T* system = new T();
			mySystems.insert({ t, (System*)system });

			return system;
		}
		template<typename T>
		T* GetSystem()
		{
			
			const char* t = typeid(T).name();

			if (mySystems.find(t) != mySystems.end())
				return (T*)mySystems[t];

			return nullptr;
		}

		template<class T>
		void AddComplexity(ComponentMask aSignature, int aMapValue = 0, bool aIfAny = false, ComponentMask aIgnoreIfMask = ComponentMask(), bool aIgnoeDeactiavted = false, bool aOnlyTakeRoot = false)
		{
			const char* t = typeid(T).name();

			if (!mySystems.count(t))
				return;

			System* system = mySystems[t];

			system->mySingnatures.insert({ aMapValue, aSignature });
			system->myIgnoreIfMasks.insert({ aMapValue, aIgnoreIfMask });
			system->myIfAny.insert({ aMapValue, aIfAny });
			system->myIgnoreDeactivated.insert({ aMapValue, aIgnoeDeactiavted });
			system->myOnlyAcceptRoot.insert({ aMapValue, aOnlyTakeRoot });
		}

		//template<class T>
		//void AddComplexity(ComponentMask aSignature, int aMapValue = 0, ComponentMask aIgnoreIfMask = ComponentMask(), SYSTEM_FLAG aFlag = 0)
		//{
		//	const char* t = typeid(T).name();

		//	if (!mySystems.count(t))
		//		return;

		//	System* system = mySystems[t];

		//	system->mySingnatures.insert({ aMapValue, aSignature });
		//	system->myIgnoreIfMasks.insert({ aMapValue, aIgnoreIfMask });
		//	system->myFlags.insert({ aMapValue, aFlag });
		//}

		void EntityDestroyed(EntityID aId);

		//these two are a bit ugly
		void EntityDeactivated(EntityID aId);
		void EntityActivated(EntityID aId, ComponentMask aSignature, bool aIsRoot);

		void EntitySignatureChanged(EntityID aId, ComponentMask aSignature, bool aIsRoot);

		void ClearLists();

	private:
		friend class EngineRegister;
		friend class SceneHandler;

		std::unordered_map<const char*, System*> mySystems;

	};
}