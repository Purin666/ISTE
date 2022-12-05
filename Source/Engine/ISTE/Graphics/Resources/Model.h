#pragma once
#include <unordered_map>
#include <ISTE/Graphics/RenderDefines.h>
#include <ISTE/Graphics/Resources/Mesh.h>
#include <ISTE/Math/Matrix4x4.h>
#include <string>

namespace ISTE {
	struct Bone {
		CU::Matrix4x4f myBindToBone;
		int myParent;
		int myChildren[MAX_BONE_CHILDREN] = {-1};
		int myChildrenCount = 0;
		std::string myName;
	};

	struct Model { 
		~Model() { 
			delete[] myMeshes;
		}
		Bone myBones[MAX_BONE_COUNT];
		size_t myBoneCount = 0;

		std::unordered_map<std::string, int> myBoneNameToId;

		Mesh* myMeshes;
		TextureID myTextures[MAX_MESH_COUNT][MAX_MATERIAL_COUNT] = { TextureID(-1) };
		int myMeshCount = -1;
		ModelID myIndex = -1;

		struct AABBData {
			CU::Vec3f myMin;
			CU::Vec3f myMax;
		} myAABB;

		//make this a bit mask later
		ModelType myModelType;
		std::string myFilePath;
	};
};