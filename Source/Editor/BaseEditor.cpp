#include "BaseEditor.h"
#include "ISTE/Builder/SceneBuilder.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Builder/BuilderDataStructs.h"
#include "ISTE/ECSB/ECSDefines.hpp"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/Physics/PhysicsEngine.h"
#include "Commands/CommandManager.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/ComponentAndSystem/CameraSystem.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"
#include "ISTE/CU/Database.h"
#include "ISTE/ComponentsAndSystems/EnemyBehaviourSystem.h"

// tools
#include "Tools/AnimationBlendTool.h"
#include "Tools/PPEffectsTool.h"
#include "Tools/PlayerCameraTool.h"
#include "Tools/ResolutionTool.h"
#include "Tools/NavMeshTool.h"
#include "Tools/ProfilingTool.h"
#include "Tools/FogTool.h"
#include "Tools/UITool.h"
#include "Tools/PlayerTool.h"
#include "Tools/VFXTool.h"
#include "Tools/StateTool.h"
#include "Tools/BehaviourTool.h"
#include "Tools/LightTool.h"
#include "Tools/EditorSettingsTool.h"

#include "imgui/imgui_internal.h"

#include "ISTE/Audio/AudioSourceSystem.h"

#include "ISTE/Graphics/GBuffer.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformSystem.h"

#include "ISTE/CU/MemTrack.hpp"



bool ISTE::BaseEditor::myWantToUpdateSize = false;

void ISTE::BaseEditor::Init()
{
	myInspector.Init();
	myImGuiInterface.Init();
	for (ISTE::SceneData i : ISTE::Context::Get()->mySceneHandler->GetSceneBuilder()->myTestScenes)
	{
		SceneLoadInfo info;
		info.mySceneName = i.myName;
		info.myScenePath = i.myPath;
		myTestScenes.push_back(info);
	}
	for (ISTE::SceneData i : ISTE::Context::Get()->mySceneHandler->GetSceneBuilder()->myBuildScenes)
	{
		SceneLoadInfo info;
		info.mySceneName = i.myName;
		info.myScenePath = i.myPath;
		myBuildScenes.push_back(info);
	}


	myTools.push_back(new ISTE::PPEffectsTool);
	myTools.push_back(new ISTE::PlayerCameraTool);
	myTools.push_back(new ISTE::ResolutionTool);
	myTools.push_back(new ISTE::NavMeshTool);
	myTools.push_back(new ISTE::ProfilingTool);
	myTools.push_back(new ISTE::FogTool);
	myTools.push_back(new ISTE::UITool);
	myTools.push_back(new ISTE::PlayerTool);
	myTools.push_back(new ISTE::StateTool);
	myTools.push_back(new ISTE::BehaviourTool);
	myTools.push_back(new ISTE::LightTool);
	myTools.push_back(new ISTE::VFXTool);
	myTools.push_back(new ISTE::AnimationBlendTool);
	myTools.push_back(new ISTE::EditorSettingsTool); //This tool makes it recommended to add all tools before initializing.

	for (AbstractTool* i : myTools)
	{
		i->Init(this);
	}

	CommandManager::Empty();

	Context::Get()->mySystemManager->GetSystem<AudioSourceSystem>()->Init();

	myEditorContext.myInstance = &myEditorContext;
	myEditorContext.myAssetsManager = &myAssetsManager;
	myEditorContext.myGizmos = &myGizmos;
	myEditorContext.myBaseEditor = this;

}

void ISTE::BaseEditor::StartFrame()
{
	myImGuiInterface.BeginFrame();
}

void ISTE::BaseEditor::EndFrame()
{
	if (myWantToUpdateSize)
	{
		myWantToUpdateSize = false;
		ImGui::EndFrame();
		return;
	}
	Context::Get()->mySceneHandler->PostLogicUpdate();
	myImGuiInterface.Render();
}

void ISTE::BaseEditor::DoWholeImGui()
{
	StartFrame();
	EndFrame();
}

void ISTE::BaseEditor::Update()
{
	//test //lukas //cause //think //this //solves //alot //of //problems
	//UpdateSceneObjects();
	{

		if (!myGameOnOff && !myInspectorDisabled) // disable gizmo while playing // Mattias
		{
			myGizmos.CheckInput(myDisplayFocused);
			myGizmoDraw = myGizmos.Draw(myDisplayRect, myDisplayFocused);

			if (myClonedBackList)
				delete myClonedBackList;

			ImDrawList* list = ImGui::GetBackgroundDrawList();

			if (list != nullptr)
			{
				myValidBackList = true;
				myClonedBackList = ImGui::GetBackgroundDrawList()->CloneOutput();

				myDrawData.Valid = true;
				myDrawData.CmdLists = &myClonedBackList;
				myDrawData.CmdListsCount = 1;
				myDrawData.TotalVtxCount = list->VtxBuffer.size();
				myDrawData.TotalIdxCount = list->IdxBuffer.size();

				myDrawData.DisplayPos = ImVec2(0.0f, 0.0f);
				myDrawData.DisplaySize = ImVec2(Context::Get()->myWindow->GetWidth(), Context::Get()->myWindow->GetHeight());
				myDrawData.FramebufferScale = ImVec2(1.0f, 1.0f);
			}
			else
			{
				myValidBackList = false;
			}

		}
	}


	CheckInputs();

	if (myEntities.size() == 0)
	{
		UpdateSceneObjects();
	}
	DrawDisplay();

	DrawMenuBar();

	ObjectMenu();
	SceneMenu();
	if (!myGameOnOff && !myInspectorDisabled)
	{
		myInspector.Update();
	}

	if (myAssetsManager.LoadedScene())
		UpdateSceneObjects();

	if (((!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyWindowFocused()) || myDisplayFocused) && !myGizmoDraw)
	{
		ImGuizmo();
	}
	myGizmoDraw = false;

	ImGui::End();
}

void ISTE::BaseEditor::CheckInputs()
{
	if (Context::Get()->myInputHandler->IsKeyHeldDown(VK_CONTROL) && Context::Get()->myInputHandler->IsKeyDown('Z'))
	{
		CommandManager::Undo();
	}
	if (Context::Get()->myInputHandler->IsKeyHeldDown(VK_CONTROL) && Context::Get()->myInputHandler->IsKeyDown('Y'))
	{
		CommandManager::Redo();
	}
	if (Context::Get()->myInputHandler->IsKeyHeldDown(VK_CONTROL) && Context::Get()->myInputHandler->IsKeyDown('S') && !myGameOnOff)
	{
		UpdateSceneObjects();
		Context::Get()->mySceneHandler->myBuilder->SaveSceneObjectData(myCreatedEntities);
		Context::Get()->mySceneHandler->myBuilder->SaveSceneData();
	}
	if (Context::Get()->myInputHandler->IsKeyHeldDown(VK_CONTROL) && Context::Get()->myInputHandler->IsKeyDown('R'))
	{
		UpdateSceneObjects();
	}
	bool windowFocused = ImGui::IsAnyWindowFocused();
	if ((!windowFocused || myDisplayFocused) && Context::Get()->myInputHandler->IsKeyDown('F'))
	{
		SnapToFocusedEntity();
	}
	if (Context::Get()->myInputHandler->IsKeyHeldDown(VK_CONTROL) && Context::Get()->myInputHandler->IsKeyDown('C'))
	{
		if (myFocusedID != INVALID_ENTITY)
		{
			myCopy = Context::Get()->mySceneHandler->myBuilder->CreatePrefabJson(Context::Get()->mySceneHandler->GetActiveScene().myEntities[GetEntityIndex(myFocusedID)]);
			myValidCopy = true;
		}
		else
		{
			myValidCopy = false;
		}
	}
	if (Context::Get()->myInputHandler->IsKeyDown(VK_DELETE))
	{
		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
		scene.DestroyEntity(myFocusedID);
		myFocusedID = INVALID_ENTITY;
		UpdateSceneObjects();
	}
}

ID3D11ShaderResourceView* ISTE::BaseEditor::GetSelectedSRV()
{
	if (Context::Get()->myInputHandler->IsKeyDown(VK_F6))
	{
		int i = (int)myRenderPass + 1;
		if (i == (int)RenderPass::Count)
			i = 0;
		myRenderPass = (RenderPass)i;
	}

	if (myRenderPass != RenderPass::FinalTex)
		return Context::Get()->myDX11->GetMainGBuffer().GetSRV((GBufferTexture)myRenderPass);
	else
		return Context::Get()->myDX11->GetSceneTarget().GetSRV();
}

void ISTE::BaseEditor::DrawDisplay()
{
	myDisplayFocused = false;
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowRounding, 0.0);
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;
		ImGui::Begin("Root", NULL, winFlags);
		ImGui::DockSpace(ImGui::GetID("Root"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::PopStyleVar(3);
	}
	ImGuiWindowFlags winFlags;

	//winFlags = ImGuiWindowFlags_NoMove;
	ImGui::Begin("Display1", NULL, 0);
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	RenderTarget& target = Context::Get()->myDX11->GetSceneTarget();
	CU::Vec2f dispRes = { (float)windowSize.x, (float)windowSize.y };

	//ImGui::Image(target.GetSRV(), { dispRes.x , dispRes.y });
	ImGui::Image(GetSelectedSRV(), { dispRes.x , dispRes.y });

	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();

	myDisplayRect.left = (LONG)min.x;
	myDisplayRect.top = (LONG)min.y;
	myDisplayRect.right = (LONG)max.x;
	myDisplayRect.bottom = (LONG)max.y;

	if (ImGui::IsWindowFocused())
	{

		myDisplayFocused = true;
	}

	if (Context::Get()->myInputHandler->IsKeyHeldDown(VK_CONTROL) && Context::Get()->myInputHandler->IsKeyDown('V') && myValidCopy)
	{
		EntityID id = Context::Get()->mySceneHandler->myBuilder->CreateFromPrefabJson(myCopy);
		Context::Get()->mySceneHandler->GetActiveScene().myEntities[GetEntityIndex(id)].myIsEditorCreated = true;
		SetTarget(id);
		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
		Context::Get()->mySystemManager->GetSystem<TransformSystem>()->CheckCache(id, scene, scene.GetComponentPool<TransformComponent>());
		UpdateSceneObjects();
	}

	if (ImGui::BeginDragDropTarget())
	{
		//should probably be moved to a seperate function
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".fbx", 0))
		{
			std::string fbxPath = *(std::string*)payload->Data;

			Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

			EntityID id = scene.NewEntity();
			scene.myEntities[GetEntityIndex(id)].myName = "Entity_" + std::to_string(scene.myEntities.size());
			scene.myEntities[GetEntityIndex(id)].myIsEditorCreated = true;
			TransformComponent* comp = scene.AssignComponent<TransformComponent>(id);
			comp->myPosition = CU::Vec3f(0, 0, 0);
			comp->myScale = CU::Vec3f(1, 1, 1);
			comp->myEuler.SetRotation(0, 0, 0);
			Context::Get()->myModelManager->LoadModel(id, fbxPath);
			//a bit ugly to check here but ye
			myCreatedEntities.push_back(id);
			UpdateSceneObjects();

			myFocusedID = id;
			myInspector.ResetTarget();
			myInspector.SetTarget(scene.myEntities[GetEntityIndex(id)]);
			myGizmos.Activate(id);
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".monster", 0))
		{
			std::string fbxPath = *(std::string*)payload->Data;

			EntityID id = Context::Get()->mySceneHandler->myBuilder->CreateFromPrefab(fbxPath);

			Context::Get()->mySceneHandler->GetActiveScene().myEntities[GetEntityIndex(id)].myPrefabConnection = fbxPath;
			Context::Get()->mySceneHandler->GetActiveScene().myEntities[GetEntityIndex(id)].myIsEditorCreated = true;
			myCreatedEntities.push_back(id);
			UpdateSceneObjects();

			myFocusedID = id;
			myInspector.ResetTarget();
			myInspector.SetTarget(Context::Get()->mySceneHandler->GetActiveScene().myEntities[GetEntityIndex(id)]);
			myGizmos.Activate(id);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}

void ISTE::BaseEditor::DrawMenuBar()
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Game"))
	{
		if (ImGui::MenuItem("Objects"))
		{
			myObjectMenuOn = !myObjectMenuOn;
		}
		if (ImGui::MenuItem("Scenes"))
		{
			myScenesMenuOn = !myScenesMenuOn;
		}

		ImGui::EndMenu();

	}
	if (ImGui::Button(myGameOnOff ? "Pause" : "Play"))
	{
		myGameOnOff = !myGameOnOff;
		if (myGameOnOff)
		{
			auto db = Context::Get()->myGenericDatabase;
			const size_t sceneIndex = (size_t)Context::Get()->mySceneHandler->GetActiveSceneIndex();
			db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
			db->SetValue<size_t>("SceneToLoadAfterFadeOut", sceneIndex);
			Context::Get()->myStateManager->PushState(StateType::FadeOut);
		}
		else
		{
			Context::Get()->myStateManager->PopAllStates();
			ISTE::Context::Get()->myGraphicsEngine->SetCamera(Context::Get()->mySceneHandler->GetCamera());

		}
	}
	if (ImGui::Button("AssetsManager"))
	{
		myAssetsManagerOnOff = !myAssetsManagerOnOff;
	}
	if (ImGui::Button("Pop All States"))
	{
		Context::Get()->myStateManager->PopAllStates();
	}
	if (ImGui::Button((myInspectorDisabled ? "Enable Inspector" : "Disable Inspector")))
	{
		myInspectorDisabled = !myInspectorDisabled;

	}

	if (ImGui::BeginMenu("Tools"))
	{
		for (AbstractTool* i : myTools)
		{
			i->CheckActive();
		}
		ImGui::EndMenu();
	}
	for (AbstractTool* i : myTools)
	{
		i->Draw();
	}
	myAssetsManager.Draw(&myAssetsManagerOnOff);
	if (ImGui::MenuItem("Save"))
	{
		//maybe just change to SaveSceneData(list<EntityID>)
		UpdateSceneObjects();
		Context::Get()->mySceneHandler->myBuilder->SaveSceneObjectData(myCreatedEntities);
		Context::Get()->mySceneHandler->myBuilder->SaveSceneData();
	}


	if (ImGui::BeginMenu("IconSettings"))
	{
		myIconSettings.StateCheck();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("VisualizationSettings"))
	{
		myVisualizationSettings.StateCheck();

		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("EditorSettings"))
	{
		myEditorSettings.Activate();
	}
	if (ImGui::MenuItem("KillAll"))
	{
		EnemyBehaviourSystem* enemySystem = Context::Get()->mySystemManager->GetSystem<EnemyBehaviourSystem>();
		enemySystem->KillAll();
	}


	myEditorSettings.Draw();

	ImGui::EndMainMenuBar();

}

void ISTE::BaseEditor::ExitImGui()
{
}

void ISTE::BaseEditor::ObjectMenu()
{
	if (myObjectMenuOn)
	{
		ImGui::Begin("Objects", &myObjectMenuOn);

		if (ImGui::Button("Create Entity"))
		{
			Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

			EntityID id = scene.NewEntity();
			EntityIndex index = GetEntityIndex(id);

			scene.myEntities[index].myName = "Entity_" + std::to_string(scene.myEntities.size());
			scene.myEntities[index].myIsEditorCreated = true;
			//myCreatedEntities.push_back(id);

			UpdateSceneObjects();
			myFocusedID = scene.myEntities[index].myId;

			myInspector.ResetTarget();
			myInspector.SetTarget(scene.myEntities[index]);
			myGizmos.Activate(myFocusedID);
		}


		ImGui::Text("Object Search");
		ImGui::InputText(" ", &myObjectSearch);

		for (const ISTE::Entity& i : myEntities) // Just checking that nothing is non-functional.
		{
			if (!Context::Get()->mySceneHandler->GetActiveScene().IsEntityIDValid(i.myId))
			{
				UpdateSceneObjects();
				break;
			}
		}
		if (ImGui::TreeNode("Scene Objects"))
		{
			CheckChildren(myParents);
			DragDrop();
			ImGui::TreePop();
		}
		ImGui::End();
	}
}

void ISTE::BaseEditor::SceneMenu()
{
	if (myScenesMenuOn)
	{
		ImGui::Begin("Scenes", &myScenesMenuOn);

		if (ImGui::TreeNode("Available Build Scenes"))
		{
			for (int i = 0; i != myBuildScenes.size(); i++)
			{
				if (ImGui::Button(myBuildScenes[i].mySceneName.c_str()))
				{
					myInspector.ResetTarget();
					myGizmos.DeActivate();
					ISTE::Context::Get()->mySceneHandler->LoadScene(i, true);
					myCreatedEntities.clear();
					UpdateSceneObjects();
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Available Test Scenes"))
		{
			for (SceneLoadInfo i : myTestScenes)
			{
				if (ImGui::Button(i.mySceneName.c_str()))
				{
					myInspector.ResetTarget();
					myGizmos.DeActivate();
					ISTE::Context::Get()->mySceneHandler->LoadScene(i.myScenePath, true);
					myCreatedEntities.clear();
					UpdateSceneObjects();

				}
			}
			ImGui::TreePop();
		}
		ImGui::End();
	}
}

void ISTE::BaseEditor::UpdateSceneObjects()
{
	myEntities.clear();
	myCreatedEntities.clear();
	myParents.clear();

	ComponentPool& tPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();

	myEntities.reserve(ISTE::Context::Get()->mySceneHandler->GetActiveScene().myEntities.size());

	for (const ISTE::Entity& i : ISTE::Context::Get()->mySceneHandler->GetActiveScene().myEntities)
	{

		if (GetEntityIndex(i.myId) == (EntityIndex)-1)
			continue;

		myEntities.push_back(i);

		TransformComponent* transform = (TransformComponent*)tPool.Get(GetEntityIndex(i.myId));

		if (transform != nullptr)
		{
			transform->myEuler.ToEulerAngles();
		}

		if (i.myIsEditorCreated)
		{
			myCreatedEntities.push_back(i.myId);
		}
		if (i.myParent == INVALID_ENTITY)
		{
			myParents.push_back(i.myId);
		}
	}

}

void ISTE::BaseEditor::ImGuizmo()
{
	if (myGameOnOff || myInspectorDisabled)
		return;

	if (Context::Get()->myInputHandler->IsKeyDown(MK_LBUTTON))
	{
		POINT mousePos = helper.GetMouseRelativePosition();
		if (myDisplayFocused)
		{
			CU::Vec2Ui res = Context::Get()->myWindow->GetResolution();
			int x = mousePos.x - myDisplayRect.left;
			int y = mousePos.y - myDisplayRect.top;

			float percX = (float)x / (float)(myDisplayRect.right - myDisplayRect.left);
			float percY = (float)y / (float)(myDisplayRect.bottom - myDisplayRect.top);

			mousePos = { (int)(res.x * percX), (int)(res.y * percY) };
		}
		EntityID id = ISTE::Context::Get()->myGraphicsEngine->Pick({ (UINT)mousePos.x, (UINT)mousePos.y }).myId;

		//if (id >= myEntities.size())
		//{
		//	UpdateSceneObjects();
		//	return;
		//}

		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		if (id != -1) //Not invalid.
		{
			myInspector.ResetTarget();
			myInspector.SetTarget(scene.myEntities[id]);
			myGizmos.Activate(scene.myEntities[id].myId);
			myFocusedID = scene.myEntities[id].myId;
		}
		else
		{
			myInspector.ResetTarget();
			myGizmos.DeActivate();
			myFocusedID = INVALID_ENTITY;
		}
	}
}

void ISTE::BaseEditor::CheckChildren(const std::vector<EntityID>& anEntityListToCheck)
{

	std::string tempSearch;
	std::transform(myObjectSearch.begin(), myObjectSearch.end(), std::back_inserter(tempSearch), ::tolower);
	
	for (const EntityID& i : anEntityListToCheck)
	{
		const ISTE::Entity& tempEntity = Context::Get()->mySceneHandler->GetActiveScene().GetEntity(i);

		std::string tempName;
		std::transform(myObjectSearch.begin(), myObjectSearch.end(), std::back_inserter(tempSearch), ::tolower);
		std::transform(tempEntity.myName.begin(), tempEntity.myName.end(), std::back_inserter(tempName), ::tolower);

		if (tempName.find(tempSearch) > tempName.length())
		{
			continue;
		}

		if (tempEntity.myChildren.size() > 0)
		{
			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
			std::string tempIdentifier;
			tempIdentifier += "##" + std::to_string(tempEntity.myId);
			bool tempTreeOpen = ImGui::TreeNodeEx(tempIdentifier.c_str(), treeFlags);
			DragDrop(tempEntity.myId);
			ImGui::SameLine();
			if (ImGui::Selectable(tempEntity.myName.c_str()))
			{
				myFocusedID = tempEntity.myId;
				myInspector.SetTarget(Context::Get()->mySceneHandler->GetActiveScene().myEntities[GetEntityIndex(tempEntity.myId)]);
				myGizmos.Activate(tempEntity.myId);
			}
			DragDrop(tempEntity.myId);
			if (tempTreeOpen)
			{
				CheckChildren(tempEntity.myChildren);
				ImGui::TreePop();
			}
		}
		else
		{
			if (ImGui::Selectable(tempEntity.myName.c_str()))
			{

				myFocusedID = tempEntity.myId;
				myInspector.SetTarget(Context::Get()->mySceneHandler->GetActiveScene().myEntities[GetEntityIndex(tempEntity.myId)]);
				myGizmos.Activate(tempEntity.myId);
			}
			DragDrop(tempEntity.myId);
		}
	}
}

void ISTE::BaseEditor::DragDrop(EntityID anID)
{
	if (anID == INVALID_ENTITY)
		return;

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("EntityID", &anID, sizeof(EntityID));
		ImGui::Text((std::to_string(anID)).c_str());
		ImGui::EndDragDropSource();
		//ISTE::Entity childEntity = Context::Get()->mySceneHandler->GetActiveScene().GetEntity(anID);
	}

	if (ImGui::BeginDragDropTarget())
	{

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityID"))
		{
			EntityID* childID = (EntityID*)payload->Data;
			ISTE::Entity childEntity = Context::Get()->mySceneHandler->GetActiveScene().GetEntity(*childID);
			{
				ISTE::Entity tempParent = Context::Get()->mySceneHandler->GetActiveScene().GetEntity(anID);
				while (true)
				{
					if (tempParent.myParent == INVALID_ENTITY)
					{
						break;
					}
					else if (tempParent.myParent == *childID)
					{
						ImGui::EndDragDropTarget();
						return;
					}
					tempParent = Context::Get()->mySceneHandler->GetActiveScene().GetEntity(tempParent.myParent);
				}
			}
			if (anID == INVALID_ENTITY && childEntity.myParent != INVALID_ENTITY)
			{
				Context::Get()->mySceneHandler->GetActiveScene().UnParent(*childID, childEntity.myParent);
				myParents.push_back(*childID);
			}
			else if (anID != INVALID_ENTITY)
			{
				Context::Get()->mySceneHandler->GetActiveScene().SetParent(*childID, anID);
				for (int i = 0; i < myParents.size(); i++)
				{
					if (myParents[i] == *childID)
					{
						myParents.erase(myParents.begin() + i);
						break;
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void ISTE::BaseEditor::DrawGizmos()
{
	if (!myValidBackList || !myGameOnOff || !myInspectorDisabled)
		return;

	//ImDrawList* tempBack = ImGui::GetBackgroundDrawList()->CloneOutput();

	//ImDrawList* back = ImGui::GetBackgroundDrawList();

	//back = myClonedBackList;

	myImGuiInterface.RenderData(&myDrawData);

	//back = tempBack;
}

void ISTE::BaseEditor::PrepareIconRenderCommands()
{
	myIconSettings.DrawIcons();
	myVisualizationSettings.DrawVisuals();
}

EntityID ISTE::BaseEditor::CreateEntity()
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	EntityID id = scene.NewEntity();
	EntityIndex index = GetEntityIndex(id);
	scene.myEntities[index].myName = "Entity_" + std::to_string(scene.myEntities.size());
	scene.myEntities[index].myIsEditorCreated = true;
	myCreatedEntities.push_back(id);
	
	UpdateSceneObjects();
	myFocusedID = scene.myEntities[index].myId; 
	myInspector.ResetTarget();
	myInspector.SetTarget(scene.myEntities[index]);
	myGizmos.Activate(myFocusedID);
	return id;
}

void ISTE::BaseEditor::SetTarget(EntityID aID)
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	if (!scene.IsEntityIDValid(aID))
		return;

	UpdateSceneObjects();
	myFocusedID = aID;
	myInspector.ResetTarget();
	myInspector.SetTarget(scene.myEntities[GetEntityIndex(myFocusedID)]);
	myGizmos.Activate(myFocusedID);
}

void ISTE::BaseEditor::SnapToFocusedEntity(bool aUseLocal)
{
	if (Context::Get()->mySceneHandler->GetActiveScene().IsEntityIDValid(myFocusedID))
	{
		TransformComponent* transform = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myFocusedID);
		if (transform != nullptr)
		{
			CU::Vec3f dir = Context::Get()->myGraphicsEngine->GetCamera().GetTransformNonConst().GetForwardV3();
			CU::Vec3f nPos;
			if (aUseLocal)
			{
				nPos = transform->myPosition - (dir.GetNormalized() * 5.f);
			}
			else
			{
				nPos = transform->myCachedTransform.GetTranslationV3() - (dir.GetNormalized() * 5.f);
			}

			Context::Get()->myGraphicsEngine->GetCamera().GetTransformNonConst().GetRow(4) = CU::Vec4f(nPos.x, nPos.y, nPos.z, 1);
		}
	}
}

ISTE::BaseEditor::~BaseEditor()
{
	if (myClonedBackList)
		delete myClonedBackList;

	for (AbstractTool* i : myTools)
	{
		delete i;
	}
	myTools.clear();
	CommandManager::Empty();
}

bool ISTE::CompareCStrings(const char* aString, const char* aSecondString, const int anAmountOfLetters)
{
	int currentLetter = 0;
	while (aString[currentLetter] == aSecondString[currentLetter] && currentLetter != anAmountOfLetters)
	{
		currentLetter++;
	}
	if (currentLetter == anAmountOfLetters)
	{
		return true;
	}
	return false;
}