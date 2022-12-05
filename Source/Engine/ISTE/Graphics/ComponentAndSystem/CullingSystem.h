#pragma once

#include <vector>
#include <set>
#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/ECSB/system.h"
#include "ISTE/Math/Vec3.h"

//constexpr int Splits = 8;
#define UwUSplits 15
//#define CULLING
#define _CULLING_UPDATE_OPTIMIZATION_ //not tested enough to guarantee correct logic

class Cell
{
public:
	std::set<EntityID> myEntities;
	CU::Vec3f myPosition;
	CU::Vec3f mySize;
	int id = 0;
	bool myIsActiveState = true;
};

namespace ISTE
{
	class CullingSystem : public System
	{
	public:
		CullingSystem();
		~CullingSystem();
		void Init();
		void SceneLoaded();
		void Update(EntityID aId);
		void EntityDestroyed(EntityID aId);
		void Cull();

		Cell* GetGrid() { return myCells; }

	private:
		Cell myCells[UwUSplits * UwUSplits];
		int myCheckDelay = 0;
		int myCheckDelayCount = 0;

		std::vector<Cell> myLastrelevantCells;

		ComponentID myTransformComponentID;
		ComponentID myModelComponentID;

#ifdef _CULLING_UPDATE_OPTIMIZATION_
		std::vector<int> myIDTOIDMAPPING[MAX_ENTITIES]{ std::vector<int>() };
		//bool myEntityStates[MAX_ENTITIES]{ true };
#endif

	};

}