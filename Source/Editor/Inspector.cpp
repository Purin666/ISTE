#include "Inspector.h"
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/ECSB/Behaviour.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"
#include "ISTE/Graphics/Resources/ShaderEnums.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"

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


#include "ISTE/CU/MemTrack.hpp"

ISTE::Inspector::~Inspector()
{
	for (AbstractComponentEdit* i : myComponentsToEdit)
	{
		delete i;
	}
	myComponentsToEdit.clear();
}

void ISTE::Inspector::Init()
{
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
}

void ISTE::Inspector::SetTarget(const Entity& anEntity)
{
	if (myTarget.myId == anEntity.myId)
	{
		myInspecting = !myInspecting;
	}
	else
	{
		myTarget = anEntity;
		myInspecting = true;
		myTmpEntityName = anEntity.myName;
		myTmpEntityTag = anEntity.myTag;
		myTmpEntityLayer = anEntity.myLayer;
	}
}

void ISTE::Inspector::Update()
{

	ISTE::Scene* tempScenePointer = &ISTE::Context::Get()->mySceneHandler->GetActiveScene();

	if (!tempScenePointer->IsEntityIDValid(myTarget.myId))
	{
		myTarget.myId = INVALID_ENTITY;
		myInspecting = false;
	}

	if (!myInspecting)
	{
		return;
	}
	if (GetEntityIndex(myTarget.myId) == (EntityIndex)-1)
	{
		ResetTarget();
		return;
	}


	ImGui::Begin("Inspector", &myInspecting);
	std::string pConnection = myTarget.myPrefabConnection;
	if (pConnection == "")
		pConnection = "N/A";
	ImGui::InputText("Prefab", &pConnection, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	ImGui::InputText("##EntityName", &myTmpEntityName);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		tempScenePointer->SetEntityName(myTarget.myId, myTmpEntityName);
	}

	ImGui::InputText("##EntityTag", &myTmpEntityTag);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		tempScenePointer->SetTag(myTarget.myId, myTmpEntityTag);
	}
	ImGui::SameLine();
	ImGui::Text("Tag");
	ImGui::InputText("##EntityLayer", &myTmpEntityLayer);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		tempScenePointer->SetLayer(myTarget.myId, myTmpEntityLayer);
	}
	ImGui::SameLine();
	ImGui::Text("Layer");

	ImGui::Separator(); 

	for (AbstractComponentEdit* i : myComponentsToEdit)
	{
		i->Draw(tempScenePointer, myTarget);
	}

	if (ImGui::Selectable("Add Component"))
	{
		myDisplayComponents = !myDisplayComponents;
	}

	SceneHandler* handler = Context::Get()->mySceneHandler;

	const Entity& tEnt = handler->GetActiveScene().GetEntity(myTarget.myId);

	if (myDisplayComponents)
	{

		for (auto& name : handler->myComponentNames)
		{
			if (ImGui::Selectable(name.second.c_str()))
			{
				int cmpId = handler->myComponentIds[name.second];

				ComponentMask mask;
				mask.set(cmpId);


				void* cmp = nullptr;

				if ((mask & handler->myFBM) != 0)
				{
					cmp = handler->GetActiveScene().AssignBehaviour(cmpId, myTarget.myId);
				}
				else
				{
					cmp = handler->GetActiveScene().AssignComponent(cmpId, myTarget.myId);
				}
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
						handler->GetActiveScene().RemoveBehaviour(i, myTarget.myId);
					}
					else
					{
						handler->GetActiveScene().RemoveComponent(i, myTarget.myId);
					}
				}
			}
		}
	}

	ImGui::End();

}

void ISTE::Inspector::ResetTarget()
{
	myInspecting = false;
}