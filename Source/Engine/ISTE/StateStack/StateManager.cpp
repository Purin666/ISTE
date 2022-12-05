
#include "StateManager.h"
#include "StateStack.h"
#include "State.h"

#include <algorithm> // find_if

namespace ISTE
{
	void StateManager::PushState(StateType aType)
	{
		ISTE::PushState(*myStack, aType);
	}

	void StateManager::PopState(size_t aCount)
	{
		myPopCount += aCount;
	}

	void StateManager::PopAllStates()
	{
		myPopCount = myStack->mySize;
	}

	void StateManager::LoadStack(const std::string& aStackName)
	{
		myStackToLoad = aStackName;
	}
}
