#pragma once
#include <assert.h>

namespace CU
{
	template<typename T, int size, typename CountType = unsigned short, bool UseSafeModeFlag = true>
	class VectorOnStack
	{
	public:
		VectorOnStack() : myCount(0), myData()
		{}
		VectorOnStack(const VectorOnStack& aVectorOnStack)
		{
			if (!mySafeMode)
			{
				memcpy(myData, aVectorOnStack.myData, sizeof(myData));
				myCount = aVectorOnStack.myCount;
				return;
			}
			myCount = aVectorOnStack.myCount;
			for (CountType i = 0; i < myCount; i++)
			{
				myData[i] = aVectorOnStack.myData[i];
			}
		}
		VectorOnStack(const std::initializer_list<T>& aInitList)
		{
			assert(aInitList.size() == size && "Initializer list is not the right size.");
			for (int i = 0; i < aInitList.size(); i++)
			{
				myData[i] = *(aInitList.begin() + i);
			}
			myCount = aInitList.size();
		}
		~VectorOnStack()
		{
			myCount = 0;
		}

		VectorOnStack& operator=(const VectorOnStack& aVectorOnStack)
		{
			if (!mySafeMode)
			{
				memcpy(myData, aVectorOnStack.myData, sizeof(myData));
				myCount = aVectorOnStack.myCount;
				return *this;
			}
			myCount = aVectorOnStack.myCount;
			for (CountType i = 0; i < myCount; i++)
			{
				myData[i] = aVectorOnStack.myData[i];
			}
			return *this;
		}
		inline const T& operator[](const CountType aIndex) const
		{
			assert(0 >= aIndex && "Index is less than zero");
			assert(aIndex < myCount && "Index is out of bounds");
			return myData[aIndex];
		}
		inline T& operator[](const CountType aIndex)
		{
			assert(0 <= aIndex && "Index is less than zero");
			assert(aIndex < myCount && "Index is out of bounds");
			return myData[aIndex];
		}
		inline void Add(const T& aObject)
		{
			assert(myCount < size && "Vector is full");
			if (!mySafeMode)
			{
				memcpy(&myData[myCount], &aObject, sizeof(aObject));
				myCount++;
				return;
			}
			myData[myCount] = aObject;
			myCount++;
		}

		inline void Insert(const CountType aIndex, const T& aObject)
		{

			assert(aIndex <= myCount && aIndex >= 0 && "Index is out of bounds");
			if (!mySafeMode)
			{
				memcpy(&myData[aIndex], &aObject, sizeof(aObject));
				return;
			}

			if (myCount > 0)
			{
				for (CountType i = myCount - 1; i >= aIndex; i--)
				{
					myData[i + 1] = myData[i];
				}
			}

			myData[aIndex] = aObject;
			++myCount;
		}

		inline void RemoveCyclic(const T& aObject)
		{
			assert(myCount > 0 && "Size is invalid");

			for (CountType i = 0; i < myCount; i++)
			{
				if (myData[i] == aObject)
				{
					RemoveCyclicAtIndex(i);
					return;
				}
			}
		}

		inline void RemoveCyclicAtIndex(const CountType aIndex)
		{
			assert(myCount > 0 && "size is invalid");
			assert(aIndex < myCount&& aIndex >= 0 && "Index is out of bounds");


			myData[aIndex] = myData[myCount - 1];
			--myCount;
		}

		inline void Clear()
		{
			myCount = 0;
		}

		__forceinline CountType Size() const
		{
			return myCount;
		}

	private:
		bool mySafeMode = UseSafeModeFlag;
		T myData[size];
		CountType myCount;
	};
}