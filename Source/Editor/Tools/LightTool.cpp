#include "LightTool.h"

#include <imgui/imgui.h>

#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "BaseEditor.h"
#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/DirectionalLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/AmbientLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

ISTE::LightTool::LightTool()
{
}

ISTE::LightTool::~LightTool()
{
}

void ISTE::LightTool::Draw()
{
	if (!myActive)
		return;
	LightDrawerSystem* lDS = myCtx->mySystemManager->GetSystem<LightDrawerSystem>(); 
	EntityID DLIndex = lDS->GetDirectionalLight();
	EntityID ALIndex = lDS->GetAmbientLight();

	ImGui::Begin("Light Tool", &myActive);
	{
		ImGui::Text("Directional Light");
		DrawDirectionalLight(DLIndex);
		
		ImGui::Separator();

		ImGui::Text("Ambiance Light");
		DrawAmbianceLight(ALIndex);
	}
	ImGui::End();

}

void ISTE::LightTool::Init(BaseEditor* aBaseEditor)
{
	myBaseEditor = aBaseEditor;
	myCtx = Context::Get();
	myToolName = "Light Tool";
}

void ISTE::LightTool::DrawDirectionalLight(EntityID aDLIndex)
{
	Scene* scene = &myCtx->mySceneHandler->GetActiveScene();
	if (aDLIndex == EntityID(-1)) 
		return;

	DirectionalLightComponent* dL = scene->GetComponent<DirectionalLightComponent>(aDLIndex);
	TransformComponent* transform = scene->GetComponent<TransformComponent>(aDLIndex);
	if (dL == nullptr)
	{
		if (ImGui::Button("Create and set DirectionalLight"))
		{
			aDLIndex = myBaseEditor->CreateEntity();
			scene->AssignComponent<DirectionalLightComponent>(aDLIndex);
			scene->AssignComponent<TransformComponent>(aDLIndex);
			myCtx->mySystemManager->GetSystem<LightDrawerSystem>()->SetDirectionalLight(aDLIndex);
			scene->SetEntityName(aDLIndex, "Directional light " + scene->GetEntityCount());
		}
		return;
	}


	float rotation[3] = { transform->myEuler.GetAngles().x, transform->myEuler.GetAngles().y, transform->myEuler.GetAngles().z };
	if (ImGui::InputFloat3("Rotation", rotation))
	{
		transform->myEuler.SetRotation(rotation[0], rotation[1], rotation[2]);
	}

	
	float* color[4] = { &dL->myColorAndIntensity.r,&dL->myColorAndIntensity.g ,&dL->myColorAndIntensity.b,  &dL->myColorAndIntensity.w };
	ImGui::DragFloat4("ColorAndIntensity", *color);
	dL->myColorAndIntensity = CU::Vec4f(*color[0], *color[1], *color[2], *color[3]);
}

void ISTE::LightTool::DrawAmbianceLight(EntityID aALIndex)
{
	if (aALIndex == EntityID(-1))
		return; 	

	Scene* scene = &myCtx->mySceneHandler->GetActiveScene(); 
	ISTE::AmbientLightComponent* ambientLight = scene->GetComponent<AmbientLightComponent>(aALIndex);
	ISTE::TransformComponent* traComp = scene->GetComponent<TransformComponent>(aALIndex);
	if (ambientLight == nullptr)
	{
		if (ImGui::Button("Create and set AmbientLight"))
		{
			aALIndex = myBaseEditor->CreateEntity();
			scene->AssignComponent<AmbientLightComponent>(aALIndex);
			scene->AssignComponent<TransformComponent>(aALIndex);
			myCtx->mySystemManager->GetSystem<LightDrawerSystem>()->SetAmbientLight(aALIndex);
			scene->SetEntityName(aALIndex, "Ambiance light " + scene->GetEntityCount());
		}

		return;
	} 
	{
		float groundColorIntensity[4] = { ambientLight->myGroundColorAndIntensity.x, ambientLight->myGroundColorAndIntensity.y, ambientLight->myGroundColorAndIntensity.z, ambientLight->myGroundColorAndIntensity.w };
		ImGui::Text("Ground Color And Intensity");
		if (ImGui::InputFloat4("##Ground Color", groundColorIntensity))
		{
			ambientLight->myGroundColorAndIntensity = { groundColorIntensity[0],groundColorIntensity[1] ,groundColorIntensity[2],groundColorIntensity[3] };
		} 
	} 
	{
		float skyColorIntensity[4] = { ambientLight->mySkyColorAndIntensity.x, ambientLight->mySkyColorAndIntensity.y, ambientLight->mySkyColorAndIntensity.z, ambientLight->mySkyColorAndIntensity.w };
		ImGui::Text("Sky Color And Intensity");
		if (ImGui::InputFloat4("##Sky Color", skyColorIntensity))
		{
			ambientLight->mySkyColorAndIntensity = { skyColorIntensity[0],skyColorIntensity[1] ,skyColorIntensity[2],skyColorIntensity[3] };
		} 

		ImGui::Text("Cube map");
		ID3D11ShaderResourceView* srv = nullptr;
		std::string name;
		if (ambientLight->myAmbianceTextureID != TextureID(-1))
		{
			auto& T = myCtx->myTextureManager->GetTexture(ambientLight->myAmbianceTextureID);
			srv = T.mySRV.Get();
			name = std::string(T.myFilePath.begin(), T.myFilePath.end());
			savedPath = std::string(T.myFilePath.begin(), T.myFilePath.end());
		}
		else
		{
			srv = nullptr;
			name = "Cubemap Not Bound";
		}

		std::wstring pathw;

		ImGui::Image(srv, { 64,64 });
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds", 0))
			{
				savedPath = *(std::string*)payload->Data;
				pathw = std::wstring(savedPath.begin(), savedPath.end());
				ambientLight->myAmbianceTextureID = Context::Get()->myTextureManager->LoadTexture(pathw, true);

			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::GetIO().MouseDoubleClicked[0] && ImGui::IsItemClicked())
		{
			myBaseEditor->GetAssetsManager().LookUp(savedPath);
		}

		ImGui::SameLine(); ImGui::Text(name.c_str());
		ImGui::DragFloat("Cube map Intensity", &ambientLight->myCubeMapIntensity, 0.1f);
		float rotation[3] = { traComp->myEuler.GetAngles().x, traComp->myEuler.GetAngles().y, traComp->myEuler.GetAngles().z };
		if (ImGui::InputFloat3("Cube map Rotation", rotation))
		{
			traComp->myEuler.SetRotation(rotation[0], rotation[1], rotation[2]);
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
}
