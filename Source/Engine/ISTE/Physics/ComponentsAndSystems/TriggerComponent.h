#pragma once

#include "ISTE/Math/Vec3.h"
namespace ISTE
{
	struct TriggerComponent
	{
		CU::Vec3f mySize = {1,1,1};
		CU::Vec3f myOffset = { 0,0,0 };
	};
}