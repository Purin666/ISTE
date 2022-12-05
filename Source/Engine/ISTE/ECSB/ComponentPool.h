#pragma once

#include "ECSDefines.hpp"

#include <vector>
namespace ISTE
{
	class ComponentPool
	{
	public:
		~ComponentPool();
		ComponentPool(size_t aComponentSize, size_t aMaxComponentAmount, size_t aMaxEntityAmount);

		template<typename T>
		void ExplicitDeconstructorCalls()
		{
			for (int i = 0; i < myMappedIndexes; i++)
			{
				((T*)(myComponentArray + i * myComponentSize))->~T();
			}
		}

		bool MapIndex(EntityIndex aIndex);
		void* RemoveMapping(EntityIndex aIndex);

		inline void* Get(size_t aIndex)
		{
			if (myIndexArray[aIndex] == -1)
				return nullptr;

			return myComponentArray + myIndexArray[aIndex] * myComponentSize;
		}


	private:
		//spoopy
		friend class ProfilingTool;

		int* myIndexArray = nullptr;
		char* myComponentArray = nullptr;

		int myMappedIndexes = 0;
		std::vector<int> myFreeSpots;

		size_t myComponentSize = 0;
		size_t myComponentAmount = 0;
	};
}