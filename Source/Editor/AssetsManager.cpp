#include "AssetsManager.h"

#include "imgui/imgui.h"

#include <iostream>

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Builder/SceneBuilder.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/CU/Worker.h"

#include "imgui/misc/cpp/imgui_stdlib.h"

#include <algorithm>
#include <map>
#include <fstream>
#include <CommCtrl.h>

#include "ISTE/CU/MemTrack.hpp"

//PrefabEdit
#include "Components/TransformEdit.h"
#include "Components/SpriteEdit.h"
#include "Components/ModelEdit.h"
#include "Components/SpotLightEdit.h"
#include "Components/PointLightEdit.h"
#include "Components/DirectionalLightEdit.h"
#include "Components/AmbientLightEdit.h"
#include "Components/AnimatorComponentEdit.h"
#include "Components/DecalEdit.h"
#include "Components/CustomShaderEdit.h"
#include "Components/AudioSourceBehaviourEdit.h"
#include "Components/EmitterComponentEdit.h"
#include "Components/MaterialEdit.h"
//

#include "ISTE/Events/EventHandler.h"
#include "EditorContext.h"
#include "BaseEditor.h"

namespace ISTE
{
#pragma region ConNDes
	AssetsManager::AssetsManager()
	{

		myFileWatcher.AddDirectoryPath("../Assets");
		myFileWatcher.SetCallbackFunc(this, &AssetsManager::UpdateHierarchy);

		myFileWatcher.SetFolderChangedCallbackFunc(&AssetsManager::FolderChange);
		myFileWatcher.SetFolderCallbackFunc(&AssetsManager::FolderContentChange);

		//myFileWatcher.AddExtensionCheck(".dds");
		//myFileWatcher.AddExtensionCheck(".fbx");

		mySelectedDir = &myRootDirectory;
		myRootDirectory.myName = "Assets";
		myRootDirectory.myFullPath = "../Assets";

		DirectX::CreateDDSTextureFromFile(Context::Get()->myDX11->GetDevice(), L"../EngineAssets/Images/Folder.dds", nullptr, &myFolderSRV);
		DirectX::CreateDDSTextureFromFile(Context::Get()->myDX11->GetDevice(), L"../EngineAssets/Images/Animation.dds", nullptr, &myAnimationSRV);
		DirectX::CreateDDSTextureFromFile(Context::Get()->myDX11->GetDevice(), L"../EngineAssets/Images/Scene.dds", nullptr, &mySceneSRV);
		DirectX::CreateDDSTextureFromFile(Context::Get()->myDX11->GetDevice(), L"../EngineAssets/Images/MousicIMG.dds", nullptr, &myMusicSRV);
		DirectX::CreateDDSTextureFromFile(Context::Get()->myDX11->GetDevice(), L"../EngineAssets/Images/TXTIMG.dds", nullptr, &myTXTSRV);
		DirectX::CreateDDSTextureFromFile(Context::Get()->myDX11->GetDevice(), L"../EngineAssets/Images/PaintIMG.dds", nullptr, &myFCKOFFSRV);

		//PrefabEdit
		myComponentsToEdit.push_back(new TransformEdit());
		myComponentsToEdit.push_back(new SpriteEdit());
		myComponentsToEdit.push_back(new ModelEdit());
		myComponentsToEdit.push_back(new SpotLightEdit());
		myComponentsToEdit.push_back(new PointLightEdit());
		myComponentsToEdit.push_back(new DirectionalLightEdit());
		myComponentsToEdit.push_back(new AmbientLightEdit());
		myComponentsToEdit.push_back(new AnimatorComponentEdit());
		myComponentsToEdit.push_back(new CustomShaderEdit());
		myComponentsToEdit.push_back(new AudioSourceBehaviourEdit());
		myComponentsToEdit.push_back(new EmitterComponentEdit());
		myComponentsToEdit.push_back(new DecalEdit());
		myComponentsToEdit.push_back(new MaterialEdit());
		//
		
		Context::Get()->myEventHandler->RegisterCallback(EventType::SceneLoaded, "AssetManager", [this](EntityID) {myPrefabTarget = INVALID_ENTITY; myPrefabPathTarget = "", mySelectedEntity = INVALID_ENTITY; });

	}
	AssetsManager::~AssetsManager()
	{
		if(myFolderSRV != nullptr)
			myFolderSRV->Release();
		if (myAnimationSRV != nullptr)
			myAnimationSRV->Release();
		if (mySceneSRV != nullptr)
			mySceneSRV->Release();
		if (myMusicSRV != nullptr)
			myMusicSRV->Release();
		if (myTXTSRV != nullptr)
			myTXTSRV->Release();
		if (myFCKOFFSRV != nullptr)
			myFCKOFFSRV->Release();


		for (auto& content : myRootDirectory.myContent)
		{
			if (content.SRV != nullptr && !content.myDontRelease)
				content.SRV->Release();

		}

		for (auto& edit : myComponentsToEdit)
		{
			delete edit;
		}

		//Context::Get()->myEventHandler->RemoveCallback(EventType::SceneLoaded, "AssetManager");
		CleanUpContent(myRootDirectory);
	}
#pragma endregion

	void AssetsManager::Draw(bool* anOpen)
	{

		if (*anOpen == false)
			return;

		if (myHighLightContent)
		{
			myHighLightTimer += Context::Get()->myTimeHandler->GetDeltaTime();
			if (myHighLightTimer >= myHighLightTime)
			{
				myHighLightContent = false;
				myHighLightTimer = 0;
			}
		}

		ImGui::Begin("Assets", anOpen);

		EditPrefab();

		myLoadedScene = false;

		myUpdateCounter++;
		if (myUpdateCounter == myUpdateDelay)
		{
			//myFileWatcher.CheckForChanges();
			try
			{
				std::vector<std::string> contents;
				std::vector<std::string> folders;

				for (auto& folder : mySelectedDir->myDirectories)
				{
					folders.push_back(folder.myFullPath);
				}

				for (auto& content : mySelectedDir->myContent)
				{
					contents.push_back(content.myFullPath);
				}

				myFileWatcher.CheckFolder(mySelectedDir->myFullPath, folders, contents);
			}
			catch (...)
			{

			}

			myUpdateCounter = 0;
		}

		if (ImGui::BeginDragDropTarget())
		{
			//should probably be moved to a seperate function
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityID", 0))
			{
				EntityID id = *(EntityID*)payload->Data;
				Context::Get()->mySceneHandler->myBuilder->SaveAsPrefab(Context::Get()->mySceneHandler->GetActiveScene().GetEntity(id));

			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_::ImGuiFocusedFlags_ChildWindows) && mySelectedDir != nullptr && Context::Get()->myInputHandler->IsKeyDown(VK_BACK))
		{
			if (mySelectedDir->myParentPath == myRootDirectory.myFullPath)
				mySelectedDir = &myRootDirectory;
			else
			{
				Directory* dir = GetDirectoryInHierarchy(myRootDirectory, mySelectedDir->myParentPath);
				if (dir != nullptr)
				{
					mySelectedDir = dir;
				}
			}
		}

		myDoubleClicked = false;

		if (myCheckForClick)
		{
			myDCTimer -= Context::Get()->myTimeHandler->GetDeltaTime();
			if (myDCTimer <= 0)
			{
				myCheckForClick = false;
				myDCTimer = myDCDelay;
			}
		}

		if (Context::Get()->myInputHandler->IsKeyDown(MK_LBUTTON))
		{
			if (myCheckForClick)
			{
				myDoubleClicked = true;
				myCheckForClick = false;
				myDCTimer = myDCDelay;
			}
			else
			{
				myCheckForClick = true;
			}
		}


		DrawAssetsHierarchy();
		DrawAssetsViewer();

		ImGui::End();
	}

	void AssetsManager::LookUp(std::string aPath)
	{
		size_t lastSlash;// = aPath.find_last_of('/');

		lastSlash = aPath.find_last_of('\\');

		if (lastSlash >= 10000)
		{
			lastSlash = aPath.find_last_of('/');

			if (lastSlash >= 10000)
				return;

		}

		std::string subbed = aPath.substr(0, lastSlash);

		Directory* dir = GetDirectoryInHierarchy(myRootDirectory, subbed);

		if (dir == nullptr)
			return;

		for (auto& content : dir->myContent)
		{
			if (content.myFullPath == aPath)
			{
				mySelectedDir = dir;
				myHiglightedContent = content.myFullPath;
				myHighLightContent = true;
				myHighLightTimer = 0;
				myScrollSet = false;
			}
		}



	}

	void AssetsManager::UpdateHierarchy(std::vector<std::string> someDirectoryContent, std::vector<std::string> someDirectories)
	{
		/*recalculate folder structure
		for starter we do a complete recalculation however should only update what is nessecary*/

		for (auto& content : myRootDirectory.myContent)
		{
			if (content.SRV != nullptr)
				content.SRV->Release();

		}

		CleanUpContent(myRootDirectory);

		myRootDirectory.myContent.clear();
		myRootDirectory.myDirectories.clear();

		//Setup directory tree

		myRootDirectory.myName = "Assets";
		myRootDirectory.myFullPath = "../Assets";

		size_t slash = 0;
		size_t secondSlash = 0;

		for (int i = 1; i < someDirectories.size(); i++)
		{
			std::string dirStr = someDirectories[i];

			slash = dirStr.find_last_of("\\");
			std::string root = dirStr.substr(0, slash);

			Directory dir;
			dir.myName = dirStr.substr(slash + 1);
			dir.myFullPath = dirStr;

			if (root == myRootDirectory.myFullPath)
			{
				dir.myParentPath = myRootDirectory.myFullPath;
				myRootDirectory.myDirectories.push_back(dir);
				continue;
			}

			Directory* foundDir = GetDirectoryInHierarchy(myRootDirectory, root);
			dir.myParentPath = foundDir->myFullPath;
			foundDir->myDirectories.push_back(dir);

		}

		Directory* lastUsedDir = &myRootDirectory;

		ModelManager* mM = Context::Get()->myModelManager;

		for (int i = 0; i < someDirectoryContent.size(); i++)
		{
			std::string fileStr = someDirectoryContent[i];
			slash = fileStr.find_last_of("\\");

			std::string dirName = fileStr.substr(0, slash);

			Content content;
			content.myFullPath = fileStr;
			content.myName = fileStr.substr(slash + 1);
			
			size_t dot = fileStr.find_last_of(".");

			std::string subbed = fileStr.substr(dot);

			//figure out display should be done better
			if (subbed == ".dds" || subbed == ".DDS")
			{
				std::wstring str(content.myFullPath.begin(), content.myFullPath.end());
				DirectX::CreateDDSTextureFromFile(Context::Get()->myDX11->GetDevice(), str.c_str(), nullptr, &content.SRV);
				content.myDrawMode = DrawMode::Texture;
			}
			else if (subbed == ".png")
			{
				content.myDontRelease = true;
				content.SRV = myFCKOFFSRV;
			}
			else if (subbed == ".fbx")
			{
				content.myDontRelease = true;
				/*ModelManager::ModelLoadResult result = mM->LoadModel(fileStr);
				
				if (result.mySuccesFlag)
				{
					content.SRV = mM->GetPreviewRenderTarget(result.myValue).GetSRV();
				}
				else
				{
					content.SRV = myAnimationSRV;
				}*/

				content.myDrawMode = DrawMode::FBX;
			}
			else if (subbed == ".uwu")
			{
				content.myDontRelease = true;
				content.SRV = mySceneSRV;
			}
			else if (subbed == ".wav" || subbed == ".mp3")
			{
				content.myDontRelease = true;
				content.SRV = myMusicSRV;
			}
			else
			{
				content.myDontRelease = true;
				content.SRV = myTXTSRV;
			}
			//

			if (lastUsedDir->myFullPath == dirName)
			{
				lastUsedDir->myContent.push_back(content);
				continue;
			}

			lastUsedDir = GetDirectoryInHierarchy(myRootDirectory, dirName);
			lastUsedDir->myContent.push_back(content);

		
		}

		//std::cout << myRootDirectory.myName << std::endl;
		//PrintDir(myRootDirectory, 1);

	}

#pragma region FileWathcerCallback
	void AssetsManager::FolderChange()
	{
		try
		{
			myFileWatcher.Reconstruct();
		}
		catch (...)
		{

		}
	}
	void AssetsManager::FolderContentChange(std::vector<std::string> someContent)
	{
		mySelectedDir->myContent.clear();
		
		ModelManager* mM = Context::Get()->myModelManager;


		size_t slash = 0;
		size_t secondSlash = 0;

		for (int i = 0; i < someContent.size(); i++)
		{
			std::string fileStr = someContent[i];
			slash = fileStr.find_last_of("\\");

			std::string dirName = fileStr.substr(0, slash);

			Content content;
			content.myFullPath = fileStr;
			content.myName = fileStr.substr(slash + 1);

			size_t dot = fileStr.find_last_of(".");

			//figure out display should be done better
			if (fileStr.substr(dot) == ".dds")
			{
				std::wstring str(content.myFullPath.begin(), content.myFullPath.end());
				DirectX::CreateDDSTextureFromFile(Context::Get()->myDX11->GetDevice(), str.c_str(), nullptr, &content.SRV);
				content.myDrawMode = DrawMode::Texture;
			}
			else if (fileStr.substr(dot) == ".png")
			{
				content.myDontRelease = true;
				content.SRV = myFCKOFFSRV;
			}
			else if (fileStr.substr(dot) == ".fbx")
			{
				content.myDontRelease = true;
				//ModelManager::ModelLoadResult result = mM->LoadModel(fileStr);

				//if (result.mySuccesFlag)
				//{
				//	content.SRV = mM->GetPreviewRenderTarget(result.myValue).GetSRV();
				//}
				//else
				//{
				//	content.SRV = myAnimationSRV;
				//}

				content.myDrawMode = DrawMode::FBX;
			}
			else if (fileStr.substr(dot) == ".uwu")
			{
				content.myDontRelease = true;
				content.SRV = mySceneSRV;
			}
			else if (fileStr.substr(dot) == ".wav" || fileStr.substr(dot) == ".mp3")
			{
				content.myDontRelease = true;
				content.SRV = myMusicSRV;
			}
			else
			{
				content.myDontRelease = true;
				content.SRV = myTXTSRV;
			}

			mySelectedDir->myContent.push_back(content);
			
		}
	}
#pragma endregion

	void AssetsManager::CleanUpContent(Directory& aDir)
	{
		for (auto& dir : aDir.myDirectories)
		{

			CleanUpContent(dir);

			for (auto& content : dir.myContent)
			{
				if (content.SRV != nullptr && !content.myDontRelease)
					content.SRV->Release();
			}
		}

	}

	Directory* AssetsManager::GetDirectoryInHierarchy(Directory& aDir, std::string aDirName)
	{
		for (auto& dir : aDir.myDirectories)
		{
			if (dir.myFullPath == aDirName)
			{
				//UwU
				return &dir;
			}

			Directory* d = GetDirectoryInHierarchy(dir, aDirName);

			if (d != nullptr)
				return d;

		}

		return nullptr;
	}

	void AssetsManager::PrintDir(Directory& aDir, int aLayer)
	{
		for (auto& dir : aDir.myDirectories)
		{
			for (int i = 0; i < aLayer; i++)
			{
				std::cout << "  ";
			}
			std::cout << dir.myName << std::endl;

			PrintDir(dir, aLayer + 1);

			for (auto& c : dir.myContent)
			{
				for (int i = 0; i < aLayer + 1; i++)
				{
					std::cout << "  ";
				}

				std::cout << c.myName << std::endl;
			}
		}
	}

#pragma region Draws
	void AssetsManager::DrawAssetsHierarchy()
	{
		ImGui::BeginChild("AssetsTreeView", { ImGui::GetWindowWidth() * myAssetTreeSizeX, 0});


		ImGui::Image((ImTextureID)myFolderSRV, { 16,16 });

		if (ImGui::IsItemClicked() && myDoubleClicked)
		{
			mySelectedDir = &myRootDirectory;
		}

		ImGui::SameLine();
		if (ImGui::TreeNode(myRootDirectory.myName.c_str()))
		{
			for (auto& dir : myRootDirectory.myDirectories)
			{
				DrawChildTree(dir);
			}


			for (auto& content : myRootDirectory.myContent)
			{
				ImGui::Text(content.myName.c_str());
			}

			ImGui::TreePop();
		}

		ImGui::EndChild();
	}
	void AssetsManager::DrawChildTree(Directory& aDir)
	{
		ImGui::Image((ImTextureID)myFolderSRV, { 16,16 });

		if (ImGui::IsItemClicked() && myDoubleClicked)
		{
			mySelectedDir = &aDir;
		}

		ImGui::SameLine();
		if (ImGui::TreeNode(aDir.myName.c_str()))
		{
			for (auto& dir : aDir.myDirectories)
			{
				DrawChildTree(dir);
			}

			for (auto& content : aDir.myContent)
			{
				ImGui::Text(content.myName.c_str());
			}


			ImGui::TreePop();
		}

	}
	void AssetsManager::DrawAssetsViewer()
	{

		//the visualsations of the assets in that folder
		ImGui::SameLine();

		ImGui::BeginChild("FolderViewer", { ImGui::GetWindowWidth() - (ImGui::GetWindowWidth() * myAssetTreeSizeX), 0 });


		ImGui::Text(mySelectedDir->myFullPath.c_str());

		ImVec2 windowSize = ImGui::GetWindowSize();

		float imPadding = imgSize * 2 + 10;

		int sameLineCount = windowSize.x / imPadding;
		int i = 0;
		//sameLineCount--;
		auto pos = ImGui::GetCursorPos();

		float xStart = pos.x;

		bool itemHovered = false;

		
		for (auto& dir : mySelectedDir->myDirectories)
		{

			i++;

			ImVec2 uv0 = { 0,0 };
			ImVec2 uv1 = { 1,1 };
			ImVec4 tintColor = { 1,1,1,1 };
			ImVec4 borderColor = { 0,0,0,0 };

			if (myHoveredItem == dir.myFullPath)
				borderColor = { 0.52f,0.8f,0.92f,1 };

			ImGui::Image((ImTextureID)myFolderSRV, { imgSize,imgSize }, uv0, uv1, tintColor, borderColor);

			if (ImGui::IsItemHovered())
			{
				myHoveredItem = dir.myFullPath;
				itemHovered = true;
			}

			pos.y += imgSize;

			ImGui::SetCursorPos(ImVec2(pos.x, pos.y));

			if (ImGui::IsItemClicked() && myDoubleClicked)
			{
				mySelectedDir = &dir;
				break;
			}

			ImVec2 textSize = ImGui::CalcTextSize(dir.myName.c_str());
			size_t stringSize = dir.myName.size();

			size_t characterLength = textSize.x / stringSize;
			size_t charactersPerLine = imgSize / characterLength;


			std::string newString = "";

			int count = 0;
			for (int c = 0; c < stringSize; c++)
			{
				newString += dir.myName[c];

				if (count == charactersPerLine)
				{
					count = 0;
					newString += "\n";
				}

				count++;
			}

			ImGui::Text(newString.c_str());

			pos.y -= imgSize;

			if (i == sameLineCount)
			{
				pos.y += imPadding;
				i = 0;
				pos.x = xStart;
			}
			else
			{
				pos.x += imPadding;
			}

			ImGui::SetCursorPos(ImVec2(pos.x, pos.y));

		}

		for (auto& content : mySelectedDir->myContent)
		{
			i++;

			ImVec2 uv0 = { 0,0 };
			ImVec2 uv1 = { 1,1 };
			ImVec4 tintColor = { 1,1,1,1 };
			ImVec4 borderColor = { 0,0,0,0 };

			if (myHoveredItem == content.myFullPath)
				borderColor = { 0.52f,0.8f,0.92f,1 };

			if (myHighLightContent && myHiglightedContent == content.myFullPath)
			{
				if (myScrollSet == false)
				{
					ImGui::SetScrollY(pos.y);
					myScrollSet = true;
				}

				borderColor = { 1,0.37f,0.12f, 1 };
			}

			ImGui::Image((ImTextureID)content.SRV, { imgSize,imgSize }, uv0, uv1, tintColor, borderColor);

			if (ImGui::IsItemHovered())
			{
				myHoveredItem = content.myFullPath;
				itemHovered = true;
			}

			pos.y += imgSize;

			ImGui::SetCursorPos(ImVec2(pos.x, pos.y));


			if (ImGui::IsItemClicked() && myDoubleClicked)
			{
				CheckSpecialCase(content);
			}

			ImGuiDragDropFlags srcFlags = 0;
			srcFlags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceNoDisableHover;
			srcFlags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID;

			if (ImGui::BeginDragDropSource(srcFlags))
			{
				if (!(srcFlags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
					ImGui::Text("Moving \"%s\"", content.myName.c_str());

				//temporary
				size_t dotIndex = content.myName.find_last_of(".");
				std::string extension = content.myName.substr(dotIndex);

				ImGui::SetDragDropPayload(extension.c_str(), &content.myFullPath, sizeof(std::string));
				ImGui::EndDragDropSource();
			}

			ImVec2 textSize = ImGui::CalcTextSize(content.myName.c_str());
			size_t stringSize = content.myName.size();

			size_t characterLength = textSize.x / stringSize;
			size_t charactersPerLine = imgSize / characterLength;


			std::string newString = "";

			int count = 0;
			for (int c = 0; c < stringSize; c++)
			{
				newString += content.myName[c];

				if (count == charactersPerLine)
				{
					count = 0;
					newString += "\n";
				}

				count++;
			}


			if (myHighLightContent && myHiglightedContent == content.myFullPath)
				ImGui::PushStyleColor(ImGuiColorEditFlags_::ImGuiColorEditFlags_None, { 1,0.37f,0.12f, 1 });

			ImGui::Text(newString.c_str());

			if (myHighLightContent && myHiglightedContent == content.myFullPath)
				ImGui::PopStyleColor();

			pos.y -= imgSize;

			if (i == sameLineCount)
			{
				pos.y += imPadding;
				i = 0;
				pos.x = xStart;
			}
			else
			{
				pos.x += imPadding;
			}

			ImGui::SetCursorPos(ImVec2(pos.x, pos.y));

		}

		if (!itemHovered)
			myHoveredItem = "";

		ImGui::EndChild();
	}
#pragma endregion

	void AssetsManager::CheckSpecialCase(Content content)
	{
		//calc extension

		//if scene 
		size_t dotIndex = content.myName.find_last_of(".");
		std::string extension = content.myName.substr(dotIndex);


		if (extension == ".uwu") 
		{
			Context::Get()->mySceneHandler->LoadScene(content.myFullPath, true);
			myLoadedScene = true;
		}
		else if (extension == ".monster")
		{
			Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

			if (scene.IsEntityIDValid(myPrefabTarget))
				scene.DestroyEntity(myPrefabTarget);

			myPrefabPathTarget = content.myFullPath;
			myPrefabTarget = Context::Get()->mySceneHandler->myBuilder->CreateFromPrefab(myPrefabPathTarget);

			const Entity& entity = scene.GetEntity(myPrefabTarget);

			mySelectedEntity = entity.myId;
			myTmpEntityTag = entity.myTag;
			myTmpEntityLayer = entity.myLayer;
			myTmpEntityName = entity.myName;


			
			EditorContext::Get()->myBaseEditor->SetTarget(mySelectedEntity);
			EditorContext::Get()->myBaseEditor->SnapToFocusedEntity(true);

		}
		else
		{
			size_t slash = content.myFullPath.find_first_of("/");
			std::string subbed = content.myFullPath.substr(slash + 1);
			 
			std::string arg = "start " + (("..\\" + ( '"' + subbed + '"')));
			system(arg.c_str());
		}
	}

#pragma region PrefabEdit
	void AssetsManager::EditPrefab()
	{

		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		if (!scene.IsEntityIDValid(myPrefabTarget))
			return;

		const Entity& entity = scene.GetEntity(myPrefabTarget);

		{

			ImGui::Begin("PrefabObjects");

			if (ImGui::Button("ExitPrefabView"))
			{
				scene.DestroyEntity(myPrefabTarget);
				myPrefabTarget = INVALID_ENTITY;
				mySelectedEntity = INVALID_ENTITY;
				ImGui::End();
				return;
			}

			//ImGui::PushID(GetEntityIndex(entity.myId));
			bool selected = (mySelectedEntity == entity.myId);
			if (entity.myChildren.size() != 0)
			{
				bool treeNodeOpen = ImGui::TreeNode("##Default");
				ImGui::SameLine();
				if (ImGui::Selectable(entity.myName.c_str(), &selected))
				{
					mySelectedEntity = entity.myId;
					myTmpEntityTag = entity.myTag;
					myTmpEntityLayer = entity.myLayer;
					myTmpEntityName = entity.myName;

				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityID", 0))
					{
						EntityID id = *(EntityID*)payload->Data;
						
						const Entity& TargetEntity = scene.GetEntity(entity.myId);
						const Entity& DropedEntity = scene.GetEntity(id);

						if (TargetEntity.myParent != DropedEntity.myId)
						{
							scene.SetParent(DropedEntity.myId, TargetEntity.myId);
						}

					}

					ImGui::EndDragDropTarget();
				}
				if (treeNodeOpen)
				{
					DrawChildren(entity.myChildren, scene);

					ImGui::TreePop();
				}

			}
			else
			{
				if (ImGui::Selectable(entity.myName.c_str(), &selected))
				{
					mySelectedEntity = entity.myId;
					myTmpEntityTag = entity.myTag;
					myTmpEntityLayer = entity.myLayer;
					myTmpEntityName = entity.myName;
					
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityID", 0))
					{
						EntityID id = *(EntityID*)payload->Data;

						const Entity& TargetEntity = scene.GetEntity(entity.myId);
						const Entity& DropedEntity = scene.GetEntity(id);

						if (TargetEntity.myParent != DropedEntity.myId)
						{
							scene.SetParent(DropedEntity.myId, TargetEntity.myId);
						}

					}

					ImGui::EndDragDropTarget();
				}
			}

			if (scene.IsEntityIDValid(mySelectedEntity) && ImGui::BeginPopupContextWindow("Options"))
			{

				if (ImGui::Selectable("CreateEntity"))
				{
					EntityID newEntity = scene.NewEntity();

					scene.AssignComponent<TransformComponent>(newEntity);
					scene.SetParent(newEntity, entity.myId);

					mySelectedEntity = newEntity;
					myTmpEntityTag = scene.GetTag(newEntity);
					myTmpEntityLayer = scene.GetLayer(newEntity);
					myTmpEntityName = scene.GetEntityName(newEntity);

					//ImGui::EndPopup();
				}

				if (ImGui::Selectable("DeleteSelected"))
				{
					scene.DestroyEntity(mySelectedEntity);
					mySelectedEntity = INVALID_ENTITY;
				}


				ImGui::EndPopup();
			}

			if (ImGui::Button("SavePrefab"))
			{
				SceneBuilder* builder = Context::Get()->mySceneHandler->GetSceneBuilder();

				nlohmann::json json = builder->CreatePrefabJson(entity);

				std::ofstream stream(myPrefabPathTarget);
				stream << json;
				stream.close();

				UpdateEntities(json, entity, scene);
			}

			ImGui::End();
		}

		DrawInspector(scene);
	}
	void AssetsManager::DrawChildren(std::vector<EntityID> someIDs, Scene& aScene)
	{
		for (auto& child : someIDs)
		{
			const Entity& entity = aScene.GetEntity(child);

			bool selected = (mySelectedEntity == entity.myId);
			if (entity.myChildren.size() != 0)
			{
				bool treeNodeOpen = ImGui::TreeNode("##Default");
				ImGui::SameLine();
				if (ImGui::Selectable(entity.myName.c_str(), &selected))
				{
					mySelectedEntity = entity.myId;
					myTmpEntityTag = entity.myTag;
					myTmpEntityLayer = entity.myLayer;
					myTmpEntityName = entity.myName;

				}
				ImGuiDragDropFlags srcFlags = 0;
				srcFlags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceNoDisableHover;
				srcFlags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID;

				if (ImGui::BeginDragDropSource(srcFlags))
				{
					if (!(srcFlags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
						ImGui::Text("Moving \"%s\"", entity.myName);

					ImGui::SetDragDropPayload("EntityID", &entity.myId, sizeof(EntityID));
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityID", 0))
					{
						EntityID id = *(EntityID*)payload->Data;

						const Entity& TargetEntity = aScene.GetEntity(entity.myId);
						const Entity& DropedEntity = aScene.GetEntity(id);

						if (TargetEntity.myParent != DropedEntity.myId)
						{
							aScene.SetParent(DropedEntity.myId, TargetEntity.myId);
						}

					}

					ImGui::EndDragDropTarget();
				}
				if (treeNodeOpen)
				{
					DrawChildren(entity.myChildren, aScene);

					ImGui::TreePop();
				}

			}
			else
			{
				if (ImGui::Selectable(entity.myName.c_str(), &selected))
				{
					mySelectedEntity = entity.myId;
					myTmpEntityTag = entity.myTag;
					myTmpEntityLayer = entity.myLayer;
					myTmpEntityName = entity.myName;

				}
				ImGuiDragDropFlags srcFlags = 0;
				srcFlags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceNoDisableHover;
				srcFlags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID;

				if (ImGui::BeginDragDropSource(srcFlags))
				{
					if (!(srcFlags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
						ImGui::Text("Moving \"%s\"", entity.myName);

					ImGui::SetDragDropPayload("EntityID", &entity.myId, sizeof(EntityID));
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityID", 0))
					{
						EntityID id = *(EntityID*)payload->Data;

						const Entity& TargetEntity = aScene.GetEntity(entity.myId);
						const Entity& DropedEntity = aScene.GetEntity(id);

						if (TargetEntity.myParent != DropedEntity.myId)
						{
							aScene.SetParent(DropedEntity.myId, TargetEntity.myId);
						}

					}

					ImGui::EndDragDropTarget();
				}
			}
		}
	}
	void AssetsManager::DrawInspector(Scene& aScene)
	{
		//we should in theory not need this but since the editor tends to crash alot
		if (!aScene.IsEntityIDValid(mySelectedEntity))
			return;

		ImGui::Begin("PrefabInspector");

		ImGuiInputTextFlags flag = ImGuiInputTextFlags_None;

		if (Context::Get()->myInputHandler->IsKeyHeldDown(MK_RBUTTON))
		{
			flag = ImGuiInputTextFlags_ReadOnly;
		}

		std::string pConnection = aScene.GetEntity(mySelectedEntity).myPrefabConnection;
		if (pConnection == "")
			pConnection = "N/A";

		ImGui::InputText("Prefab", &pConnection, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

		ImGui::InputText("##EntityName", &myTmpEntityName, flag);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aScene.SetEntityName(mySelectedEntity, myTmpEntityName);
		}

		ImGui::InputText("##EntityTag", &myTmpEntityTag, flag);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aScene.SetTag(mySelectedEntity, myTmpEntityTag);
		}
		ImGui::SameLine();
		ImGui::Text("Tag");
		ImGui::InputText("##EntityLayer", &myTmpEntityLayer, flag);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			aScene.SetLayer(mySelectedEntity, myTmpEntityLayer);
		}
		ImGui::SameLine();
		ImGui::Text("Layer");

		ImGui::Separator();

		for (auto& edit : myComponentsToEdit)
		{
			edit->Draw(&aScene, aScene.myEntities[GetEntityIndex(mySelectedEntity)]);
		}

		if (ImGui::Button("Add Component"))
		{
			myDisplayComponents = !myDisplayComponents;
		}

		SceneHandler* handler = Context::Get()->mySceneHandler;

		const Entity& tEnt = handler->GetActiveScene().GetEntity(mySelectedEntity);



		if (myDisplayComponents)
		{

			ImGui::InputText("AddSearch", &myTmpComponentSearch);

			for (auto& name : handler->myComponentNames)
			{
				std::string uglyTing;
				std::string uglyTing2;

				for (auto& ew : name.second)
				{
					uglyTing += std::tolower(ew);
				}

				for (auto& ew : myTmpComponentSearch)
				{
					uglyTing2 += std::tolower(ew);
				}

				if (uglyTing.find(uglyTing2) >= 10000)
					continue;

				if (ImGui::Selectable(name.second.c_str()))
				{
					int cmpId = handler->myComponentIds[name.second];

					ComponentMask mask;
					mask.set(cmpId);


					void* cmp = nullptr;

					if ((mask & handler->myFBM) != 0)
					{
						cmp = handler->GetActiveScene().AssignBehaviour(cmpId, mySelectedEntity);
					}
					else
					{
						cmp = handler->GetActiveScene().AssignComponent(cmpId, mySelectedEntity);
					}

					break;
				}
			}
		}

		if (ImGui::Button("Remove Component"))
		{
			myDisplayRemoveComponents = !myDisplayRemoveComponents;
		}

		if (myDisplayRemoveComponents)
		{
			for (int i = 0; i < tEnt.myMask.size(); i++)
			{
				if (tEnt.myMask.test(i))
				{

					if (ImGui::Selectable(handler->myComponentNames[i].c_str()))
					{
						ComponentMask mask;
						mask.set(i);

						if ((mask & handler->myFBM) != 0)
						{
							handler->GetActiveScene().RemoveBehaviour(i, mySelectedEntity);
						}
						else
						{
							handler->GetActiveScene().RemoveComponent(i, mySelectedEntity);
						}
					}
				}
			}
		}

		ImGui::End();
	}
	void AssetsManager::UpdateEntities(nlohmann::json& aJson, const Entity& aRefEntity, Scene& aScene)
	{
		SceneHandler* handler = Context::Get()->mySceneHandler;
		SceneBuilder* builder = handler->myBuilder;

		ComponentMask ignoreMask;
		ignoreMask.set(handler->GetId<TransformComponent>());


		std::vector<Entity> copyList = aScene.myEntities;

		for (auto& entity : copyList)
		{
			if (entity.myPrefabConnection == myPrefabPathTarget && entity.myId != aRefEntity.myId)
			{

				//aScene.SetEntityName(entity.myId, aJson["Name"].get<std::string>());
				aScene.SetTag(entity.myId, aJson["Tag"].get<std::string>());
				aScene.SetLayer(entity.myId, aJson["Layer"].get<std::string>());

				nlohmann::json& compList = aJson["ComponentList"];

				size_t compnentAmount = compList.size();

				ComponentMask trueMask;

				for (size_t i = 0; i < compnentAmount; i++)
				{
					std::string compName = compList[i]["Name"].get<std::string>();
					ComponentID compID = handler->myComponentIds[compName];

					trueMask.set(compID);

					if (ignoreMask.test(compID))
						continue;

					if (!entity.myMask.test(compID))
					{
						aScene.AssignComponent(compID, entity.myId);
					}

					builder->myCRAWs[compName]->Read(aScene.GetComponent(compID, entity.myId), entity.myId, compList[i]["Data"]);
				}

				for (size_t i = 0; i < MAX_COMPONENTS; i++)
				{
					if (!trueMask.test(i) && entity.myMask.test(i))
					{
						aScene.RemoveComponent((ComponentID)i, entity.myId);
					}
				}

				nlohmann::json& children = aJson["Children"];

				size_t childrenAmount = children.size();
				for (int c = entity.myChildren.size() - 1; c >= 0; c--)
				{
					bool exist = false;
					for (size_t i = 0; i < childrenAmount; i++)
					{
						if (children[i]["Name"].get<std::string>() == aScene.GetEntity(entity.myChildren[c]).myName)
						{
							UpdateEntity(children[i], aScene.myEntities[GetEntityIndex(entity.myChildren[c])], aScene, ignoreMask);
							exist = true;
							break;
						}
					}

					if(!exist)
						aScene.DestroyEntity(entity.myChildren[c]);
				}

				for (size_t i = 0; i < childrenAmount; i++)
				{
					bool exist = false;
					for (size_t c = 0; c < entity.myChildren.size(); c++)
					{
						if (aScene.GetEntity(entity.myChildren[c]).myName == children[i]["Name"].get<std::string>())
						{
							exist = true;
							break;
						}
					}

					if (!exist && entity.myId != INVALID_ENTITY)
					{
						EntityID nID = aScene.NewEntity();
						UpdateEntity(children[i], aScene.myEntities[GetEntityIndex(nID)], aScene, ignoreMask);
						aScene.SetParent(nID, entity.myId, false);
						aScene.myEntities[GetEntityIndex(nID)].myIsEditorCreated = true;
					}
				}

			}
		}

	}
	void AssetsManager::UpdateEntity(nlohmann::json& aJson, Entity& aEntity, Scene& aScene, ComponentMask& aIgnoreMask)
	{
		SceneHandler* handler = Context::Get()->mySceneHandler;
		SceneBuilder* builder = handler->myBuilder;

		aEntity.myTag = aJson["Tag"];
		aEntity.myLayer = aJson["Layer"];
		aEntity.myName = aJson["Name"];

		nlohmann::json& compList = aJson["ComponentList"];

		size_t compnentAmount = compList.size();

		ComponentMask trueMask;

		for (size_t i = 0; i < compnentAmount; i++)
		{
			std::string compName = compList[i]["Name"].get<std::string>();
			ComponentID compID = handler->myComponentIds[compName];

			trueMask.set(compID);

			if (!aEntity.myMask.test(compID))
			{
				aScene.AssignComponent(compID, aEntity.myId);
			}

			builder->myCRAWs[compName]->Read(aScene.GetComponent(compID, aEntity.myId), aEntity.myId, compList[i]["Data"]);
		}

		for (size_t i = 0; i < MAX_COMPONENTS; i++)
		{
			if (!trueMask.test(i) && aEntity.myMask.test(i))
			{
				aScene.RemoveComponent((ComponentID)i, aEntity.myId);
			}
		}

		nlohmann::json& children = aJson["Children"];

		size_t childrenAmount = children.size();
		for (int c = aEntity.myChildren.size() - 1; c >= 0; c--)
		{
			bool exist = false;
			for (size_t i = 0; i < childrenAmount; i++)
			{
				if (children[i]["Name"].get<std::string>() == aScene.GetEntity(aEntity.myChildren[c]).myName)
				{
					UpdateEntity(children[i], aScene.myEntities[GetEntityIndex(aEntity.myChildren[c])], aScene, aIgnoreMask);
					exist = true;
					break;
				}
			}

			if (!exist)
				aScene.DestroyEntity(aEntity.myChildren[c]);
		}

		for (size_t i = 0; i < childrenAmount; i++)
		{
			bool exist = false;
			for (size_t c = 0; c < aEntity.myChildren.size(); c++)
			{
				if (aScene.GetEntity(aEntity.myChildren[c]).myName == children[i]["Name"].get<std::string>())
				{
					exist = true;
					break;
				}
			}

			if (!exist)
			{
				EntityID nID = aScene.NewEntity();
				UpdateEntity(children[i], aScene.myEntities[GetEntityIndex(nID)], aScene, aIgnoreMask);
				aScene.SetParent(nID, aEntity.myId, false);
				aScene.myEntities[GetEntityIndex(nID)].myIsEditorCreated = true;
			}
		}

	}
#pragma endregion
}