#pragma once

#include "ISTE/ECSB/System.h"
#include "ISTE/Math/Vec3.h"

#include <Windows.h>

#include "ISTE/CU/RayFromScreen.h"

namespace ISTE
{
	//for now this will exist here

	struct Box
	{
		CU::Vec3f myMin;
		CU::Vec3f myMax;
		CU::Vec3f mySize;
	};

	//

	class MouseObjectSelectionSystem : public System
	{
	public:
		bool IsInside(Box aBox, CU::Vec3f aPosition);
		bool Intersects(Box aBox, ScreenHelper::Ray aRay, float& aDist);
		EntityID GetObjectClickedOn();

	private:
		ScreenHelper helper;
	};
}