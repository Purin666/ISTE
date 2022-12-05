#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "BuilderDataStructs.h"
#include "ObjectBuilder.h"
#include "ISTE/Graphics/RenderDefines.h"
#include "ComponentReadAndWrites/ComponentReadAndWrite.h"

namespace ISTE
{

	class Scene;
	class SceneHandler;
	//class NavMeshImporter; // Legacy

	struct Entity;

	class SceneBuilder
	{
	public:
		~SceneBuilder();
		void Init();
		Scene* LoadScene(size_t aIndex);
		Scene* LoadScene(std::string aString);

		template<class T>
		void RegisterObjectBuilder(std::string aTag)
		{
			if (!std::is_base_of<ObjectBuilder, T>())
				return;

			myObjectBuilders.insert({ aTag, new T() });
		}

		template<class T, class To>
		void RegisterComponentReadAndWrite()
		{
			if (!std::is_base_of<ComponentReadAndWrite, T>())
				return;

			myCRAWs.insert({ typeid(To).name(), new T() });
		}

	private:
		friend class SceneHandler;
		friend class BaseEditor;
		friend class AssetsManager;

		//Load
		Scene* LoadScene(SceneData& someData);
		void GetSceneData(std::string aPath, std::string aName, bool saveToBuild = true);
		SceneData GetSceneDataT(std::string aPath, std::string aName, bool aSaveToBuild);
		void LoadSceneObjectData(std::string aLoadFromJson);
		void LoadSceneObjectData(nlohmann::json& aJson);
		EntityID CreateFromPrefabJson(nlohmann::json& aJson);
		EntityID CreateFromPrefab(std::string aPath);
		void AssigneMaterials(EntityID aId, Scene* aScene,ModelID aModelId, TextureData& aGObject);
		//

		//Writing
		void SaveSceneObjectData(std::vector<EntityID> someEntities);
		void SaveSceneData();
		void WriteLightData(const LightData& someData);
		nlohmann::json CreatePrefabJson(const Entity& aEntity);
		void SaveAsPrefab(const Entity& aEntity);
		void SceneBuilder::SaveAsPrefab(const Entity& aEntity, std::string aPath);
		//
		

		SceneHandler* mySceneHandler = nullptr;
		//NavMeshImporter* myNavMeshImporter = nullptr; // Legacy

		std::unordered_map<std::string, ObjectBuilder*> myObjectBuilders;
		std::unordered_map<std::string, ComponentReadAndWrite*> myCRAWs;

		std::unordered_map<std::string, int> myMappedBuldScenes;
		std::unordered_map<std::string, int> myMappedTestScenes;

		std::vector<SceneData> myBuildScenes;
		std::vector<SceneData> myTestScenes;

		int myLoadedSceneIndex = -1;
		bool myLoadedTestScene = false;

		//test
		std::mutex testLock;

	};
}