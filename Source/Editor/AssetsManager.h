#pragma once

#include "ISTE/CU/FileWatcher.h"

#include <DirectXTex/WICTextureLoader/WICTextureLoader11.h>
#include <DirectXTex/DDSTextureLoader/DDSTextureLoader11.h>

//prefab editing
#include "Components/AbstractComponentEdit.h"
#include "Json/json.hpp"
//

namespace ISTE
{
	enum class DrawMode
	{
		Texture,
		FBX,
		count
	};

	struct Content
	{
		ID3D11ShaderResourceView* SRV = nullptr;
		bool myDontRelease = false;
		std::string myFullPath;
		std::string myName;
		DrawMode myDrawMode = DrawMode::count;
		
	};

	struct Directory
	{
		std::string myFullPath = "";
		std::string myParentPath = "";
		std::string myName;
		std::vector<Content> myContent;
		std::vector<Directory> myDirectories;
	};

	class BaseEditor;

	class AssetsManager
	{
	public:
		AssetsManager();
		~AssetsManager();
		void Draw(bool* anOpen);
		bool LoadedScene() { return myLoadedScene; }

		void LookUp(std::string aPath);

	private:
		void UpdateHierarchy(std::vector<std::string> someDirectoryContent, std::vector<std::string> someDirectories);
		void FolderChange();
		void FolderContentChange(std::vector<std::string> someContent);

		CU::FileWatcher<AssetsManager> myFileWatcher;

		void CleanUpContent(Directory& aDir);

		Directory* GetDirectoryInHierarchy(Directory& aDir, std::string aDirName);

		void PrintDir(Directory& aDir, int aLayer);

		Directory myRootDirectory;

		Directory* mySelectedDir = nullptr;

		float myAssetTreeSizeX = 0.25f;

		void DrawAssetsHierarchy();
		void DrawChildTree(Directory& aDir);
		void DrawAssetsViewer();
		void CheckSpecialCase(Content content);

		ID3D11ShaderResourceView* myFolderSRV = nullptr;
		ID3D11ShaderResourceView* myAnimationSRV = nullptr;
		ID3D11ShaderResourceView* mySceneSRV = nullptr;
		ID3D11ShaderResourceView* myMusicSRV = nullptr;
		ID3D11ShaderResourceView* myTXTSRV = nullptr;
		ID3D11ShaderResourceView* myFCKOFFSRV = nullptr;

		int myUpdateDelay = 20;
		int myUpdateCounter = 0;

		float myDCTimer = 0.25f;
		float myDCDelay = 0.25f;
		bool myDoubleClicked = false;
		bool myCheckForClick = false;

		bool myLoadedScene = false;

		//vars
		float imgSize = 64;
		//

		//kill me
		bool myScrollSet = false;
		//

		//higlighted
		std::string myHiglightedContent = "";
		float myHighLightTimer = 0;
		float myHighLightTime = 1.f;
		bool myHighLightContent = false;
		//

		std::string myHoveredItem = "";

		//Prefab Editing
		//ugly copy of inspector
		std::vector<AbstractComponentEdit*> myComponentsToEdit;
		void EditPrefab();
		void DrawChildren(std::vector<EntityID> someIDs, Scene& aScene);
		void DrawInspector(Scene& aScene);
		void UpdateEntities(nlohmann::json& aJson, const Entity& aRefEntity, Scene& aScene);
		void UpdateEntity(nlohmann::json& aJson, Entity& aEntity, Scene& aScene, ComponentMask& aIgnoreMask);
		EntityID myPrefabTarget = INVALID_ENTITY;
		EntityID mySelectedEntity = INVALID_ENTITY;
		std::string myPrefabPathTarget;

		bool myDisplayComponents = false;
		bool myDisplayRemoveComponents = false;

		std::string myTmpEntityName = "";
		std::string myTmpEntityTag = "";
		std::string myTmpEntityLayer = "";
		std::string myTmpComponentSearch = "";
		//

	};
}