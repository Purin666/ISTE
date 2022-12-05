#pragma once

#include "ISTE/StateStack/State.h"

namespace ISTE
{
	class UI;

	class State_Lose : public State
	{
	public:
		~State_Lose();

		void Init()				 override;
		void Update(const float) override;
		void Render()			 override;

	private:
		UI* myUI = nullptr;

		float myElapsedTime = 0.f;
		float myDuration = 0.f;
	};
}