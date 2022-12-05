#pragma once

#include "ISTE/ECSB/System.h"
#include "ISTE/Graphics/RenderDefines.h"

namespace ISTE
{
	struct Context;

	class EmitterSystem : public System
	{
	public:
		void Init();

		void SpawnEmitters();
		void UpdateEmitterPositions();
		void PrepareEmitterIcons();

	private:
		Context* myCtx;

		TextureID myEmitterIcon;
	};
}

