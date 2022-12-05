#pragma once
#include "ISTE/Math/Vec4.h"
#include "ISTE/Graphics/RenderDefines.h"
namespace ISTE
{
	struct AmbientLightComponent
	{
		CU::Vec4f	myGroundColorAndIntensity;
		CU::Vec4f	mySkyColorAndIntensity;
		float		myCubeMapIntensity		= 1;
		TextureID	myAmbianceTextureID		= -1;
	};
};