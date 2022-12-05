#pragma once
#include "ISTE/Math/Matrix4x4.h"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/ECSB/ECSDefines.hpp"

struct DecalRenderCommand {
public:
	bool operator<(DecalRenderCommand& aLeft) { return myDiffLengthToCamera > aLeft.myDiffLengthToCamera; }

	CU::Matrix4x4f myTransform;
	CU::Matrix4x4f myTransformInv;
	
	TextureID myTextureIDs[MAX_MATERIAL_COUNT];
	float myDiffLengthToCamera;

	EntityIndex myEntIndex;
	float myThreshold = 0;	//in radians
	float myLerpValues[3];

	//what the decal is allowed to draw on
	RenderFlags myCoverageFlag =
		RenderFlags::Environment;
};