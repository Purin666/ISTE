#pragma once

#include "StateType.h"

#include <unordered_map>
#include <functional>
#include <string>
#include <typeinfo>

namespace ISTE
{
	using StateStackTable = std::unordered_map<std::string, std::vector<std::string>>;

	class State;
	enum  StateFlags;

	struct StateImpl
	{
		StateType  myType;
		State*     myBasePtr;
		StateFlags myFlags;
	};

	struct StateStack
	{
		~StateStack()
		{
			while (mySize > 0)
			{
				auto& state = myStates[--mySize];

				delete state.myBasePtr; state.myBasePtr = nullptr;
				state.myFlags = (StateFlags)0;
			}
		}

		using CreateFn = std::function<State* ()>;

		size_t	  mySize = 0;
		StateImpl myStates[globalStateCount];

		// Below arrays are kept constant after initialization.
		// They are indexed by underlying value of StateType.

		std::string  myNames[globalStateCount];		// value(State Type) |-> Name
		CreateFn	 myFactory[globalStateCount];	// value(State Type) |-> CreateFn
	};

	void InitStack(StateStack&);

	template <class State>
	void RegisterState(StateStack& aStack, StateType aType)
	{
		aStack.myNames[static_cast<size_t>(aType)]   = (typeid(State).name() + 12); // given "class [name]", skip "class ISTE::"
		aStack.myFactory[static_cast<size_t>(aType)] = []() { return new State(); };
	}

	bool PushState(StateStack&, StateType);
	bool PopState(StateStack&);

	void UpdateStateImplPtrs(StateStack&);

	bool MoveStateToTopAndPop(StateStack&, StateType);
	void MoveState(StateStack&, size_t aStartPos, size_t anEndPos);

	void LoadStack(StateStack&, const StateStackTable&, const std::string& aStackName);
	void PurgeStack(StateStack&); // purges from top to bottom

	void UpdateStates(StateStack&, float aDeltaTime);
	void RenderStates(StateStack&);
}