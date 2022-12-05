
#include "State.h"
#include "StateStack.h"
#include "ISTE/Context.h"

namespace ISTE
{
	StateFlags operator|(StateFlags l, StateFlags r)
	{
		return static_cast<StateFlags>(static_cast<int>(l) | static_cast<int>(r));
	}

	void State::SetStateFlags(StateFlags someFlags)
	{
		myImpl->myFlags = someFlags;
	}
}