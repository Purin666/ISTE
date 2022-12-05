#pragma once

//#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/ECSB/ECSDefines.hpp"

#define MAX_LODLEVEL 4
#define LODLEVEL unsigned short

namespace ISTE
{
	struct LoDComponent
	{
		float myLoDLevels[MAX_LODLEVEL];
		EntityID myLoDEntities[MAX_LODLEVEL] = { INVALID_ENTITY };
		//ModelID myLoDModel[MAX_LODLEVEL];
		LODLEVEL myCurrentLoDLevel = 0;
		bool myUseDistance = true;
	};
}