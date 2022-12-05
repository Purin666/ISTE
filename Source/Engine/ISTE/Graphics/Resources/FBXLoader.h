#pragma once
#include <string>
#include <vector>

#include <ISTE/CU/HashMap.hpp>
#include <ISTE/Math/Vec.h>
#include <unordered_map>

#include "ISTE/ECSB/ECSDefines.hpp"
#include "../RenderDefines.h"
#include "Model.h"
#include "Animation.h" 

struct aiScene;
struct aiMesh;
struct aiNode;
struct aiNodeAnim;

namespace ISTE
{
	struct Context;
	class FBXLoader
	{
		class SceneHandler;
	public: //structs
		struct FBXResult { 
			operator bool() { return mySuccesFlag; }
			operator Model*() { return myModel; }
			operator Animation*() { return myAnimation; }

			bool mySuccesFlag = false;
			Model* myModel = nullptr;
			Animation* myAnimation = nullptr; 
		};

	public:
		FBXLoader()  = default;
		~FBXLoader() = default;
		void Init();
		FBXResult LoadModel(std::string aFilePath);
		FBXResult LoadAnimation(std::string aFilePath, int aModelId);
	private:
		Context* myCtx;

	private: //Assimp processing 
		void ProcessMesh(Model*&, const aiScene*);
		void ProcessBonesInfluences(Model*&, const aiScene*);
		void ProcessVertecies(std::vector<VertexData>&, const aiMesh*, int aAiMeshId);
		void ProcessTangentSpaces(std::vector<VertexData>&, const aiMesh*);
		void ProcessIndecies(std::vector<unsigned int>&, const aiMesh*);
		void ProcessBoneWeights(std::vector<VertexData>&, Model*&, const aiMesh*);
		void InitMeshes(Model*&);

		void GetNodeNames(std::vector<std::string>& aOutList, aiNode* aNode);
		bool ProcessBoneMap(Model*& aModel, const aiScene* aScene, aiNode* aCurrNode);
		void ConstructBoneNameMap(Model*&, aiMesh*);
		void ProcessBoneHeirachie(Model*&, const aiScene* aScene, int aParentId, aiNode*, int& boneCount);

		bool ProcessAnimation(ModelID aModelIt, Animation*&, const aiScene*);
		bool GetBoneIndxRelativeSkelettonStructure(aiNodeAnim* aAiAnimation, const aiNode* aRoot, int& indx);


		//CU::Matrix4x4f GetTransformOfBone(int aModelIt, aiNode*);

		void AssignMaterials(Model*&, const aiScene*);

	};
};