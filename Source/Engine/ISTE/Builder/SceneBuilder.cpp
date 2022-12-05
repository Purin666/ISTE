#include "SceneBuilder.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/AnimationManager.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include <Json/json.hpp>

#include <filesystem>
#include <fstream>

#include <chrono>

#include "ISTE/CU/ReadWrite.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/CU/StopWatch.h"

#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"
#include "ISTE/VFX/SpriteParticles/EmitterSystem.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/PP_FS_Effects/MotionBlurEffect.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Builder/BuilderDataStructs.h"
//#include "../Engine/ISTE/Navigation/Legacy/NavMesh/NavMeshImporter.h" // Legacy

//pools
#include "../ECSB/ComponentPool.h"
//

//components
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/DirectionalLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/ComponentsAndSystems/LoDComponent.h"
//

#include "ISTE/Events/EventHandler.h"

//System
#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"

#include <Windows.h>
#include <lmcons.h>

#include "ISTE/CU/Helpers.h"

#pragma comment(lib, "winmm.lib")

#include "ISTE/CU/MemTrack.hpp"

#include "ISTE/CU/Worker.h"



// test
#include "ISTE/Navigation/NavMeshUtilities.h"

#include "ISTE/CU/StopWatch.h"

std::string names[13]
{

};

void Check()
{
	char* user = getenv("username");
	std::string name(user);

	//if (name == "george.chahine")
	//{
	//	PlaySound(L"../Assets/Audio/Music/Rebecca_Black_Friday.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//}
	//else if (name == "olivia.xu")
	//{
	//	PlaySound(L"../Assets/Audio/Music/HudsonMohawke-Cbat.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//}
	if (name == "mathias.boman")
	{
		ISTE::Context::Get()->myGraphicsEngine->GetMBlurEffect().SetEnabled(true);
		ISTE::Context::Get()->myGraphicsEngine->GetMBlurEffect().GetBufferData().mySamples = 32;
		ISTE::Context::Get()->myGraphicsEngine->GetMBlurEffect().GetBufferData().myBlurStrength = 0.15;
		//PlaySound(L"../Assets/Audio/Music/CFAR.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		//std::cout << "Hello " << name << " hope that you're doing great and I love you" << std::endl;
	}
	//else if (name == "loke.djerf")
	//{
	//	//PlaySound(L"../Assets/Audio/Music/One-eyed_Maestro.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//}
	////else if (name == "laura.sanderson")
	////{
	////	ISTE::Context::Get()->myTimeHandler->SetTimeScale(2.f);
	////}
	//else
	//{
	//	//PlaySound(L"../Assets/Audio/Music/CFAR.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//}

	//system("start chrome https://musiclab.chromeexperiments.com/Shared-Piano/#PqUzMBSC2");

	std::cout << name << " you can do it!" << std::endl;
}

namespace ISTE
{
	SceneBuilder::~SceneBuilder()
	{
		//delete myNavMeshImporter; // Legacy

		for (auto& pair : myObjectBuilders)
		{
			delete pair.second;
		}

		for (auto& crawPair : myCRAWs)
		{
			delete crawPair.second;
		}


		myCRAWs.clear();
		myObjectBuilders.clear();
	}

	void SceneBuilder::Init()
	{
		Check();
		std::string buildPath = "../Assets\\Scenes\\EpicUwUStuff\\";
		//std::string testPath = "../Assets\\Scenes\\EpicUwUTest\\";

		struct Data
		{
			std::string file;
			std::string name;
			bool saveToBuild = true;
		};

		std::vector<Data> totalData;

		int iterator = 0;
		for (auto& path : std::filesystem::directory_iterator(buildPath))
		{
			std::string file = path.path().u8string();
			size_t slashOffset = file.find_last_of("\\");
			size_t dotOffset = file.find_last_of(".");

			if (file.substr(dotOffset) != ".uwu")
				continue;

			std::string name = file.substr(slashOffset + 1, dotOffset - slashOffset - 1);

			myMappedBuldScenes.insert({ file, iterator });

			Data data;

			data.file = file;
			data.name = name;

			totalData.push_back(data);
			iterator++;
			//GetSceneData(file, name);
		}

		//iterator = 0;
		//for (auto& path : std::filesystem::directory_iterator(testPath))
		//{
		//	std::string file = path.path().u8string();
		//	size_t slashOffset = file.find_last_of("\\");
		//	size_t dotOffset = file.find_last_of(".");

		//	if (file.substr(dotOffset) != ".uwu")
		//		continue;

		//	std::string name = file.substr(slashOffset + 1, dotOffset - slashOffset - 1);

		//	myMappedTestScenes.insert({ file, iterator });

		//	Data data;

		//	data.file = file;
		//	data.name = name;
		//	data.saveToBuild = false;

		//	totalData.push_back(data);
		//	iterator++;
		//	//GetSceneData(file, name, false);
		//}


		size_t dataSplit = totalData.size() / 2;
		size_t restData = totalData.size() - dataSplit;
		size_t dataSize = dataSplit * sizeof(Data);
		dataSize += dataSize % 64;
		alignas(64) char* data1 = new char[dataSize];
		dataSize = (totalData.size() - dataSplit) * sizeof(Data);
		dataSize += dataSize % 64;
		alignas(64) char* data2 = new char[dataSize];

		memcpy(&data1[0], &totalData[0], sizeof(Data) * dataSplit);
		memcpy(&data2[0], &totalData[dataSplit], sizeof(Data) * (totalData.size() - dataSplit));

		CU::Worker<std::vector<SceneData>> worker;

		//should maybe let the main thread and one worker run instead of two workers
		worker.AddWroker([this, &data1, dataSplit]()
			{
				std::vector<SceneData> d;

				for (size_t i = 0; i < dataSplit; i++)
				{
					Data da = *(Data*)&data1[i * sizeof(Data)];
					d.push_back(GetSceneDataT(da.file, da.name, da.saveToBuild));
				}

				return d;
			});
		worker.AddWroker([this, &data2, restData]()
			{
				std::vector<SceneData> d;

				for (int i = 0; i < restData; i++)
				{
					Data da = *(Data*)&data2[i * sizeof(Data)];
					d.push_back(GetSceneDataT(da.file, da.name, da.saveToBuild));
				}

				return d;
			});


		worker.Run();

		std::vector<CU::WorkerContainer<std::vector<SceneData>>> container = worker.GetContainer();

		std::sort(container.begin(), container.end(), [](const CU::WorkerContainer<std::vector<SceneData>>& aLeft, CU::WorkerContainer<std::vector<SceneData>>& aRight)
			{return aLeft.myId < aRight.myId; }
		);

		int total = 0;
		for (auto& d : container)
		{
			for (auto& data : d.myData)
			{
				if (totalData[total].saveToBuild)
				{
					data.myId = myBuildScenes.size();
					myBuildScenes.push_back(data);
				}
				else
				{
					data.myId = myTestScenes.size();
					myTestScenes.push_back(data);
				}

				total++;
			}
		}

		delete[] data1;
		delete[] data2;

		return;

	}

	SceneData SceneBuilder::GetSceneDataT(std::string aPath, std::string aName, bool)
	{
		SceneData data;
		data.myName = aName;
		data.myPath = aPath;

		//testLock.lock();
		std::ifstream stream(aPath);

		nlohmann::json j;

		stream >> j;

		stream.close();
		//testLock.unlock();

		nlohmann::json json = j["SceneData"];
		nlohmann::json jsonDatabase = j["Database"];

		//Culling maybe?
		data.myMaxBounds = CU::Vec3f(json["MaxBounds"]["x"].get<float>(), json["MaxBounds"]["y"].get<float>(), json["MaxBounds"]["z"].get<float>());
		data.myMinBounds = CU::Vec3f(json["MinBounds"]["x"].get<float>(), json["MinBounds"]["y"].get<float>(), json["MinBounds"]["z"].get<float>());

		//CU::Vec3f test = CU::Vec3f(json["MaxBounds"]["x"].get<float>(), json["MaxBounds"]["y"].get<float>(), json["MaxBounds"]["z"].get<float>());
		//test = CU::Vec3f(json["MinBounds"]["x"].get<float>(), json["MinBounds"]["y"].get<float>(), json["MinBounds"]["z"].get<float>());
		//test = CU::Vec3f(jsonDatabase["Blackpoint"]["r"].get<float>(), jsonDatabase["Blackpoint"]["g"].get<float>(), jsonDatabase["Blackpoint"]["b"].get<float>());
		//test = CU::Vec3f(jsonDatabase["Tint"]["r"].get<float>(), jsonDatabase["Tint"]["g"].get<float>(), jsonDatabase["Tint"]["b"].get<float>());
		//test = data.myContrast = CU::Vec3f(jsonDatabase["Contrast"]["r"].get<float>(), jsonDatabase["Contrast"]["g"].get<float>(), jsonDatabase["Contrast"]["b"].get<float>());

		if(j.contains("EngineSceneData"))
		{
			data.myEngineLoadData = j["EngineSceneData"];
		}

		try {
			//PPEffects
			data.myBlackPoint = CU::Vec3f(jsonDatabase["Blackpoint"]["r"].get<float>(), jsonDatabase["Blackpoint"]["g"].get<float>(), jsonDatabase["Blackpoint"]["b"].get<float>());
			data.myTint = CU::Vec3f(jsonDatabase["Tint"]["r"].get<float>(), jsonDatabase["Tint"]["g"].get<float>(), jsonDatabase["Tint"]["b"].get<float>());
			data.myContrast = CU::Vec3f(jsonDatabase["Contrast"]["r"].get<float>(), jsonDatabase["Contrast"]["g"].get<float>(), jsonDatabase["Contrast"]["b"].get<float>());
			data.myBloomBlending = jsonDatabase["BloomBlending"].get<float>();
			data.mySaturation = jsonDatabase["Saturation"].get<float>();
			data.myExposure = jsonDatabase["Exposure"].get<float>();
			//Fog
			data.myFogColor = CU::Vec3f(jsonDatabase["FogColor"]["r"].get<float>(), jsonDatabase["FogColor"]["g"].get<float>(), jsonDatabase["FogColor"]["b"].get<float>());
			data.myFogHighlightColor = CU::Vec3f(jsonDatabase["FogHighlightColor"]["r"].get<float>(), jsonDatabase["FogHighlightColor"]["g"].get<float>(), jsonDatabase["FogHighlightColor"]["b"].get<float>());
			data.myFogStartDistance = jsonDatabase["FogStartDist"].get<float>();
			data.myFogDensity = jsonDatabase["FogGlobalDensity"].get<float>();
			data.myFogHeightFalloff = jsonDatabase["FogHeightFalloff"].get<float>();
		}
		catch (...)
		{
			data.myBloomBlending = .3f;
			data.mySaturation = 1.0f;
			data.myExposure = 1.0f;
			//data.myMaxBounds = CU::Vec3f(100.f, 100.f, 100.f);
			//data.myMinBounds = CU::Vec3f(0.f, 0.f, 0.f);
			data.myContrast = CU::Vec3f(1.f, 1.f, 1.f);
			data.myTint = CU::Vec3f(1.f, 1.f, 1.f);
			data.myBlackPoint = CU::Vec3f(0.f, 0.f, 0.f);

			data.myFogColor;
			data.myFogHighlightColor;
			data.myFogStartDistance;
			data.myFogDensity;
			data.myFogHeightFalloff;
		}


		size_t transformSize = json["GameObjects"].size();

		for (size_t i = 0; i < transformSize; i++)
		{
			GObject gameObject;
			gameObject.myTransform.myPosition.x = json["GameObjects"][i]["Transform"]["Position"]["x"];
			gameObject.myTransform.myPosition.y = json["GameObjects"][i]["Transform"]["Position"]["y"];
			gameObject.myTransform.myPosition.z = json["GameObjects"][i]["Transform"]["Position"]["z"];

			gameObject.myTransform.myRotation.x = json["GameObjects"][i]["Transform"]["Rotation"]["x"];
			gameObject.myTransform.myRotation.y = json["GameObjects"][i]["Transform"]["Rotation"]["y"];
			gameObject.myTransform.myRotation.z = json["GameObjects"][i]["Transform"]["Rotation"]["z"];
			gameObject.myTransform.myRotation.w = json["GameObjects"][i]["Transform"]["Rotation"]["w"];

			gameObject.myTransform.myScale.x = json["GameObjects"][i]["Transform"]["Scale"]["x"];
			gameObject.myTransform.myScale.y = json["GameObjects"][i]["Transform"]["Scale"]["y"];
			gameObject.myTransform.myScale.z = json["GameObjects"][i]["Transform"]["Scale"]["z"];

			size_t animations = json["GameObjects"][i]["ModelData"]["Animations"].size();
			for (size_t c = 0; c < animations; c++)
			{
				std::string aniPath = json["GameObjects"][i]["ModelData"]["Animations"][c];
				aniPath = "../" + aniPath;
				gameObject.myModelData.myAnimations.push_back(aniPath);
			}

			std::string tempPath = json["GameObjects"][i]["ModelData"]["MeshPath"];
			if (tempPath != "UnitCube")
			{
				tempPath = "../" + tempPath;
			}

			size_t textureDataSize = json["GameObjects"][i]["ModelData"]["TextureData"]["AlbedoTextures"].size();

			for (size_t tDataIndex = 0; tDataIndex < textureDataSize; tDataIndex++)
			{

				gameObject.myModelData.myTextureData.myAlbedoTextures.push_back(json["GameObjects"][i]["ModelData"]["TextureData"]["AlbedoTextures"][tDataIndex].get<std::string>());
				gameObject.myModelData.myTextureData.myNormalTextures.push_back(json["GameObjects"][i]["ModelData"]["TextureData"]["NormalTextures"][tDataIndex].get<std::string>());
				gameObject.myModelData.myTextureData.myMaterialTextures.push_back(json["GameObjects"][i]["ModelData"]["TextureData"]["MaterialTextures"][tDataIndex].get<std::string>());
			}

			gameObject.myModelData.myMeshPath = tempPath;
			gameObject.myTag = json["GameObjects"][i]["Tag"];
			gameObject.myName = json["GameObjects"][i]["Name"];

			if (json["GameObjects"][i].contains("Layer"))
			{
				gameObject.myLayer = json["GameObjects"][i]["Layer"];
			}

			if (json["GameObjects"][i].contains("LoDData"))
			{
				gameObject.myLoDData.myLoDModel = json["GameObjects"][i]["LoDData"]["LoDModel"].get<std::string>();


				size_t textureDataSize = json["GameObjects"][i]["LoDData"]["TextureData"]["AlbedoTextures"].size();

				for (size_t tDataIndex = 0; tDataIndex < textureDataSize; tDataIndex++)
				{

					gameObject.myLoDData.myTextureData.myAlbedoTextures.push_back(json["GameObjects"][i]["LoDData"]["TextureData"]["AlbedoTextures"][tDataIndex].get<std::string>());
					gameObject.myLoDData.myTextureData.myNormalTextures.push_back(json["GameObjects"][i]["LoDData"]["TextureData"]["NormalTextures"][tDataIndex].get<std::string>());
					gameObject.myLoDData.myTextureData.myMaterialTextures.push_back(json["GameObjects"][i]["LoDData"]["TextureData"]["MaterialTextures"][tDataIndex].get<std::string>());
				}
			}

			if (json["GameObjects"][i].contains("TriggerData") && gameObject.myTag == "EventTrigger")
			{
				gameObject.myTriggerData.myType = json["GameObjects"][i]["TriggerData"]["Type"].get<int>();
				gameObject.myTriggerData.myText = json["GameObjects"][i]["TriggerData"]["Text"].get<std::string>();
				gameObject.myTriggerData.myFont = json["GameObjects"][i]["TriggerData"]["Font"].get<std::string>();
				gameObject.myTriggerData.myRadius = json["GameObjects"][i]["TriggerData"]["Radius"].get<float>();
				gameObject.myTriggerData.myFollowTimer = json["GameObjects"][i]["TriggerData"]["FollowTimer"].get<float>();
				gameObject.myTriggerData.myWriteSpeed = json["GameObjects"][i]["TriggerData"]["WriteSpeed"].get<float>();
				gameObject.myTriggerData.myPageFlipDelay = json["GameObjects"][i]["TriggerData"]["PageFlipDelay"].get<float>();
				gameObject.myTriggerData.myPanTimer = json["GameObjects"][i]["TriggerData"]["PanTimer"].get<float>();
				gameObject.myTriggerData.myWaitTimer = json["GameObjects"][i]["TriggerData"]["WaitTimer"].get<float>();
				gameObject.myTriggerData.myDegreesToRotate = json["GameObjects"][i]["TriggerData"]["DegreesToRotate"].get<float>();
				gameObject.myTriggerData.myFontSize = json["GameObjects"][i]["TriggerData"]["FontSize"].get<int>();
				gameObject.myTriggerData.myPageSize = json["GameObjects"][i]["TriggerData"]["PageSize"].get<int>();
				gameObject.myTriggerData.mySpawnPoolSize = json["GameObjects"][i]["TriggerData"]["SpawnPoolSize"].get<int>();
				gameObject.myTriggerData.myOneUse = json["GameObjects"][i]["TriggerData"]["OneUse"].get<bool>();
				gameObject.myTriggerData.myWillFollow = json["GameObjects"][i]["TriggerData"]["WillFollow"].get<bool>();
				gameObject.myTriggerData.myUseAnimation = json["GameObjects"][i]["TriggerData"]["UseAnimation"].get<bool>();
				gameObject.myTriggerData.myUsePages = json["GameObjects"][i]["TriggerData"]["UsePages"].get<bool>();
				gameObject.myTriggerData.myUseCamera = json["GameObjects"][i]["TriggerData"]["UseCamera"].get<bool>();
				
				size_t pageSize = json["GameObjects"][i]["TriggerData"]["Pages"].size();

				for (size_t y = 0; y < pageSize; y++)
				{
					gameObject.myTriggerData.myPages.push_back(json["GameObjects"][i]["TriggerData"]["Pages"][y].get<std::string>());
				}

				size_t poolSize = json["GameObjects"][i]["TriggerData"]["SpawnPool"].size();

				for (size_t y = 0; y < poolSize; y++)
				{
					gameObject.myTriggerData.mySpawnPool.push_back
					({ 
							json["GameObjects"][i]["TriggerData"]["SpawnPool"][y]["x"].get<float>(),
							json["GameObjects"][i]["TriggerData"]["SpawnPool"][y]["y"].get<float>(),
							json["GameObjects"][i]["TriggerData"]["SpawnPool"][y]["z"].get<float>()
					});
				}
			}

			data.myGameObjects.push_back(gameObject);
		}

		return data;
	}

	void SceneBuilder::GetSceneData(std::string aPath, std::string aName, bool aSaveToBuild)
	{
		SceneData data;
		data.myName = aName;
		data.myPath = aPath;

		std::ifstream stream(aPath);

		nlohmann::json j;

		stream >> j;

		nlohmann::json json = j["SceneData"];
		CU::Vec3f test = CU::Vec3f(json["MaxBounds"]["x"].get<float>(), json["MaxBounds"]["y"].get<float>(), json["MaxBounds"]["z"].get<float>());
		test = CU::Vec3f(json["MinBounds"]["x"].get<float>(), json["MinBounds"]["y"].get<float>(), json["MinBounds"]["z"].get<float>());
		test = CU::Vec3f(json["Database"]["Blackpoint"]["r"].get<float>(), json["Database"]["Blackpoint"]["g"].get<float>(), json["Database"]["Blackpoint"]["b"].get<float>());
		test = CU::Vec3f(json["Database"]["Tint"]["r"].get<float>(), json["Database"]["Tint"]["g"].get<float>(), json["Database"]["Tint"]["b"].get<float>());
		test = data.myContrast = CU::Vec3f(json["Database"]["Contrast"]["r"].get<float>(), json["Database"]["Contrast"]["g"].get<float>(), json["Database"]["Contrast"]["b"].get<float>());
		data.myMaxBounds = CU::Vec3f(json["MaxBounds"]["x"].get<float>(), json["MaxBounds"]["y"].get<float>(), json["MaxBounds"]["z"].get<float>());
		data.myMinBounds = CU::Vec3f(json["MinBounds"]["x"].get<float>(), json["MinBounds"]["y"].get<float>(), json["MinBounds"]["z"].get<float>());
		data.myBlackPoint = CU::Vec3f(json["Database"]["Blackpoint"]["r"].get<float>(), json["Database"]["Blackpoint"]["g"].get<float>(), json["Database"]["Blackpoint"]["b"].get<float>());
		data.myTint = CU::Vec3f(json["Database"]["Tint"]["r"].get<float>(), json["Database"]["Tint"]["g"].get<float>(), json["Database"]["Tint"]["b"].get<float>());
		data.myContrast = CU::Vec3f(json["Database"]["Contrast"]["r"].get<float>(), json["Database"]["Contrast"]["g"].get<float>(), json["Database"]["Contrast"]["b"].get<float>());
		data.myBloomBlending = json["Database"]["BloomBlending"].get<float>();
		data.mySaturation = json["Database"]["Saturation"].get<float>();
		data.myExposure = json["Database"]["Exposure"].get<float>();

		size_t transformSize = json["GameObjects"].size();

		for (size_t i = 0; i < transformSize; i++)
		{
			GObject gameObject;
			gameObject.myTransform.myPosition.x = json["GameObjects"][i]["Transform"]["Position"]["x"];
			gameObject.myTransform.myPosition.y = json["GameObjects"][i]["Transform"]["Position"]["y"];
			gameObject.myTransform.myPosition.z = json["GameObjects"][i]["Transform"]["Position"]["z"];

			gameObject.myTransform.myRotation.x = json["GameObjects"][i]["Transform"]["Rotation"]["x"];
			gameObject.myTransform.myRotation.y = json["GameObjects"][i]["Transform"]["Rotation"]["y"];
			gameObject.myTransform.myRotation.z = json["GameObjects"][i]["Transform"]["Rotation"]["z"];
			gameObject.myTransform.myRotation.w = json["GameObjects"][i]["Transform"]["Rotation"]["w"];

			gameObject.myTransform.myScale.x = json["GameObjects"][i]["Transform"]["Scale"]["x"];
			gameObject.myTransform.myScale.y = json["GameObjects"][i]["Transform"]["Scale"]["y"];
			gameObject.myTransform.myScale.z = json["GameObjects"][i]["Transform"]["Scale"]["z"];

			size_t animations = json["GameObjects"][i]["ModelData"]["Animations"].size();
			for (size_t c = 0; c < animations; c++)
			{
				std::string aniPath = json["GameObjects"][i]["ModelData"]["Animations"][c];
				aniPath = "../" + aniPath;
				gameObject.myModelData.myAnimations.push_back(aniPath);
			}

			std::string tempPath = json["GameObjects"][i]["ModelData"]["MeshPath"];
			if (tempPath != "UnitCube")
			{
				tempPath = "../" + tempPath;
			}

			gameObject.myModelData.myMeshPath = tempPath;
			gameObject.myTag = json["GameObjects"][i]["Tag"];
			gameObject.myName = json["GameObjects"][i]["Name"];

			data.myGameObjects.push_back(gameObject);
		}

		if (aSaveToBuild)
		{
			data.myId = myBuildScenes.size();
			myBuildScenes.push_back(data);
		}
		else
		{
			data.myId = myTestScenes.size();
			myTestScenes.push_back(data);
		}


	}

	void SceneBuilder::LoadSceneObjectData(std::string aLoadFromJson)
	{

		//if we wanted a separation between unity object data and engine object data we could substring to the last dot and add on another format
		//assuming that the 2 files are in the same folder

		std::ifstream stream(aLoadFromJson);

		if (!stream.good())
		{
			stream.close();
			return;
		}

		nlohmann::json loadedaData;

		stream >> loadedaData;

		if (!loadedaData.contains("EngineSceneData"))
			return;

		//Scene& scene = mySceneHandler->GetActiveScene();

		nlohmann::json json = loadedaData["EngineSceneData"];

		size_t entityCount = json.size();

		for (size_t i = 0; i < entityCount; i++)
		{
			nlohmann::json entityJson = json[i];

			CreateFromPrefabJson(entityJson);

		}

	}

	void SceneBuilder::LoadSceneObjectData(nlohmann::json& aJson)
	{
		size_t entityCount = aJson.size();

		for (size_t i = 0; i < entityCount; i++)
		{
			nlohmann::json entityJson = aJson[i];

			CreateFromPrefabJson(entityJson);

		}
	}

	EntityID SceneBuilder::CreateFromPrefabJson(nlohmann::json& aJson)
	{
		Scene& scene = mySceneHandler->GetActiveScene();

		EntityID id = scene.NewEntity();
		Entity& entity = scene.myEntities[GetEntityIndex(id)];

		entity.myName = aJson["Name"];

		if (aJson.contains("Tag"))
			entity.myTag = aJson["Tag"];

		if (aJson.contains("Layer"))
			entity.myLayer = aJson["Layer"];

		if (aJson.contains("PrefabConnection"))
			entity.myPrefabConnection = aJson["PrefabConnection"];
		 
		entity.myIsEditorCreated = true;

		//ugly

		int counter = 0;
		for (int i = 0; i < scene.myEntities.size(); i++)
		{
			std::string oName = scene.myEntities[i].myName;
			oName = oName.substr(0, entity.myName.size());
			if (entity.myName == oName && GetEntityIndex(id) != (EntityIndex)i)
			{
				counter++;
			}

		}

		if (counter > 0)
		{
			entity.myName += ("(" + std::to_string(counter) + ")");
		}

		//

		size_t cListSize = aJson["ComponentList"].size();

		for (size_t i = 0; i < cListSize; i++)
		{
			std::string name = aJson["ComponentList"][i]["Name"];
			
			ASSERT_WITH_MSG(myCRAWs.count(name), (((L"The CRAW for " + std::wstring(name.begin(), name.end())) + L" has not been registered").c_str()));
			//assert(myCRAWs.count(name), "The ReadAndWrite for this component has not been registered");

			int cmpId = mySceneHandler->myComponentIds[name];

			void* cmp = nullptr;

			if (mySceneHandler->myFBM.test(cmpId))
				cmp = scene.AssignBehaviour(cmpId, id);
			else
				cmp = scene.AssignComponent(cmpId, id);

			//myCRAWs[name]->Assign(scene, GetEntityIndex(id));
			myCRAWs[name]->Read(cmp, id, aJson["ComponentList"][i]["Data"]);
		}


		if (aJson.contains("Children"))
		{
			size_t childrenCount = aJson["Children"].size();

			for (size_t i = 0; i < childrenCount; i++)
			{
				nlohmann::json childJson = aJson["Children"][i];

				EntityID child = CreateFromPrefabJson(childJson);

				scene.SetParent(child, id, false);

			}

		}

		return id;
	}

	EntityID SceneBuilder::CreateFromPrefab(std::string aPath)
	{
		std::ifstream stream(aPath);

		nlohmann::json json;
		stream >> json;

		stream.close();

		return CreateFromPrefabJson(json);

	}

	void SceneBuilder::SaveSceneObjectData(std::vector<EntityID> someEntities)
	{

		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		nlohmann::json json;

		{
			std::ifstream readStream(mySceneHandler->GetActiveScene().myPath);

			readStream >> json;

			readStream.close();
		}

		nlohmann::json entityDataJson;

		int total = 0;
		for (auto& id : someEntities)
		{
			if (!scene.IsEntityIDValid(id))
				continue;

			//a bit of an ugly safety check
			std::vector<int> componentIds;
			std::vector<std::string> components;

			Entity& entity = scene.myEntities[GetEntityIndex(id)];

			if (entity.myParent != INVALID_ENTITY)
			{
				continue;
			}

			//registercomponents guarantess that i is a registered component
			bool failed = false;

			{
				for (int i = 0; i < mySceneHandler->myRegisteredComponents.size(); i++)
				{
					if (entity.myMask.test(i))
					{
						std::string cmpString = mySceneHandler->myComponentNames[i];

						if (myCRAWs.count(cmpString) == 0)
						{
							std::cout << "CRAW for <" << cmpString << "> could not be found" << std::endl;
							failed = true;
						}

						components.push_back(cmpString);
						componentIds.push_back(i);
					}
				}

				if (failed)
				{
					std::cout << "This Entity wont be saved! \n";
					continue;
				}
			}

			entityDataJson[total] = CreatePrefabJson(entity);

			total++;
		}

		json["EngineSceneData"] = entityDataJson;


		//should substring to get the .owo path
		std::ofstream write(scene.myPath);
		write << json;
		write.close();
	}

	void SceneBuilder::SaveSceneData()
	{
		Scene* scene = mySceneHandler->myActiveScene;

		assert(scene != nullptr && "Tried to write scene data but no scene was active");

		nlohmann::json json;
		//should substring to get the .owo file instead

		std::ifstream read(scene->myPath);
		read >> json;
		read.close();

		CU::Database<true>& database = scene->myDatabase;

		//PPEffects
		json["Database"]["Test"] = database.Get<int>("Test");
		json["Database"]["Test2"] = database.Get<std::string>("Test2");
		json["Database"]["BloomBlending"] = database.Get<float>("BloomBlending");
		json["Database"]["Saturation"] = database.Get<float>("Saturation");
		json["Database"]["Exposure"] = database.Get<float>("Exposure");
		json["Database"]["Contrast"]["r"] = database.Get<CU::Vec3f>("Contrast").x;
		json["Database"]["Contrast"]["g"] = database.Get<CU::Vec3f>("Contrast").y;
		json["Database"]["Contrast"]["b"] = database.Get<CU::Vec3f>("Contrast").z;
		json["Database"]["Tint"]["r"] = database.Get<CU::Vec3f>("Tint").x;
		json["Database"]["Tint"]["g"] = database.Get<CU::Vec3f>("Tint").y;
		json["Database"]["Tint"]["b"] = database.Get<CU::Vec3f>("Tint").z;
		json["Database"]["Blackpoint"]["r"] = database.Get<CU::Vec3f>("Blackpoint").x;
		json["Database"]["Blackpoint"]["g"] = database.Get<CU::Vec3f>("Blackpoint").y;
		json["Database"]["Blackpoint"]["b"] = database.Get<CU::Vec3f>("Blackpoint").z;
		//

		//Fog
		json["Database"]["FogColor"]["r"] = database.Get<CU::Vec3f>("FogColor").x;
		json["Database"]["FogColor"]["g"] = database.Get<CU::Vec3f>("FogColor").y;
		json["Database"]["FogColor"]["b"] = database.Get<CU::Vec3f>("FogColor").z;
		json["Database"]["FogHighlightColor"]["r"] = database.Get<CU::Vec3f>("FogHighlightColor").x;
		json["Database"]["FogHighlightColor"]["g"] = database.Get<CU::Vec3f>("FogHighlightColor").y;
		json["Database"]["FogHighlightColor"]["b"] = database.Get<CU::Vec3f>("FogHighlightColor").z;
		json["Database"]["FogStartDist"] = database.Get<float>("FogStartDist");
		json["Database"]["FogGlobalDensity"] = database.Get<float>("FogGlobalDensity");
		json["Database"]["FogHeightFalloff"] = database.Get<float>("FogHeightFalloff");
		//

		std::ofstream write(scene->myPath);
		write << json;
		write.close();



	}

	void SceneBuilder::WriteLightData(const LightData&)
	{
		Scene* scene = mySceneHandler->myActiveScene;

		assert(scene != nullptr && "Tried to write light data but no scene was active");

		nlohmann::json json;

		//read json
		std::ifstream read(scene->myPath);
		read >> json;
		read.close();

		//change json
		json["LightData"]["Test"] = "EngineTest";

		//write json
		std::ofstream write(scene->myPath);
		write << json;
		write.close();

	}

	nlohmann::json SceneBuilder::CreatePrefabJson(const Entity& aEntity)
	{
		nlohmann::json json;

		{
			json["Name"] = aEntity.myName;
			json["Tag"] = aEntity.myTag;
			json["Layer"] = aEntity.myLayer;
			json["PrefabConnection"] = aEntity.myPrefabConnection;

			nlohmann::json componentList;

			int pos = 0;
			for (int i = 0; i < aEntity.myMask.size(); i++)
			{
				//assert(mySceneHandler->myComponentNames.count(i), "The Component could not be found");

				if (aEntity.myMask.test(i))
				{
					assert(myCRAWs.count(mySceneHandler->myComponentNames[i]), "The Component did not have a registered ReadAndWrite");
					const char* name = mySceneHandler->myComponentNames[i].c_str();

					componentList[pos]["Name"] = name;
					void* comp = mySceneHandler->GetActiveScene().GetComponent(i, aEntity.myId);
					componentList[pos]["Data"] = myCRAWs[name]->Write(comp, aEntity.myId);
					pos++;
				}
			}

			json["ComponentList"] = componentList;
		}



		if (aEntity.myChildren.size() != 0)
		{
			nlohmann::json children;

			for (int i = 0; i < aEntity.myChildren.size(); i++)
			{
				Entity child = Context::Get()->mySceneHandler->GetActiveScene().GetEntity(aEntity.myChildren[i]);


				children[i] = CreatePrefabJson(child);
			}

			json["Children"] = children;
		}


		return json;
	}

	void SceneBuilder::SaveAsPrefab(const Entity& aEntity)
	{
		std::string savePath = "../Assets\\Prefabs\\" + aEntity.myName + ".monster";
		mySceneHandler->GetActiveScene().myEntities[GetEntityIndex(aEntity.myId)].myPrefabConnection = savePath;
		nlohmann::json json = CreatePrefabJson(aEntity);

		std::ofstream stream(savePath);

		stream << json;

		stream.close();

	}

	void SceneBuilder::SaveAsPrefab(const Entity& aEntity, std::string aPath)
	{

		nlohmann::json json = CreatePrefabJson(aEntity);

		std::ofstream stream(aPath);

		stream << json;

		stream.close();

	}

	Scene* SceneBuilder::LoadScene(size_t aIndex)
	{
		return LoadScene(myBuildScenes[aIndex]);
	}

	Scene* SceneBuilder::LoadScene(std::string aString)
	{

		if (myMappedBuldScenes.count(aString) != 0)
		{
			return LoadScene(myBuildScenes[myMappedBuldScenes[aString]]);
		}
		if (myMappedTestScenes.count(aString) != 0)
		{
			return LoadScene(myTestScenes[myMappedTestScenes[aString]]);
		}

	}

	void SceneBuilder::AssigneMaterials(EntityID aId, Scene* aScene, ModelID aModelID, TextureData& someTextureData)
	{
		MaterialComponent* matComp = aScene->AssignComponent<MaterialComponent>(aId);

		Model* model = Context::Get()->myModelManager->GetModel(aModelID);

		memcpy(matComp->myTextures, model->myTextures, sizeof(TextureID) * MAX_MESH_COUNT * MAX_MATERIAL_COUNT);

		TextureManager::TextureLoadResult tResult;

		size_t tDataSize = someTextureData.myAlbedoTextures.size();

		for (size_t tIndex = 0; tIndex < tDataSize; tIndex++)
		{
			std::string albedo = someTextureData.myAlbedoTextures[tIndex];
			if (albedo.size() != 0)
			{
				std::wstring alb(albedo.begin(), albedo.end());
				tResult = Context::Get()->myTextureManager->LoadTexture(alb, true);
				if (tResult.mySuccesFlag)
				{
					matComp->myTextures[tIndex][ALBEDO_MAP] = tResult.myValue;
				}
			}

			std::string normal = someTextureData.myNormalTextures[tIndex];
			if (normal.size() != 0)
			{
				std::wstring norm(normal.begin(), normal.end());
				tResult = Context::Get()->myTextureManager->LoadTexture(norm, false);
				if (tResult.mySuccesFlag)
				{
					matComp->myTextures[tIndex][NORMAL_MAP] = tResult.myValue;
				}
			}

			std::string material = someTextureData.myMaterialTextures[tIndex];
			if (material.size() != 0)
			{
				std::wstring mat(material.begin(), material.end());
				tResult = Context::Get()->myTextureManager->LoadTexture(mat, false);
				if (tResult.mySuccesFlag)
				{
					matComp->myTextures[tIndex][MATERIAL_MAP] = tResult.myValue;
				}
			}
		}
	}

	Scene* SceneBuilder::LoadScene(SceneData& someData)
	{ 

		Context::Get()->myTimeHandler->RemoveAllTimers();
		Context::Get()->mySprite3DParticleHandler->KillAllEmitters();

		Scene* scene = new Scene();

		scene->myId = someData.myId;
		scene->myPath = someData.myPath;
		scene->myName = someData.myName;
		scene->myDatabase.SetValue<CU::Vec3f>("MaxBounds", someData.myMaxBounds);
		scene->myDatabase.SetValue<CU::Vec3f>("MinBounds", someData.myMinBounds);
		scene->myDatabase.SetValue<float>("BloomBlending", someData.myBloomBlending);
		scene->myDatabase.SetValue<float>("Saturation", someData.mySaturation);
		scene->myDatabase.SetValue<float>("Exposure", someData.myExposure);
		scene->myDatabase.SetValue<CU::Vec3f>("Blackpoint", someData.myBlackPoint);
		scene->myDatabase.SetValue<CU::Vec3f>("Tint", someData.myTint);
		scene->myDatabase.SetValue<CU::Vec3f>("Contrast", someData.myContrast);


		scene->myDatabase.SetValue<CU::Vec3f>("FogColor", someData.myFogColor);
		scene->myDatabase.SetValue<float>("FogStartDist", someData.myFogStartDistance);
		scene->myDatabase.SetValue<CU::Vec3f>("FogHighlightColor", someData.myFogHighlightColor);
		scene->myDatabase.SetValue<float>("FogGlobalDensity", someData.myFogDensity);
		scene->myDatabase.SetValue<float>("FogHeightFalloff", someData.myFogHeightFalloff);

		//temp
		mySceneHandler->myActiveScene = scene;
		Context::Get()->mySystemManager->ClearLists();
		//

		//testing
		scene->myDatabase.SetValue("Test", 10);
		scene->myDatabase.SetValue<std::string>("Test2", "stringTing");
		//

		scene->mySceneHandler = mySceneHandler;

		assert(someData.myGameObjects.size() <= MAX_ENTITIES && "Max entity limit reached");

		//SetUpScene

		//for now we'll just create a pool for every registered component 
		ComponentMask combinedMask = mySceneHandler->myFBM | mySceneHandler->myFCM;

		for (size_t i = 0; i < combinedMask.size(); i++)
		{
			if (combinedMask.test(i))
			{
				scene->CreateComponentPool((ComponentID)i, mySceneHandler->myPoolSizes[(ComponentID)i]);
			}
		}

		Context::Get()->myModelManager->LoadUnitCube();

		CU::Worker<int> worker;

		worker.AddWroker([this, &someData, &scene]()
			{
				for (auto& data : someData.myGameObjects)
				{
					EntityID id = scene->NewEntity();

					scene->SetEntityName(id, data.myName);
					scene->SetLayer(id, data.myLayer);

					TransformComponent* t = scene->AssignComponent<TransformComponent>(id);

					t->myPosition = data.myTransform.myPosition;
					t->myQuaternion = data.myTransform.myRotation;
					t->myScale = data.myTransform.myScale;

					ModelManager::ModelLoadResult mResult = Context::Get()->myModelManager->LoadModel(data.myModelData.myMeshPath);

					//ModelComponent* m = scene->AssignComponent<ModelComponent>(id);

					//Context::Get()->myTextureManager->LoadTexture()
					if (mResult.mySuccesFlag)
					{
						ModelComponent* m = scene->AssignComponent<ModelComponent>(id);
						m->myModelId = mResult.myValue;

						AssigneMaterials(id, scene, mResult.myValue, data.myModelData.myTextureData);

						//for now this will be how it works
						if (data.myLoDData.myLoDModel != "")
						{
							ModelManager::ModelLoadResult lodResult = Context::Get()->myModelManager->LoadModel(data.myLoDData.myLoDModel);

							if (lodResult.mySuccesFlag)
							{
								EntityID lodedEntity = scene->NewEntity();

								scene->SetEntityName(lodedEntity, data.myName + "_LOD");

								TransformComponent* lodedTransform = scene->AssignComponent<TransformComponent>(lodedEntity);

								lodedTransform->myPosition = data.myTransform.myPosition;
								lodedTransform->myQuaternion = data.myTransform.myRotation;
								lodedTransform->myScale = data.myTransform.myScale;

								ModelComponent* lodedModel = scene->AssignComponent<ModelComponent>(lodedEntity);
								lodedModel->myModelId = lodResult.myValue;

								AssigneMaterials(lodedEntity, scene, lodResult.myValue, data.myLoDData.myTextureData);

								LoDComponent* lodComp = scene->AssignComponent<LoDComponent>(id);
								//ignoring lod levels since we messure against player
								lodComp->myLoDEntities[0] = id;
								lodComp->myLoDEntities[1] = lodedEntity;

								scene->DeactivateEntity(lodedEntity);
								//lodComp->myLoDModel[0] = mResult.myValue;
								//lodComp->myLoDModel[1] = lodResult.myValue;


							}
						}
						//

						for (int i = 0; i < data.myModelData.myAnimations.size(); i++)
						{
							Context::Get()->myAnimationManager->LoadAnimation(id, data.myModelData.myAnimations[i]);
						}

					}

					if (myObjectBuilders.count(data.myTag))
					{
						myObjectBuilders[data.myTag]->Create(id, data, *scene);
					}


				}
				return 0;
			});

		std::string path = "../Assets/NavMesh/" + someData.myName + "NavMesh.obj"; // Mesh path for now.

		worker.AddWroker([this, &path, &scene, miniX = someData.myMinBounds.x, miniZ = someData.myMinBounds.z]()
			{
				NavMesh::LoadNavMesh(path, scene->myNavMesh);

				scene->myNavMeshGrid.Init(CU::Vec2f(miniX, miniZ), 50, 50, 10);
				for (auto& tri : scene->myNavMesh.triangles)
				{
					scene->myNavMeshGrid.AddTriangle(&tri);
				}

				return 0;
			});

		worker.Run();

		{
			//scene->myNavMeshGrid.Init(CU::Vec2f(someData.myMinBounds.x, someData.myMinBounds.z), 50, 50, 10);
			//for (auto& tri : scene->myNavMesh.triangles)
			//{
			//	scene->myNavMeshGrid.AddTriangle(&tri);
			//}
		}

		//if running two files replace extension with .owo to get the engine data path

		//the second one is faster but the first one allows engine data to be dynamiclly reloaded
		LoadSceneObjectData(scene->myPath);
		//LoadSceneObjectData(someData.myEngineLoadData);
		
		for(auto& ents : scene->myEntities)
			Context::Get()->myEventHandler->InvokeEvent(EventType::EntityCreated, ents.myId);

		Context::Get()->myEventHandler->InvokeEvent(EventType::SceneLoaded, 0);

		// VFX
		Context::Get()->mySystemManager->GetSystem<EmitterSystem>()->SpawnEmitters();
		{
			//VFXEmitter* smokeEmitter = new VFXEmitter;
			//smokeEmitter->Init(INVALID_ENTITY);
			//smokeEmitter->SetDuration(999999);
			//smokeEmitter->SetPosition(CU::Vec3f()); // for the future
			//EntityID id = Context::Get()->myVFXHandler->AddVFX(smokeEmitter);
			//Context::Get()->myVFXHandler->Play(id);
		}

		return scene;
	}
}