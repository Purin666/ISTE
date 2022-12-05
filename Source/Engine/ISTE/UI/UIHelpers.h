#pragma once

#include "UIDefines.h"
#include "ISTE/Math/Vec2.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/DebugDrawer.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/WindowsWindow.h"


namespace ISTE
{
	namespace UIHelpers
	{
		static const bool IsInside(const CU::Vec3f& aPosition, const CU::Vec3f& aScale, const CU::Vec2f& aMousePos)
		{
			const CU::Vec2f min = { aPosition.x - aScale.x, aPosition.y - aScale.y };
			const CU::Vec2f max = { aPosition.x + aScale.x, aPosition.y + aScale.y };

			return	aMousePos.x >= min.x &&
					aMousePos.x <= max.x &&
					aMousePos.y >= min.y &&
					aMousePos.y <= max.y;
		}
	}
}