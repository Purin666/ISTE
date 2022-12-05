#pragma once

#include <map>
#include <string>

#include "ISTE/CU/Helpers.h"

namespace CU
{
	template<bool Copy = false>
	class Database
	{
	public:
		Database() = default;
		Database(const Database<Copy>&) = delete;
		~Database()
		{
			if(Copy)
			{
				for(auto& elem : myDatabase)
				{
					delete elem.second;
				}
			}

			myDatabase.clear();
		}
		template<typename T> 
		void SetValue(const std::string& aName, const T& aValue)
		{
			if (Copy)
			{
				//for safety
				if (myDatabase.count(aName))
				{
					delete myDatabase[aName];
				}
				void* elem = malloc(sizeof(T));

				memcpy(elem, (void*)&aValue, sizeof(T));
				myDatabase[aName] = elem;
			}
			else
			{
				myDatabase[aName] = (void*)&aValue;
			}

#ifdef _DEBUG

			myDataInfo[aName] = typeid(T).name();


#endif // _DEBUG


		}
		template<typename T>
		T& Get(const std::string& aName)
		{
			assert(myDatabase.count(aName));

			return *((T*)myDatabase[aName]);
		}

		template<typename T>
		const T& Get(const std::string& aName) const
		{
			assert(myDatabase.count(aName));

			return *((T*)myDatabase[aName]);
		}

		//Does not check if T is the same type as whatever was saved
		template<typename T>
		bool TryGet(const std::string& aName, T& aOut)
		{
			if (myDatabase.count(aName))
			{
#ifdef _DEBUG
				std::string type = typeid(T).name();
				if (type != myDataInfo[aName])
				{
					std::string assertMsg = "AccessKey: " + aName + "\nArgumentType: " + type + "\nSavedType: " + myDataInfo[aName];
					std::wstring output(assertMsg.begin(), assertMsg.end());
					ASSERT_WITH_MSG(false, output.c_str());
					return false;
				}
#endif
				aOut = *((T*)myDatabase[aName]);
				return true;
			}

			return false;
		}

		const bool Contains(const std::string& aName) { return myDatabase.count(aName); }

		//should only be used if nessecary
		inline std::map<std::string, void*>& GetDatabase() { return myDatabase; }

	private:
		std::map<std::string, void*> myDatabase;

#ifdef _DEBUG
		std::map<std::string, std::string> myDataInfo;
#endif

	};
}