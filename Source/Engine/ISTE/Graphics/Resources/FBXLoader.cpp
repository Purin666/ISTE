#include "FBXLoader.h"

#include <fstream> 
#include <d3d11.h>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <assimp/material.h>

#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/Resources/ModelManager.h"


#include "ISTE/Logger/Logger.h"

#include "Model.h"
#include "Animation.h" 
#include "ISTE/Helper/StringCast.h"

void ISTE::FBXLoader::Init()
{
	myCtx = Context::Get();
}

ISTE::FBXLoader::FBXResult ISTE::FBXLoader::LoadModel(std::string aFilePath)
{
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.0f);
	std::ifstream fileIn(aFilePath);
	if (fileIn.fail())
	{ 
#ifdef _DEBUG
		std::cout << aFilePath << " : Could not be found" << std::endl; 
#endif

		FBXResult res;
		//res.myModel = LoadModel("../Assets/Models/ERROR.fbx").myModel;
		res.mySuccesFlag = false;
		return res;
	}
	fileIn.close();
	
	const aiScene* scene = importer.ReadFile(aFilePath,
		aiProcess_Triangulate				|
		aiProcess_ConvertToLeftHanded		|
		aiProcess_PopulateArmatureData		|
		aiProcess_CalcTangentSpace			|
		aiProcess_GlobalScale               |
		aiProcess_GenBoundingBoxes		  /*|
		aiProcess_PreTransformVertices		| 
		aiProcess_LimitBoneWeights			|
		aiProcess_JoinIdenticalVertices		| //have read that this might cause problems
		aiProcess_GenNormals				|
		aiProcess_GenUVCoords */
	);
	
	if (scene == 0)
	{
#ifdef _DEBUG
		std::cout << aFilePath << " : Failed to load :" << importer.GetErrorString();
#endif
		FBXResult res;
		//res.myModel = LoadModel("../Assets/Models/ERROR.fbx").myModel;
		res.mySuccesFlag = false;
		return res;
	}

	if (scene->mNumMeshes == 0)
	{
#ifdef _DEBUG
		std::cout << aFilePath << " : Failed to load : Has 0 meshes";
#endif
		FBXResult res;
		//res.myModel = LoadModel("../Assets/Models/ERROR.fbx").myModel;
		res.mySuccesFlag = false;
		return res;
	}

	//double scaleConvention = 0.0;
	//scene->mMetaData->Get("UnitScaleFactor", scaleConvention);

	Model* model = new Model();
	model->myMeshes = new Mesh[scene->mNumMeshes];
	model->myMeshCount = scene->mNumMeshes;
	model->myFilePath = aFilePath;
	
	ProcessBoneMap(model,scene, scene->mRootNode);
	ProcessMesh(model, scene);
	int boneC = 0;
	ProcessBoneHeirachie(model, scene, -1, scene->mRootNode, boneC);
	ProcessBonesInfluences(model, scene);
	InitMeshes(model);

	AssignMaterials(model, scene); 

	FBXResult res;
	res.myModel = model;
	res.mySuccesFlag = true;
	return res;
}

ISTE::FBXLoader::FBXResult ISTE::FBXLoader::LoadAnimation(std::string aFilePath, int aModelId)
{
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.0f);

	//checks if file exists
	{
		std::ifstream fileIn(aFilePath);
		if (fileIn.fail())
		{
#ifdef _DEBUG
			std::wstring msg = StringCast<std::wstring>(aFilePath) + L" : Could not be found";
			Logger::DebugOutputError(msg);
			std::cout << aFilePath.c_str() << std::endl; 
#endif  

			FBXResult res;
			res.mySuccesFlag = false;
			return res;
		}
		fileIn.close();
	}

	const aiScene* scene = importer.ReadFile(aFilePath,
		aiProcess_Triangulate				|
		aiProcess_ConvertToLeftHanded		|
		aiProcess_PopulateArmatureData		|
		aiProcess_CalcTangentSpace			|
		aiProcess_GlobalScale			  /*|
		aiProcess_LimitBoneWeights			|
		aiProcess_GenNormals				|
		aiProcess_MakeLeftHanded			|
		aiProcess_GenUVCoords			  */
	);
	if (scene == 0)
	{
#ifdef _DEBUG
		std::cout << aFilePath.c_str() << L" : Assimp couldnt load this" << std::endl;
		Logger::DebugOutputWarning(StringCast<std::wstring>(aFilePath) + L" : Assimp not load this");
#endif  	
		FBXResult res;
		res.mySuccesFlag = false;
		return res;
	}
	 
	double scaleConvention = 0;
	scene->mMetaData->Get("UnitScaleFactor", scaleConvention);
	 
	Animation* createdAnimation = new Animation();
	if (!ProcessAnimation(aModelId, createdAnimation, scene))
	{   
#ifdef _DEBUG
		std::cout << aFilePath << L" : Failed to be processed" << std::endl;
		Logger::DebugOutputWarning(StringCast<std::wstring>(aFilePath) + L" : Failed to be processed");
#endif
		delete createdAnimation; 
		FBXResult res;
		res.mySuccesFlag = false;
		return res;
	}

	createdAnimation->myModel = aModelId;

	FBXResult res;
	res.myAnimation = createdAnimation;
	res.mySuccesFlag = true;
	return res;
}


bool ISTE::FBXLoader::ProcessBoneMap(Model*& aModel, const aiScene* aScene, aiNode* aCurrNode)
{  
	for (size_t i = 0; i < aCurrNode->mNumChildren; i++)
	{
		if (!ProcessBoneMap(aModel, aScene, aCurrNode->mChildren[i]))
			return false;
	}
	if (aCurrNode->mNumMeshes == 0)
	{
		aModel->myBoneNameToId[aCurrNode->mName.C_Str()] = aModel->myBoneCount;
		aModel->myBoneCount++; 
	}
	return true;
}


void ISTE::FBXLoader::ProcessMesh(Model*& aOutModel, const aiScene* aInputScene)
{
	for (size_t i = 0; i < aOutModel->myMeshCount; i++)
	{ 
		aiMesh* processedMesh = aInputScene->mMeshes[i];
		aOutModel->myAABB.myMin.x = min(processedMesh->mAABB.mMin.x, aOutModel->myAABB.myMin.x);
		aOutModel->myAABB.myMin.y = min(processedMesh->mAABB.mMin.y, aOutModel->myAABB.myMin.y);
		aOutModel->myAABB.myMin.z = min(processedMesh->mAABB.mMin.z, aOutModel->myAABB.myMin.z);
		
		aOutModel->myAABB.myMax.x = max(processedMesh->mAABB.mMax.x, aOutModel->myAABB.myMax.x);
		aOutModel->myAABB.myMax.y = max(processedMesh->mAABB.mMax.y, aOutModel->myAABB.myMax.y);
		aOutModel->myAABB.myMax.z = max(processedMesh->mAABB.mMax.z, aOutModel->myAABB.myMax.z);

		std::vector<VertexData> vertecies;
		vertecies.reserve(processedMesh->mNumVertices);
		std::vector<unsigned int> indecies;
		indecies.reserve(processedMesh->mNumFaces * 3);

		ProcessVertecies(vertecies, processedMesh, i);
		ProcessIndecies(indecies, processedMesh);
		  
		aOutModel->myMeshes[i].SetVertexList(vertecies);
		aOutModel->myMeshes[i].SetIndexList(indecies);
	} 
}

void ISTE::FBXLoader::ProcessBonesInfluences(Model*& aOutModel, const aiScene* aInputScene)
{
	for (size_t i = 0; i < aOutModel->myMeshCount; i++)
	{
		aiMesh* processedMesh = aInputScene->mMeshes[i]; 
		ProcessBoneWeights(aOutModel->myMeshes[i].GetVertexList(),  aOutModel, processedMesh);
	}
}

void ISTE::FBXLoader::GetNodeNames(std::vector<std::string>& aOutList, aiNode* aNode)
{
	aOutList.emplace_back(aNode->mName.C_Str());

	for (size_t j = 0; j < aNode->mNumChildren; j++)
	{
		ISTE::FBXLoader::GetNodeNames(aOutList, aNode->mChildren[j]);
	}
}

void ISTE::FBXLoader::ProcessVertecies(std::vector<VertexData>& vertecies, const aiMesh* aiMesh, int aAiMeshIt)
{
	
	for (size_t j = 0; j < aiMesh->mNumVertices; j++)
	{
		VertexData vertexData = { };
		vertexData.myPosition.x = aiMesh->mVertices[j].x; 
		vertexData.myPosition.y = aiMesh->mVertices[j].y; 
		vertexData.myPosition.z = aiMesh->mVertices[j].z; 

		//should handle the rest soon 
		if (aiMesh->HasTextureCoords(0))
		{
			vertexData.myTexCoord.x = aiMesh->mTextureCoords[0][j].x;
			vertexData.myTexCoord.y = aiMesh->mTextureCoords[0][j].y;
		}
		if (aiMesh->HasNormals())
		{
			vertexData.myNormal.x = aiMesh->mNormals[j].x;
			vertexData.myNormal.y = aiMesh->mNormals[j].y;
			vertexData.myNormal.z = aiMesh->mNormals[j].z;
		}
		if (aiMesh->HasVertexColors(j))
		{
			vertexData.myColor.x = aiMesh->mColors[j]->r;
			vertexData.myColor.y = aiMesh->mColors[j]->g;
			vertexData.myColor.z = aiMesh->mColors[j]->b;
			vertexData.myColor.w = aiMesh->mColors[j]->a;
		}
		else
		{
			vertexData.myColor.x = 0.5f;
			vertexData.myColor.y = 0.5f;
			vertexData.myColor.z = 0.5f;
			vertexData.myColor.w = 1.f;
		}
		if (aiMesh->HasTangentsAndBitangents())
		{
			vertexData.myTangent.x = aiMesh->mTangents[j].x;
			vertexData.myTangent.y = aiMesh->mTangents[j].y;
			vertexData.myTangent.z = aiMesh->mTangents[j].z;
		
			vertexData.myBiNormal.x = aiMesh->mBitangents[j].x;
			vertexData.myBiNormal.y = aiMesh->mBitangents[j].y;
			vertexData.myBiNormal.z = aiMesh->mBitangents[j].z;
		}
		else
		{
			CU::Vec3f normal = vertexData.myNormal;
			vertexData.myTangent = normal.Cross({ 0.f,0.f,1.f }).GetNormalized();
			vertexData.myBiNormal = normal.Cross({ 1.f,0.f,0.f }).GetNormalized();
		}
		
		vertecies.emplace_back(vertexData);
	}
}

void ISTE::FBXLoader::ProcessBoneWeights(std::vector<VertexData>& vertecies, Model*& aOutModel, const aiMesh* aMesh)
{ 
	for (size_t i = 0; i < aMesh->mNumBones && i < MAX_BONE_COUNT; i++)
	{
		aiBone* processedBone = aMesh->mBones[i]; 
		//processes the offset matrix
		size_t index = aOutModel->myBoneNameToId.at(processedBone->mName.C_Str());
		{
		if (index >= MAX_BONE_COUNT)
			continue;

		memcpy(&aOutModel->myBones[index].myBindToBone(1, 1), &processedBone->mOffsetMatrix.a1, sizeof(float) * 16);
		aOutModel->myBones[index].myBindToBone = CU::Matrix4x4f::Transpose(aOutModel->myBones[index].myBindToBone);
		//aOutModel->myBoneCount++; 
		} 

		for (size_t j = 0; j < processedBone->mNumWeights; j++)
		{
			size_t vertexId = processedBone->mWeights[j].mVertexId;
			for (int k = 0; k < 4; k++)
			{  
				if ((&vertecies[vertexId].myWeight.x)[k] == 0)
				{
					(&vertecies[vertexId].myWeight.x)[k] = processedBone->mWeights[j].mWeight;
					(&vertecies[vertexId].myBones.x)[k] = index;
					break;
				}
			}
		}
	} 
}

void ISTE::FBXLoader::InitMeshes(Model*& aOutModel)
{
	for (size_t i = 0; i < aOutModel->myMeshCount; i++)
	{
		aOutModel->myMeshes[i].Init();
	}
}

void ISTE::FBXLoader::ConstructBoneNameMap(Model*& aOutModel, aiMesh* aAiMesh)
{ 
	size_t size = aAiMesh->mNumBones;
	if (size >= MAX_BONE_COUNT)
	{
		size = MAX_BONE_COUNT;
	}

	for (size_t i = 0; i < size; i++)
	{
		aOutModel->myBoneNameToId.insert(std::pair<std::string, int>(aAiMesh->mBones[i]->mName.C_Str(), aOutModel->myBoneCount));
		aOutModel->myBoneCount++;
	}
}

void ISTE::FBXLoader::ProcessBoneHeirachie(Model*& aOutModel, const aiScene* aScene, int aParentId, aiNode* aNode, int& boneCount)
{
	//need to rethink this later
	auto& iterator = aOutModel->myBoneNameToId.find(aNode->mName.C_Str());
	if (iterator != aOutModel->myBoneNameToId.end())
	{
		size_t aiBoneIndex = iterator->second = boneCount;
		boneCount++;
		if (aiBoneIndex >= MAX_BONE_COUNT)
			return;

		aOutModel->myBones[aiBoneIndex].myName = iterator->first;
		aOutModel->myBones[aiBoneIndex].myParent = aParentId;
		

		if (aParentId != -1)
		{ 
			aOutModel->myBones[aParentId].myChildren[aOutModel->myBones[aParentId].myChildrenCount] = aiBoneIndex;
			aOutModel->myBones[aParentId].myChildrenCount++; 
		}
		
		aParentId = aiBoneIndex;
	}

	for (size_t i = 0; i < aNode->mNumChildren && i < MAX_BONE_CHILDREN; i++)
	{ 
		ProcessBoneHeirachie(aOutModel, aScene, aParentId, aNode->mChildren[i], boneCount);
	}
}

void ISTE::FBXLoader::ProcessTangentSpaces(std::vector<VertexData>& aVertexlist, const aiMesh* aMesh)
{ 
	//for (size_t j = 0; j < aMesh->mNumFaces; j++)
	//{	
		//aiFace face = aMesh->mFaces[j];
		//finns någon miss calculation jag måste stämma över
		//
		//
		////calculates tangent and binormal
		//CU::Vec3f q2, q1;
		//float s2, s1;
		//float t2, t1;
		//
		//q1 = aVertexlist[face.mIndices[1]].myPosition - aVertexlist[face.mIndices[0]].myPosition;
		//q2 = aVertexlist[face.mIndices[2]].myPosition - aVertexlist[face.mIndices[0]].myPosition;
		//	 
		//s1 = aVertexlist[face.mIndices[1]].myTexCoord.x - aVertexlist[face.mIndices[0]].myTexCoord.x;
		//s2 = aVertexlist[face.mIndices[2]].myTexCoord.x - aVertexlist[face.mIndices[0]].myTexCoord.x;
		//	 
		//t1 = aVertexlist[face.mIndices[1]].myTexCoord.y - aVertexlist[face.mIndices[0]].myTexCoord.y;
		//t2 = aVertexlist[face.mIndices[2]].myTexCoord.y - aVertexlist[face.mIndices[0]].myTexCoord.y;
		//
		//CU::Vec3f tangent = (t2 * q1 - t1 * q2).GetNormalized();
		//CU::Vec3f binormal = ((s2 * -1.f) * q1 - s1 * q2).GetNormalized();
		//
		//aVertexlist[face.mIndices[0]].myTangent = tangent;
		//aVertexlist[face.mIndices[1]].myTangent = tangent;
		//aVertexlist[face.mIndices[2]].myTangent = tangent;
		//
		//aVertexlist[face.mIndices[0]].myBiNormal = binormal;
		//aVertexlist[face.mIndices[1]].myBiNormal = binormal;
		//aVertexlist[face.mIndices[2]].myBiNormal = binormal;
	//}
}

void ISTE::FBXLoader::ProcessIndecies(std::vector<unsigned int>& aIndexList, const aiMesh* aMesh)
{
	for (size_t j = 0; j < aMesh->mNumFaces; j++)
	{
		for (size_t k = 0; k < aMesh->mFaces[j].mNumIndices; k++)
		{
			aIndexList.emplace_back(aMesh->mFaces[j].mIndices[k]);
		}
	}
}

void ISTE::FBXLoader::AssignMaterials(Model*& aOutModel, const aiScene* aInputScene)
{
	//this is disgusting	this can be improved
	std::wstring directoryPath = StringCast<std::wstring>(aOutModel->myFilePath);
	for (size_t i = directoryPath.size() - 1; directoryPath[i] != '/' && directoryPath[i] != '\\'; i--)
		directoryPath.pop_back();
	 
	for (size_t i = 0; i < aInputScene->mNumMeshes; i++)
	{
		//Gets the material for model
		aiMaterial* material = aInputScene->mMaterials[aInputScene->mMeshes[i]->mMaterialIndex];

		//checks if model has a transparant mesh
		float opacity = 1; 
		material->Get(AI_MATKEY_OPACITY, opacity);
		if (opacity <= 0.95f)
			aOutModel->myModelType = ModelType::eTransparantModel;

		//gets the material for texture loading
		aiString materialName;
		material->Get(AI_MATKEY_NAME, materialName);
		std::wstring matName = directoryPath + StringCast<std::wstring>(materialName.C_Str());

		//Loads albedo
		auto loadResult = myCtx->myTextureManager->LoadTexture(matName + L"_C.dds");
		if (!loadResult)
		{
#ifdef _DEBUG
			std::cout << "Did not find Albedo map for " << materialName.C_Str() << std::endl;
			Logger::DebugOutputWarning(L"Did not find Albedo map for " + matName);
#endif 
			loadResult = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/T_Default_C.dds");
		}
		aOutModel->myTextures[i][ALBEDO_MAP] = loadResult;

		//loads Normal + AO map
		loadResult = myCtx->myTextureManager->LoadTexture(matName + L"_N.dds", false);
		if (!loadResult)
		{
#ifdef _DEBUG
			std::cout << "Did not find Normal map for " << materialName.C_Str() << std::endl;
			Logger::DebugOutputWarning(L"Did not find Normal map for " + matName);
#endif   
			loadResult = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/T_Default_N.dds", false);
		}
		aOutModel->myTextures[i][NORMAL_MAP] = loadResult;

		//Loads Material map (metallness, roughness, emissive, e_strngth)
		loadResult = myCtx->myTextureManager->LoadTexture(matName + L"_M.dds", false);
		if (!loadResult)
		{
#ifdef _DEBUG
			std::cout << "Did not find Material map for " << materialName.C_Str() << std::endl;
			Logger::DebugOutputWarning(L"Did not find Material map for " + matName);
#endif  
			loadResult = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/T_Default_M.dds", false);
		}
		aOutModel->myTextures[i][MATERIAL_MAP] = loadResult;
	}
}

bool ISTE::FBXLoader::ProcessAnimation(ModelID aModelIt, Animation*& anAnimation, const aiScene* aScene)
{
	Model* model = myCtx->myModelManager->GetModel(aModelIt);
	std::unordered_map<std::string, int> NameIdMap = model->myBoneNameToId;
	
	if (NameIdMap.empty())
	{
		std::string name = "could not find a Bone to ID map for: " + model->myFilePath;
		Logger::CoutError(name);
	}

	//assuming we are only loading one animation per file
	for (size_t i = 0; i < aScene->mNumAnimations; i++)
	{
		aiAnimation* processedAnim = aScene->mAnimations[i];
		anAnimation->myFps = (float)processedAnim->mTicksPerSecond;
		anAnimation->myKeyFrameCount = anAnimation->myLengthInFrames = (float)processedAnim->mDuration - 1;
		anAnimation->myLengthInSeconds = anAnimation->myLengthInFrames / anAnimation->myFps;
		anAnimation->myKeyFrames.resize(anAnimation->myKeyFrameCount + 2);

		for (size_t j = 0; j < processedAnim->mNumChannels; j++)
		{
			aiNodeAnim* currentBone = processedAnim->mChannels[j];
			auto& indxNameMap = NameIdMap.find(currentBone->mNodeName.C_Str());
			if (indxNameMap == NameIdMap.end())
				continue;

			size_t boneIndex = indxNameMap->second;
			NameIdMap.erase(currentBone->mNodeName.C_Str());
			
			//position
			for (size_t k = 0; k < currentBone->mNumPositionKeys; k++)
			{
				auto AssPosKey = currentBone->mPositionKeys[k];
				Animation::KeyFrame& currentKeyFrame = anAnimation->myKeyFrames[(size_t)AssPosKey.mTime];
		
				CU::Vec3f& pos = currentKeyFrame.myTransforms[boneIndex].myPosition;
				memcpy(&pos, &AssPosKey.mValue, sizeof(float) * 3); 
			}
		
			//scale 
			for (size_t k = 0; k < currentBone->mNumScalingKeys; k++)
			{
				auto AssScaleKey = currentBone->mScalingKeys[k];
				Animation::KeyFrame& currentKeyFrame = anAnimation->myKeyFrames[(size_t)AssScaleKey.mTime];
		
				CU::Vec3f& scale = currentKeyFrame.myTransforms[boneIndex].myScale;
				memcpy(&scale, &AssScaleKey.mValue, sizeof(float) * 3); 
			}
			
			//rotation 
			for (size_t k = 0; k < currentBone->mNumRotationKeys; k++)
			{ 
				auto AssRotKey = currentBone->mRotationKeys[k];
				Animation::KeyFrame& currentKeyFrame = anAnimation->myKeyFrames[(size_t)AssRotKey.mTime];
				
				aiMatrix3x3 aiM = AssRotKey.mValue.GetMatrix();
				CU::Quaternionf& quat = currentKeyFrame.myTransforms[boneIndex].myQuaternion;
		
				memcpy(&quat.w, &AssRotKey.mValue.w, sizeof(float) * 4);
				quat *= -1.f;   
			}
		}

		for (auto& it : NameIdMap)
		{
			int boneIndex = it.second;
			auto aiN = aScene->mRootNode->FindNode(it.first.c_str());
			if (aiN == nullptr)
				continue; 

			CU::Matrix4x4f m;
			auto aiM = aiN->mTransformation;
			memcpy(&m(1, 1), &aiM.a1, sizeof(float) * 16);
			m = CU::Matrix4x4f::Transpose(m);

			for (size_t j = 0; j < anAnimation->myKeyFrameCount + 1; j++)
			{
				Animation::KeyFrame& currentKeyFrame = anAnimation->myKeyFrames[j];
				//scale
				{
					CU::Vec3f& scale = currentKeyFrame.myTransforms[boneIndex].myScale;
					CU::Vec3f AssScale = m.DecomposeScale();
					scale = AssScale;
				}
			
				//rotation
				{
					CU::Quaternionf& quat = currentKeyFrame.myTransforms[boneIndex].myQuaternion;
					CU::Quaternionf AssQuat(m);
					quat = AssQuat;
				}
				//pos
				{
					CU::Vec3f& pos = currentKeyFrame.myTransforms[boneIndex].myPosition;
					CU::Vec3f AssPoss = m.GetTranslationV3();
					pos = AssPoss;
				}
			}
		}
	}
	return true;
}

bool ISTE::FBXLoader::GetBoneIndxRelativeSkelettonStructure(aiNodeAnim* aAiAnimation, const aiNode* aRoot, int& outIndex)
{
	for (size_t i = 0; i < aRoot->mNumChildren; i++)
	{
		outIndex++;
		if (aRoot->mName == aAiAnimation->mNodeName ||
			GetBoneIndxRelativeSkelettonStructure(aAiAnimation, aRoot->mChildren[i], outIndex))
			return true; 
	}
	return false;
}
