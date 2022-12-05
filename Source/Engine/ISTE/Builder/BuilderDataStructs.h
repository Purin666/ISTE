#pragma once

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include <string>
#include <vector>

#include "Json/json.hpp"

namespace ISTE
{
	struct TForm
	{
		CU::Vec3f myPosition;
		CU::Quaternionf myRotation;
		CU::Vec3f myScale;
	};

	struct TextureData
	{
		std::vector<std::string> myAlbedoTextures;
		std::vector<std::string> myNormalTextures;
		std::vector<std::string> myMaterialTextures;
	};

	struct MData
	{
		std::string myMeshPath = "";
		std::vector<std::string> myAnimations;
		TextureData myTextureData;
	};

	struct LoDData
	{
		std::string myLoDModel;
		TextureData myTextureData;
	};

	struct TData
	{
		int myType = 0;
		int myFontSize = 0;
		int myPageSize = 0;
		int mySpawnPoolSize = 0;
		std::string myText = "";
		std::string myFont = "";
		float myRadius = 0.f;
		float myFollowTimer = 0.f;
		float myWriteSpeed = 0.f;
		float myPageFlipDelay = 0.f;
		float myPanTimer = 0.f;
		float myWaitTimer = 0.f;
		float myDegreesToRotate = 0.f;
		bool myOneUse = false;
		bool myWillFollow = false;
		bool myUseAnimation = false;
		bool myUsePages = false;
		bool myUseCamera = false;
		std::vector<std::string> myPages;
		std::vector<CU::Vec3f> mySpawnPool;
	};

	struct GObject
	{
		TForm myTransform;
		MData myModelData;
		TData myTriggerData;
		LoDData myLoDData;
		std::string myTag = "";
		std::string myName = "";
		std::string myLayer = "";
	};

	struct SceneData
	{
		size_t myId = 0;
		std::string myPath = "";
		std::string myName = "";
		std::vector<GObject> myGameObjects;
		//PPEffects CU::Vec3f
		CU::Vec3f myMaxBounds;
		CU::Vec3f myMinBounds;
		CU::Vec3f myBlackPoint;
		CU::Vec3f myTint;
		CU::Vec3f myContrast;
		//Fog CU::Vec3f
		CU::Vec3f myFogColor;
		CU::Vec3f myFogHighlightColor;
		//PPEffects floats
		float myExposure;
		float mySaturation;
		float myBloomBlending;
		//Fog floats
		float myFogStartDistance = 0;
		float myFogDensity = 0;
		float myFogHeightFalloff = 0;
		nlohmann::json myEngineLoadData;
	};

	struct LightData
	{

	};
}