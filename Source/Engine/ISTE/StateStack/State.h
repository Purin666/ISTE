#pragma once

namespace ISTE
{
	struct StateStack;
	enum class StateType;
	struct Context;
	struct StateImpl;

	enum StateFlags
	{
		StateFlags_None				= 0,
		StateFlags_PassthruUpdate	= 1 << 0, // Allow states under the owner of the flag to be updated.
		StateFlags_PassthruRender	= 1 << 1, // Allow states under the owner of the flag to be rendered.
		StateFlags_AlwaysUpdate		= 1 << 2, // Update state regardless of whether above states have PassthruUpdate enabled.
		StateFlags_AlwaysRender		= 1 << 3, // Update state regardless of whether above states have PassthruRender enabled.

		StateFlags_Default = StateFlags_PassthruUpdate | StateFlags_PassthruRender
	};

	StateFlags operator|(StateFlags, StateFlags);

	class State
	{
		friend bool PushState(StateStack&, StateType);
		friend void UpdateStateImplPtrs(StateStack&);
	public:
		virtual ~State() = default;

		virtual void Init()				 = 0;
		virtual void Update(const float) = 0;
		virtual void Render()			 = 0;

		void SetStateFlags(StateFlags);

	protected:
		Context* myCtx;

	private:
		StateImpl* myImpl;
	};
}