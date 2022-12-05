#pragma once

#include "ISTE/StateStack/State.h"

#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"

namespace ISTE
{
	class State_FadeOut : public State
	{
	public:
		~State_FadeOut() = default;

		void Init()				 override;
		void Update(const float) override;
		void Render()			 override;

	private:
		Sprite2DRenderCommand myFade;
		Sprite2DRenderCommand myLoad;

		float myElapsedTime;
		float myDuration;
	};
}