#pragma once

#include "ISTE/StateStack/State.h"

#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"

namespace ISTE
{
	class State_SplashScreen : public State
	{
	public:
		~State_SplashScreen();

		virtual void Init()				 override;
		virtual void Update(const float) override;
		virtual void Render()			 override;

	private:
		Sprite2DRenderCommand myBackground;

		Sprite2DRenderCommand myLogos[2];

		size_t myLogoIndex;

		int myEmitterIds[5];

		float myElapsedTime;
		float myLogoDuration;
	};
}