#pragma once

#include "ISTE/Math/Vec3.h"

namespace ISTE
{
	struct SphereTriggerComponent
	{
		CU::Vec3f myOffset;
		float myRadius = 1;
	};
}