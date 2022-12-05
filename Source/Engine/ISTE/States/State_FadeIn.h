#pragma once

#include "ISTE/StateStack/State.h"

#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"

namespace ISTE
{
	class State_FadeIn : public State
	{
	public:
		~State_FadeIn() = default;

		void Init()				 override;
		void Update(const float) override;
		void Render()			 override;

	private:
		Sprite2DRenderCommand myFade;

		float myElapsedTime;
		float myDuration;
		bool mySkipFirstFrame = true;
	};
}