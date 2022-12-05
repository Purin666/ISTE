#pragma once
#include <vector>
#include <string> 
#include <hash/xxh64_en.hpp> 

namespace CU
{ 
	template <class Key, class Value, int MaxSize, class IteratorType = int>
	class HashMap
	{
	private:
		enum class ElementState
		{
			eOpen,
			eTaken,
			eNextTaken		//used in case two elements has the same hash
		};

		struct Element {
		public:
			Value		myValue	= Value();
			Key			myKey	= Key();
			uint64_t	myHash	= 0;
			int			myState	= 0;
		};

		int mySize = 0;
		Element myMap[MaxSize] = {};
		Element* myAllocatedElements[MaxSize] = {0};

		template<class key>
		uint64_t Hash(key aKey)
		{
			return xxh64::hash((char*)&aKey, sizeof(aKey), 0);
		}
		template<>
		uint64_t Hash(std::string aKey)
		{
			return xxh64::hash((char*)aKey.c_str(), sizeof(char) * aKey.length(), 0);
		}
		template<>
		uint64_t Hash(std::wstring aKey)
		{
			return xxh64::hash((char*)aKey.c_str(), sizeof(wchar_t) * aKey.length(), 0);
		}

	public:
		HashMap()
		{
			mySize = 0;
		}
		 

		IteratorType Insert(const Key& aKey, const Value& aValue)
		{
			uint64_t hash = Hash(aKey);

			if (mySize == MaxSize)
				return -1;


			uint64_t key = hash % MaxSize;
			uint64_t n = 0;
			while (myMap[key].myState != 0 && myMap[key].myHash != hash)
			{
				myMap[key].myState = 2;
				n++;
				key += n * n;
				key = key % MaxSize;
			}

			myMap[key].myValue = aValue;
			myMap[key].myHash = hash;
			myMap[key].myState = 1;

			myAllocatedElements[mySize] = &myMap[key];
			mySize++;
			return (int)key;
		}
		bool Remove(const Key& aKey)
		{
			uint64_t hash = Hash(aKey);

			uint64_t key = hash % MaxSize;
			uint64_t lastKey = 0;
			uint64_t n = 0;
			while (myMap[key].myHash != hash && myMap[key].myState == 2)
			{
				lastKey = key;
				n++;
				key += n * n;
				key = key % MaxSize;
			}
			if (n)
				myMap[lastKey].myState = 1;

			if (myMap[key].myState == 0)
				return false;
			myMap[key].myState = 0;

			return true;
		}

		const Value& GetByIterator(const IteratorType aIt) const
		{
			return myMap[aIt].myValue; 
		}
		Value& GetByIterator(const IteratorType aIt)
		{ 
			return myMap[aIt].myValue; 
		}

		size_t GetSize() { return mySize; }
		
		//Element* GetBucket()
		//{
		//	return myAllocatedElements;
		//}


		const Value& Get(const Key& aKey) const
		{
			uint64_t hash = Hash(aKey);

			uint64_t key = hash % myMap.size();
			uint64_t lastKey;

			uint64_t n = 0;
			while (myMap[key].myHash != hash && myMap[key].myState == 2)
			{
				lastKey = key;
				n++;
				key += n * n;
				key = key % MaxSize;
			}
			return myMap[key].myValue;
		}
		Value& Get(const Key& aKey)
		{
			uint64_t hash = Hash(aKey);

			uint64_t key = hash % MaxSize;
			uint64_t n = 0;
			while (myMap[key].myHash != hash && myMap[key].myState == 2)
			{
				n++;
				key += n * n;
				key = key % MaxSize;
			}
			return myMap[key].myValue;
		}

		bool Exists(const Key& aKey)
		{
			uint64_t hash = Hash(aKey);

			uint64_t key = hash % MaxSize;
			uint64_t n = 0;
			while (myMap[key].myHash != hash && myMap[key].myState != 0)
			{
				n++;
				key += n * n;
				key = key % MaxSize;
			}
			return myMap[key].myState != 0; 
		}

		void ClearFromHeap() {
			for (size_t i = 0; i < mySize; i++)
				delete myAllocatedElements[i]->myValue;


			mySize = 0;
		}  
		void Clear() {
			memset(myMap, 0, sizeof(Element) * MaxSize);
			mySize = 0;
		}
	};
}





/*
#pragma once
#include <vector>
#include <string>
#include <hash/xxh64_en.hpp>

namespace CU
{
	template <class Key, class Value, int MaxSize>
	class HashMap
	{
	private:
		enum class ElementState
		{
			eEmpty,
			eOccuppied,
		};

		struct Element {
		public:
			Value myValue = Value();
			uint64_t myHash = 0;
			ElementState myState = 0;
		};

		int mySize = 0;
		Element myMap[MaxSize] = {};
		Element* myAllocatedElements[MaxSize] = {0};

		template<class key>
		uint64_t Hash(key aKey)
		{
			return xxh64::hash((char*)&aKey, sizeof(aKey), 0);
		}
		template<>
		uint64_t Hash(std::string aKey)
		{
			return xxh64::hash((char*)aKey.c_str(), sizeof(char) * aKey.length(), 0);
		}
		template<>
		uint64_t Hash(std::wstring aKey)
		{
			return xxh64::hash((char*)aKey.c_str(), sizeof(wchar_t) * aKey.length(), 0);
		}

	public:
		HashMap()
		{
			mySize = 0;
		}


		int Insert(const Key& aKey, const Value& aValue)
		{
			uint64_t hash = Hash(aKey);

			if (mySize == MaxSize)
				return -1;


			uint64_t key = hash % MaxSize;
			uint64_t n = 0;
			while (myMap[key].myState != ElementState::eEmpty)
			{
				n++;
				key += n * n;
				key = key % MaxSize;
			}

			myMap[key].myValue = aValue;
			myMap[key].myHash = hash;
			myMap[key].myState = ElementState::eOccuppied;

			myAllocatedElements[mySize] = &myMap[key];
			mySize++;
			return key;
		}
		bool Remove(const Key& aKey)
		{
			uint64_t hash = Hash(aKey);

			uint64_t key = hash % MaxSize;
			uint64_t lastKey = 0;
			uint64_t n = 0;
			while (myMap[key].myHash != hash && myMap[key].myState == 2)
			{
				lastKey = key;
				n++;
				key += n * n;
				key = key % MaxSize;
			}
			if (n)
				myMap[lastKey].myState = 1;

			if (myMap[key].myState == 0)
				return false;
			myMap[key].myState = 0;

			return true;
		}

		const Value& GetByIterator(const int aIt) const
		{
			return myMap[aIt].myValue;
		}
		Value& GetByIterator(const int aIt)
		{
			return myMap[aIt].myValue;
		}

		const Value& Get(const Key& aKey) const
		{
			uint64_t hash = Hash(aKey);

			uint64_t key = hash % myMap.size();
			uint64_t lastKey;

			uint64_t n = 0;
			while (myMap[key].myHash != hash && myMap[key].myState == 2)
			{
				lastKey = key;
				n++;
				key += n * n;
				key = key % MaxSize;
			}
			return myMap[key].myValue;
		}
		Value& Get(const Key& aKey)
		{
			uint64_t hash = Hash(aKey);

			uint64_t key = hash % MaxSize;
			uint64_t n = 0;
			while (myMap[key].myHash != hash && myMap[key].myState == 2)
			{
				n++;
				key += n * n;
				key = key % MaxSize;
			}
			return myMap[key].myValue;
		}

		bool Exists(const Key& aKey)
		{
			uint64_t hash = Hash(aKey);

			uint64_t key = hash % MaxSize;
			uint64_t n = 0;
			while (myMap[key].myHash != hash && myMap[key].myState == 2)
			{
				n++;
				key += n * n;
				key = key % MaxSize;
			}
			return myMap[key].myState != 0;
		}


		void ClearFromHeap() {
			for (size_t i = 0; i < mySize; i++)
				delete myAllocatedElements[i]->myValue;
			mySize = 0;
		}
		void Clear() {
			memset(myMap, 0, sizeof(Element) * MaxSize);
			mySize = 0;
		}
	};
}
*/