#include "ComponentPool.h"

#include "ISTE/CU/MemTrack.hpp"

namespace ISTE
{
	ComponentPool::~ComponentPool()
	{
		std::free(myIndexArray);
		std::free(myComponentArray);
	}

	ComponentPool::ComponentPool(size_t aComponentSize, size_t aMaxComponentAmount, size_t aMaxEntityAmount)
	{
		myIndexArray = (int*)malloc(aMaxEntityAmount * sizeof(int));
		myComponentArray = (char*)malloc(aMaxComponentAmount * aComponentSize);
		myComponentSize = aComponentSize;
		myComponentAmount = aMaxComponentAmount;

		for (int i = 0; i < aMaxEntityAmount; i++)
		{
			myIndexArray[i] = -1;
		}
	}

	bool ComponentPool::MapIndex(EntityIndex aIndex)
	{
		if (myIndexArray[aIndex] != -1)
			return true;

		if (myFreeSpots.size() != 0)
		{
			myIndexArray[aIndex] = myFreeSpots[0];
			myFreeSpots.erase(myFreeSpots.begin());
			return true;
		}

		if (myMappedIndexes == myComponentAmount)
			return false;

		myIndexArray[aIndex] = myMappedIndexes;

		myMappedIndexes++;

		return true;


	}

	void* ComponentPool::RemoveMapping(EntityIndex aIndex)
	{
		if (myIndexArray[aIndex] == -1)
			return nullptr;

		void* cmp = Get(aIndex);

		myFreeSpots.push_back(myIndexArray[aIndex]);
		myIndexArray[aIndex] = -1;

		return cmp;
	}
}