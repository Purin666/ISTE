#pragma once
#include <bitset>
#include "ISTE/Graphics/RenderDefines.h"

namespace ISTE
{
	struct MaterialComponent 
	{
		MaterialComponent()
		{
			memset(myTextures, TextureID(-1), sizeof(TextureID) * MAX_MESH_COUNT * MAX_MATERIAL_COUNT);
		}
		void OverwriteTexture(TextureID aTexID, int aMap)
		{
			for (size_t i = 0; i < MAX_MESH_COUNT; i++)
				myTextures[i][aMap] = aTexID;
		}

		TextureID		myTextures[MAX_MESH_COUNT][MAX_MATERIAL_COUNT];
		
		/*
			some rendering flags 
			check renderdefines for the flags
		*/
		RenderFlags myRenderFlags = 
			RenderFlags::Environment;
	};
};