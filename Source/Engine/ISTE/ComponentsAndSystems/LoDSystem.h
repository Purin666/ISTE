#pragma once

#include "ISTE/ECSB/System.h"
#include "ISTE/ComponentsAndSystems/LoDComponent.h"

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>

typedef std::function<void(EntityID, LODLEVEL)> LODCALLBACK;


namespace ISTE
{
	class LoDSystem : public System
	{
	public:
		void Update();
		void UpdateAgainstPlayer();
		void UpdateFades();

		void RegisterCallback(std::string aIdentifier, LODCALLBACK aCallBack);
		void RemoveCallback(std::string aIdentifier);
		

	private:
		struct EntityFades
		{
			EntityID myID = INVALID_ENTITY;
			int myProperty = 0;
		};

	private:
		//helpers
		//bool Contains(const std::vector<EntityFades>& someFades, const EntityID& aId);
		void Remove(std::vector<EntityFades>& someFades, const EntityID& aId);
		void Insert(std::vector<EntityFades>& someFades, const EntityID& aId, const int aProperty);
		//

		std::unordered_map<std::string, LODCALLBACK> myCallBacks;
		std::vector<EntityFades> myFadingEntites;
	};
}