
#include "StateStack.h"
#include "State.h"	
#include "ISTE/Context.h"

#include <algorithm> // find, find_if, find_if_not, rotate
#include <iostream>
#include <iterator>  // begin, end
#include <stack>

#define GET_INDEX(stateType) static_cast<size_t>(stateType)

namespace ISTE
{
	void InitStack(StateStack& aStack)
	{
		for (size_t i = 0; i < globalStateCount; ++i)
			aStack.myStates[i].myType = static_cast<StateType>(i);
	}

	bool PushState(StateStack& aStack, StateType aType)
	{
		const auto& name = aStack.myNames[GET_INDEX(aType)];

		// verify state can be created
		if (!aStack.myFactory[GET_INDEX(aType)])
		{
			std::cout << "Error in " << __func__ << ": \"" << name << "\" does not have a creator" << std::endl;
			return false;
		}

		// verify state is not already in stack
		auto itr = std::find_if(std::begin(aStack.myStates), std::end(aStack.myStates), [aType](const auto& aState) { return aState.myType == aType; });
		if (itr < aStack.myStates + aStack.mySize)
		{
			std::cout << "Error in " << __func__ << ": \"" << name << "\" is already in stack" << std::endl;
			return false;
		}

		itr->myType = aStack.myStates[aStack.mySize].myType; // swap old type with new

		State* newState = aStack.myFactory[GET_INDEX(aType)]();
		aStack.myStates[aStack.mySize++] = { aType, newState, StateFlags_Default };

		newState->myCtx = Context::Get();
		newState->myImpl = aStack.myStates + aStack.mySize - 1;
		newState->Init();

		return true;
	}

	bool PopState(StateStack& aStack)
	{
		if (aStack.mySize == 0)
		{
			std::cout << "Error in " << __func__ << ": Pop called on empty stack" << std::endl;
			return false;
		}

		auto& state = aStack.myStates[--aStack.mySize];

		delete state.myBasePtr; state.myBasePtr = nullptr;
		state.myFlags = StateFlags_None;

		return true;
	}

	void UpdateStateImplPtrs(StateStack& aStack)
	{
		for (size_t i = 0; i < aStack.mySize; ++i)
			aStack.myStates[i].myBasePtr->myImpl = &aStack.myStates[i];
	}

	bool MoveStateToTopAndPop(StateStack& aStack, StateType aType)
	{
		auto end = aStack.myStates + aStack.mySize;
		auto itr = std::find_if(aStack.myStates, end, [aType](const auto& aState) { return aState.myType == aType; });

		if (itr == end)
		{
			std::cout << "Error in " << __func__ << ": " << aStack.myNames[GET_INDEX(aType)] << " is not in stack" << std::endl;
			return false;
		}

		if (itr < end - 1)
			std::rotate(itr, itr + 1, end);

		UpdateStateImplPtrs(aStack);

		PopState(aStack);

		return true;
	}

	void MoveState(StateStack& aStack, size_t aStartPos, size_t anEndPos)
	{
		auto begin = aStack.myStates;

		if (aStartPos < anEndPos && anEndPos < aStack.mySize)
		{
			std::rotate(begin + aStartPos, begin + aStartPos + 1, begin + anEndPos + 1);
			UpdateStateImplPtrs(aStack);
		}

		if (anEndPos < aStartPos && aStartPos < aStack.mySize)
		{
			std::rotate(begin + anEndPos, begin + aStartPos, begin + aStartPos + 1);
			UpdateStateImplPtrs(aStack);
		}
	}

	void LoadStack(StateStack& aStack, const StateStackTable& aTable, const std::string& aStackName)
	{
		auto itrStack = aTable.find(aStackName);

		if (itrStack == aTable.end())
		{
			std::cout << "Error in " << __func__ << ": Could not find and load stack \"" << aStackName << "\"" << std::endl;
			return;
		}

		PurgeStack(aStack);

		const std::string* beg = std::begin(aStack.myNames);
		const std::string* end = std::end(aStack.myNames);

		for (const auto& name : itrStack->second)
		{
			const std::string* itrName = std::find(beg, end, name);
			if (itrName != end)
				PushState(aStack, static_cast<StateType>(std::distance(beg, itrName)));
		}
	}

	void PurgeStack(StateStack& aStack)
	{
		while (aStack.mySize > 0)
			PopState(aStack);
	}

	void UpdateStates(StateStack& aStack, float aDt)
	{
		bool update = true;
		std::stack<State*> statesToUpdate;

		for (size_t i = aStack.mySize; i-- > 0;)
		{
			const bool always   = (aStack.myStates[i].myFlags & StateFlags_AlwaysUpdate);
			const bool passthru = (aStack.myStates[i].myFlags & StateFlags_PassthruUpdate);

			if (update || always)
				statesToUpdate.push(aStack.myStates[i].myBasePtr);

			update = update & passthru;
		}

		while (!statesToUpdate.empty())
		{
			statesToUpdate.top()->Update(aDt);
			statesToUpdate.pop();
		}
	}

	void RenderStates(StateStack& aStack)
	{
		bool render = true;
		std::stack<State*> statesToRender;

		for (size_t i = aStack.mySize; i-- > 0;)
		{
			const bool always   = (aStack.myStates[i].myFlags & StateFlags_AlwaysRender);
			const bool passthru = (aStack.myStates[i].myFlags & StateFlags_PassthruRender);

			if (render || always)
				statesToRender.push(aStack.myStates[i].myBasePtr);

			render = render & passthru;
		}

		while (!statesToRender.empty())
		{
			statesToRender.top()->Render();
			statesToRender.pop();
		}
	}
}
