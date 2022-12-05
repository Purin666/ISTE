#include "AnimationManager.h"

#include <fstream>

#include "ISTE/Context.h"

#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h>
#include <Json/json.hpp>
/*
	one optimization is to make the nodes index point to other stuff. ie a sparse tree that "+points" to an array database 
*/

bool ISTE::AnimationManager::Init()
{
	myCtx = Context::Get();
	myFbxLoader.Init();
	return true;
}

ISTE::AnimationLoadResult ISTE::AnimationManager::LoadAnimation(ModelID aModelId, const std::string& aFilePath)
{
	if (myAnimations.Exists(aFilePath))
		return { true, myAnimations.Get(aFilePath)->myIndex };


	FBXLoader::FBXResult res = myFbxLoader.LoadAnimation(aFilePath, aModelId);
	Animation* createdAnimation = res;
	createdAnimation->myPath = aFilePath;
	auto from = createdAnimation->myPath.find_last_of('/') + 1;
	createdAnimation->myAnimationName = std::string(createdAnimation->myPath.begin() + from, createdAnimation->myPath.end());
	AnimationID index = myAnimations.Insert(aFilePath, createdAnimation);
	createdAnimation->myIndex = index;
	createdAnimation->myModel = aModelId;
	return { res.mySuccesFlag, index };
}

ISTE::AnimationLoadResult ISTE::AnimationManager::LoadAnimation(EntityID aEntityID, const std::string& anAnimation)
{
	if (!IsEntityIndexValid(aEntityID))
		return{ false, AnimationID(-1) };
	Scene& activeScene = myCtx->mySceneHandler->GetActiveScene();

	const ModelID modelIndex = activeScene.GetComponent<ModelComponent>(aEntityID)->myModelId;
	AnimationLoadResult loadResult = LoadAnimation(modelIndex, anAnimation);

	//in case it failed loading
	if (!loadResult)
		return{ false,AnimationID(-1) };

	//checks if it has a Animator component; assigns it if not; and sets modelId to the component
	AnimatorComponent* animator = activeScene.GetComponent<AnimatorComponent>(aEntityID);
	if (animator == nullptr)
		animator = activeScene.AssignComponent<AnimatorComponent>(aEntityID);

	//sets animation in to the list, increments the count and sets current animation.
	animator->myAnimations[animator->myAnimationCount] = loadResult;
	animator->myAnimationCount++;

	//ew
	if (animator->myCurrentAnimation == AnimationID(-1))
		animator->myCurrentAnimation = loadResult;


	//should it be playing and looping by default?
	//should also probably make this a bitmask
	animator->myAnimationState = AnimationState::ePlaying;

	return loadResult;
}

bool ISTE::AnimationManager::SaveAnimationBlend(const AnimationBlendData& someData, const std::string& aPath) 
{
	nlohmann::json outData;
	outData["RootNode"]["Type"] = (int)someData.myRootNode.myType;
	outData["RootNode"]["DataIndex"] = someData.myRootNode.myDataIndex;
	
	outData["Fetch"]["Size"] = someData.myFetchSize;
	for (size_t i = 0; i < someData.myFetchSize; i++)
	{
		outData["Fetch"]["Data"][i]["Path"] = myAnimations.GetByIterator(someData.myFetchOperations[i].myAnimation)->myPath;
	}

	outData["Addative"]["Size"] = someData.myAddSize;
	for (size_t i = 0; i < someData.myAddSize; i++)
	{
		outData["Addative"]["Data"][i]["Node1"]["Type"]			= (int)someData.myAddativeOperation[i].myNodes[0].myType;
		outData["Addative"]["Data"][i]["Node1"]["DataIndex"]	= (int)someData.myAddativeOperation[i].myNodes[0].myDataIndex;

		outData["Addative"]["Data"][i]["Node2"]["Type"]			= (int)someData.myAddativeOperation[i].myNodes[1].myType;
		outData["Addative"]["Data"][i]["Node2"]["DataIndex"]	= (int)someData.myAddativeOperation[i].myNodes[1].myDataIndex;
	}

	outData["Interp"]["Size"] = someData.myInterpSize;
	for (size_t i = 0; i < someData.myInterpSize; i++)
	{
		outData["Interp"]["Data"][i]["tValue"]		= someData.myInterpOperations[i].myLerpValue;
		outData["Interp"]["Data"][i]["InterpType"]	= (int)someData.myInterpOperations[i].myInterpID.myType;
		outData["Interp"]["Data"][i]["InterpIndx"]	= someData.myInterpOperations[i].myInterpID.myDataIndex;

		outData["Interp"]["Data"][i]["Node1"]["Type"]		= (int)someData.myInterpOperations[i].myNodes[0].myType;
		outData["Interp"]["Data"][i]["Node1"]["DataIndex"]	= (int)someData.myInterpOperations[i].myNodes[0].myDataIndex;
	
		outData["Interp"]["Data"][i]["Node2"]["Type"]		= (int)someData.myInterpOperations[i].myNodes[1].myType;
		outData["Interp"]["Data"][i]["Node2"]["DataIndex"]	= (int)someData.myInterpOperations[i].myNodes[1].myDataIndex;
	}

	//will be broken untill i figure out a better way of fetching what bone name to partial from
	outData["Partial"]["Size"] = someData.myPartialSize;
	for (size_t i = 0; i < someData.myPartialSize; i++)
	{
		outData["Partial"]["Data"][i]["tValue"]		= someData.myPartialOperationData[i].myPartialInfluence; 

		outData["Partial"]["Data"][i]["Node1"]["Type"]		= (int)someData.myPartialOperationData[i].myNodes[0].myType;
		outData["Partial"]["Data"][i]["Node1"]["DataIndex"] = (int)someData.myPartialOperationData[i].myNodes[0].myDataIndex;

		outData["Partial"]["Data"][i]["Node2"]["Type"]		= (int)someData.myPartialOperationData[i].myNodes[1].myType;
		outData["Partial"]["Data"][i]["Node2"]["DataIndex"] = (int)someData.myPartialOperationData[i].myNodes[1].myDataIndex;
	}


	outData["InterConstData"]["Size"] = someData.myInterpConstantSize;
	for (size_t i = 0; i < someData.myInterpConstantSize; i++)
	{
		outData["InterConstData"]["Data"][i]["tValue"] = someData.myInterpConstant[i].myTValue;
	}

	outData["InterLinData"]["Size"] = someData.myInterpLinSize;
	for (size_t i = 0; i < someData.myInterpLinSize; i++)
	{
		outData["InterLinData"]["Data"][i]["Speed"] = someData.myInterpLinear[i].mySpeed;
	}

	outData["InterSinData"]["Size"] = someData.myInterpSinSize;
	for (size_t i = 0; i < someData.myInterpSinSize; i++)
	{
		outData["InterSinData"]["Data"][i]["Speed"]	= someData.myInterpSin[i].mySpeed;
		outData["InterSinData"]["Data"][i]["Mod"]		= someData.myInterpSin[i].myModifier;
	}

	std::ofstream out;
	out.open(aPath);
	if (!out.is_open())
		return false;
	out << outData;
	out.close();
	return true; 
}

ISTE::AnimationBlendData& ISTE::AnimationManager::GetAnimationBlend(AnimationBlendID id)
{
	return myBlendData.GetByIterator(id);
}

AnimationBlendID ISTE::AnimationManager::LoadAnimationBlend(EntityID anEntity, const std::string& aPath)
{
	nlohmann::json inData;
	std::ifstream in;
	in.open(aPath);
	if (!in.is_open())
		return false;
	in >> inData;
	in.close(); 

	AnimationBlendData data;

	data.myRootNode.myType = (AnimBlendOperationType)inData["RootNode"]["Type"];
	data.myRootNode.myDataIndex = inData["RootNode"]["DataIndex"];

	data.myFetchSize = inData["Fetch"]["Size"];
	for (size_t i = 0; i < data.myFetchSize; i++)
	{
		data.myFetchOperations[i].myAnimation = LoadAnimation(anEntity,inData["Fetch"]["Data"][i]["Path"]);
	}

	data.myAddSize = inData["Addative"]["Size"];
	for (size_t i = 0; i < data.myAddSize; i++)
	{
		data.myInterpOperations[i].myNodes[0].myType = (AnimBlendOperationType)inData["Addative"]["Data"][i]["Node1"]["Type"];
		data.myInterpOperations[i].myNodes[0].myDataIndex = inData["Addative"]["Data"][i]["Node1"]["DataIndex"];
		data.myInterpOperations[i].myNodes[1].myType = (AnimBlendOperationType)inData["Addative"]["Data"][i]["Node2"]["Type"];
		data.myInterpOperations[i].myNodes[1].myDataIndex = inData["Addative"]["Data"][i]["Node2"]["DataIndex"];
	}
	data.myInterpSize = inData["Interp"]["Size"];
	for (size_t i = 0; i < data.myInterpSize; i++)
	{
		data.myInterpOperations[i].myLerpValue = inData["Interp"]["Data"][i]["tValue"];
		
		data.myInterpOperations[i].myInterpID.myType		= (InterpolationType)		inData["Interp"]["Data"][i]["InterpType"];
		data.myInterpOperations[i].myInterpID.myDataIndex	=							inData["Interp"]["Data"][i]["InterpIndx"];
		data.myInterpOperations[i].myNodes[0].myType		= (AnimBlendOperationType)	inData["Interp"]["Data"][i]["Node1"]["Type"];
		data.myInterpOperations[i].myNodes[0].myDataIndex	=							inData["Interp"]["Data"][i]["Node1"]["DataIndex"];
		data.myInterpOperations[i].myNodes[1].myType		= (AnimBlendOperationType)	inData["Interp"]["Data"][i]["Node2"]["Type"];
		data.myInterpOperations[i].myNodes[1].myDataIndex	=							inData["Interp"]["Data"][i]["Node2"]["DataIndex"];
	}
	data.myInterpSize = inData["Partial"]["Size"];
	for (size_t i = 0; i < data.myInterpSize; i++)
	{
		data.myInterpOperations[i].myLerpValue = inData["Partial"]["Data"][i]["tValue"];
		 
		data.myInterpOperations[i].myNodes[0].myType		= (AnimBlendOperationType)	inData["Partial"]["Data"][i]["Node1"]["Type"];
		data.myInterpOperations[i].myNodes[0].myDataIndex	=							inData["Partial"]["Data"][i]["Node1"]["DataIndex"];
		data.myInterpOperations[i].myNodes[1].myType		= (AnimBlendOperationType)	inData["Partial"]["Data"][i]["Node2"]["Type"];
		data.myInterpOperations[i].myNodes[1].myDataIndex	=							inData["Partial"]["Data"][i]["Node2"]["DataIndex"];
	}


	data.myInterpConstantSize = inData["InterConstData"]["Size"];
	for (size_t i = 0; i < data.myInterpConstantSize; i++)
	{
		data.myInterpConstant[i].myTValue	= inData["InterConstData"]["Data"][i]["tValue"];
	}

	data.myInterpLinSize = inData["InterLinData"]["Size"];
	for (size_t i = 0; i < data.myInterpLinSize; i++)
	{
		data.myInterpLinear[i].mySpeed = inData["InterLinData"]["Data"][i]["Speed"];  
	} 

	data.myInterpSinSize = inData["InterSinData"]["Size"];
	for (size_t i = 0; i < data.myInterpSinSize; i++)
	{
		data.myInterpSin[i].mySpeed = inData["InterSinData"]["Data"][i]["Speed"];
		data.myInterpSin[i].myModifier = inData["InterSinData"]["Data"][i]["Mod"];
	} 



	AnimationBlendID id = myBlendData.Insert(aPath, data);
	myBlendData.GetByIterator(id).myID = id;
	return id;
}

 