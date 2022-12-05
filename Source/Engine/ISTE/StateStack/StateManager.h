#pragma once

#include <string>

namespace ISTE
{
	class GameManager;
	enum class StateType;
	struct StateStack;

	class StateManager
	{
		friend class Engine;
	public:
		// PushState adds a new scene on top of stack, PopState removes topmost scene on stack, LoadStack replaces the current stack with a new one
		// of these, only PushState executes immediately; PopState and LoadStack instead buffers the request and executes it between frames
		// PopState may be called multiple times and will buffer each request; LoadStack will always attempt to use the argument passed in its latest call
		// LoadStack replaces the entire stack and is executed last, so if called successfully, it will de facto override any other requests

		void PushState(StateType aType);
		void PopState(size_t aCount = 1);
		void PopAllStates();
		void LoadStack(const std::string& aStackName);

	private:
		StateStack* myStack;
		size_t		myPopCount = 0;
		std::string	myStackToLoad;
	};
}