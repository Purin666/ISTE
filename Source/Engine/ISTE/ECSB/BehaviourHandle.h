#pragma once

#include "Behaviour.h"

#include <vector>
namespace ISTE
{
	class BehaviourHandle
	{
	public:
		template<typename T>
		void AddBehaviour(T* aComponent)
		{
			myBehaviours.push_back((Behaviour*)aComponent);
		}
		void AddBehaviour(Behaviour* aBehaviour)
		{
			myBehaviours.push_back(aBehaviour);
		}
		template<typename T>
		void RemoveBehaviour()
		{
			int counter = 0;
			for (auto* behaviour : myBehaviours)
			{
				if (typeid(T).name() == typeid(*behaviour).name())
				{
					myBehaviours.erase(myBehaviours.begin() + counter);
					break;
				}

				counter++;
			}
		}
		template<typename T>
		T* GetBehaviour()
		{
			for (auto* behaviour : myBehaviours)
			{
				if (typeid(T).name() == typeid(*behaviour).name())
				{
					return (T*)behaviour;
				}
			}
		}

		inline std::vector<Behaviour*>& GetEverything()
		{
			return myBehaviours;
		}

	private:
		std::vector<Behaviour*> myBehaviours;
	};
}