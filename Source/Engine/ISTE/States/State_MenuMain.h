#pragma once

#include "ISTE/StateStack/State.h"

namespace ISTE
{
	class UI;

	class State_MenuMain : public State
	{
	public:
		~State_MenuMain();

		virtual void Init()				 override;
		virtual void Update(const float) override;
		virtual void Render()			 override;

	private:
		void LoadLevelAndSetCamera();

		UI* myUI = nullptr;

		float myElapsedTime = 0.f;
	};
}