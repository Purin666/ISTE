#include "SystemManager.h"

#include "ISTE/CU/MemTrack.hpp"

namespace ISTE
{
	SystemManager::~SystemManager()
	{
		for (auto item : mySystems)
		{
			delete item.second;
		}

	}

	void SystemManager::EntityDestroyed(EntityID aId)
	{
		for (auto const& pair : mySystems)
		{
			auto const& system = pair.second;

			for (auto& entities : system->myEntities)
			{
				entities.second.erase(aId);
			}
		}
	}

	void SystemManager::EntityDeactivated(EntityID aId)
	{
		for (auto const& pair : mySystems)
		{
			auto const& system = pair.second;

			for (auto& entities : system->myEntities)
			{
				if (system->myIgnoreDeactivated[entities.first])
					continue;

				entities.second.erase(aId);
			}
		}
	}

	//void SystemManager::EntityDeactivated(EntityID aId)
	//{
	//	for (auto const& pair : mySystems)
	//	{
	//		auto const& system = pair.second;

	//		for (auto& entities : system->myEntities)
	//		{
	//			if ((system->myFlags[entities.first] & ECSBSYSTEM_IGNORE_DEACTIVATED) != 0)
	//				continue;

	//			entities.second.erase(aId);
	//		}
	//	}
	//}

	void SystemManager::EntityActivated(EntityID aId, ComponentMask aSignature, bool aIsRoot)
	{
		for (auto const& pair : mySystems)
		{
			auto const& system = pair.second;

			for (auto& signature : system->mySingnatures)
			{
				if (system->myIgnoreDeactivated[signature.first] || (system->myOnlyAcceptRoot[signature.first] && !aIsRoot))
					continue;

				auto const& systemSignature = signature.second;
				auto const& ignoreMask = system->myIgnoreIfMasks[signature.first];
				auto const& ifAny = system->myIfAny[signature.first];

				bool firstCondition = (aSignature & systemSignature) == systemSignature;
				bool secondCondition = ((aSignature & systemSignature) != 0) && ifAny;
				bool thirdCondition = (aSignature & ignoreMask) == 0;

				if ((firstCondition || secondCondition) && thirdCondition)
				{
					system->myEntities[signature.first].insert(aId);
				}
				else
				{
					system->myEntities[signature.first].erase(aId);
				}
			}
		}
	}

	//void SystemManager::EntityActivated(EntityID aId, ComponentMask aSignature, bool aIsRoot)
	//{
	//	for (auto const& pair : mySystems)
	//	{
	//		auto const& system = pair.second;

	//		for (auto& signature : system->mySingnatures)
	//		{

	//			SYSTEM_FLAG sFlag = system->myFlags[signature.first];
	//			if ((sFlag & ECSBSYSTEM_IGNORE_DEACTIVATED) || ((sFlag & ECSBSYSTEM_ONLY_ACCEPT_ROOT) && !aIsRoot))
	//				continue;

	//			auto const& systemSignature = signature.second;
	//			auto const& ignoreMask = system->myIgnoreIfMasks[signature.first];

	//			//bool const& ifAny = system->myIfAny[signature.first];

	//			bool firstCondition = (aSignature & systemSignature) == systemSignature;
	//			bool secondCondition = ((aSignature & systemSignature) != 0) && (sFlag & ECSBSYSTEM_IF_ANY);
	//			bool thirdCondition = (aSignature & ignoreMask) == 0;

	//			if ((firstCondition || secondCondition) && thirdCondition)
	//			{
	//				system->myEntities[signature.first].insert(aId);
	//			}
	//			else
	//			{
	//				system->myEntities[signature.first].erase(aId);
	//			}
	//		}
	//	}
	//}

	void SystemManager::EntitySignatureChanged(EntityID aId, ComponentMask aSignature, bool aIsRoot)
	{
		for (auto const& pair : mySystems)
		{

			auto const& system = pair.second;

			for (auto& signature : system->mySingnatures)
			{
				auto const& onlyRoot = system->myOnlyAcceptRoot[signature.first];

				if (onlyRoot && !aIsRoot)
					continue;

				auto const& systemSignature = signature.second;
				auto const& ignoreMask = system->myIgnoreIfMasks[signature.first];
				auto const& ifAny = system->myIfAny[signature.first];

				bool firstCondition = (aSignature & systemSignature) == systemSignature;
				bool secondCondition = ((aSignature & systemSignature) != 0) && ifAny;
				bool thirdCondition = (aSignature & ignoreMask) == 0;

				if ((firstCondition || secondCondition) && thirdCondition)
				{
					system->myEntities[signature.first].insert(aId);
				}
				else
				{
					system->myEntities[signature.first].erase(aId);
				}
			}
		}
	}

	//void SystemManager::EntitySignatureChanged(EntityID aId, ComponentMask aSignature, bool aIsRoot)
	//{
	//	for (auto const& pair : mySystems)
	//	{

	//		auto const& system = pair.second;

	//		for (auto& signature : system->mySingnatures)
	//		{



	//			SYSTEM_FLAG sFlag = system->myFlags[signature.first];
	//			if ((sFlag & ECSBSYSTEM_ONLY_ACCEPT_ROOT) && !aIsRoot)
	//				continue;

	//			auto const& systemSignature = signature.second;
	//			auto const& ignoreMask = system->myIgnoreIfMasks[signature.first];

	//			bool firstCondition = (aSignature & systemSignature) == systemSignature;
	//			bool secondCondition = ((aSignature & systemSignature) != 0) && (sFlag & ECSBSYSTEM_IF_ANY);
	//			bool thirdCondition = (aSignature & ignoreMask) == 0;

	//			if ((firstCondition || secondCondition) && thirdCondition)
	//			{
	//				system->myEntities[signature.first].insert(aId);
	//			}
	//			else
	//			{
	//				system->myEntities[signature.first].erase(aId);
	//			}
	//		}
	//	}
	//}

	void SystemManager::ClearLists()
	{
		for (auto& s : mySystems)
		{
			for (auto& entityList : s.second->myEntities)
			{
				entityList.second.clear();
			}
		}
	}
}