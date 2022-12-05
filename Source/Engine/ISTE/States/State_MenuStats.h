#pragma once

#include "ISTE/StateStack/State.h"

namespace ISTE
{
	class UI;

	class State_MenuStats : public State
	{
	public:
		~State_MenuStats();

		virtual void Init()				 override;
		virtual void Update(const float) override;
		virtual void Render()			 override;

	private:
		UI* myUI = nullptr;

		float myElapsedTime = 0.f;
	};
}