#pragma once

#include "ISTE/StateStack/State.h"

namespace ISTE
{
	class State_Play : public State
	{
	public:
		~State_Play();

		virtual void Init()				 override;
		virtual void Update(const float) override;
		virtual void Render()			 override;

	private:
		float myElapsedTime;
	};
}