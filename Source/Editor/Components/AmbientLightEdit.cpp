#include "AmbientLightEdit.h"

#include "ISTE/Context.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/GraphicsEngine.h" 
#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/AmbientLightComponent.h"
#include "Commands/CommandManager.h"
#include "Commands/VariableCommand.h"
#include "Commands/CommandFunctions.h"
//#include "ISTE/Math/Vec4.h"


#include "ISTE/Helper/StringCast.h"
#include "ISTE/CU/MemTrack.hpp"

void ISTE::AmbientLightEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	ISTE::AmbientLightComponent* ambientLight = aScenePointer->GetComponent<ISTE::AmbientLightComponent>(anEntity.myId);
	if (ambientLight == nullptr)
	{
		return;
	}
	ImGui::Text("Ambient Light Component");
	{
		float groundColorIntensity[4] = { ambientLight->myGroundColorAndIntensity.x, ambientLight->myGroundColorAndIntensity.y, ambientLight->myGroundColorAndIntensity.z, ambientLight->myGroundColorAndIntensity.w };
		ImGui::Text("Ground Color And Intensity");
		if (ImGui::InputFloat4("##Ground Color", groundColorIntensity))
		{
			ambientLight->myGroundColorAndIntensity = { groundColorIntensity[0],groundColorIntensity[1] ,groundColorIntensity[2],groundColorIntensity[3] };
		}
		CF::CheckEditing<CU::Vec4f>(myOriginGroundColorIntensity, &ambientLight->myGroundColorAndIntensity, myIsEditing);
	}
	ImGui::Text("Ambient Light Component");
	{
		float skyColorIntensity[4] = { ambientLight->mySkyColorAndIntensity.x, ambientLight->mySkyColorAndIntensity.y, ambientLight->mySkyColorAndIntensity.z, ambientLight->mySkyColorAndIntensity.w };
		ImGui::Text("Sky Color And Intensity");
		if (ImGui::InputFloat4("##Sky Color", skyColorIntensity))
		{
			ambientLight->mySkyColorAndIntensity = { skyColorIntensity[0],skyColorIntensity[1] ,skyColorIntensity[2],skyColorIntensity[3] };
		}
		CF::CheckEditing<CU::Vec4f>(myOriginSkyColorIntensity, &ambientLight->mySkyColorAndIntensity, myIsEditing2);
	}

	char input[127] = { 0 };
	if (ImGui::InputText("Cubemap path", input, 127, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	{
		ambientLight->myAmbianceTextureID = Context::Get()->myTextureManager->LoadTexture(StringCast<std::wstring>(input).c_str());
	}
	std::string id = "Texture id" + std::to_string(ambientLight->myAmbianceTextureID);
	ImGui::Text(id.c_str());
	DragDrop(ambientLight);
	if (ImGui::DragFloat("Cubemap intensity", &myOriginalCubeMapIntensity, 0.1f))
	{
		ambientLight->myCubeMapIntensity = myOriginalCubeMapIntensity;
	}
	CF::CheckEditing<float>(myOriginalCubeMapIntensity, &ambientLight->myCubeMapIntensity, myIsEditing3);

	if (ImGui::Button("Set active ##AmbienLight"))
	{
		Context::Get()->mySystemManager->GetSystem<ISTE::LightDrawerSystem>()->SetAmbientLight(anEntity.myId);
	}
	ImGui::Spacing();
	ImGui::Separator();
}
void ISTE::AmbientLightEdit::DragDrop(ISTE::AmbientLightComponent* anAmbientComponent)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds", 0))
		{
			TextureManager* tM = Context::Get()->myTextureManager;
			std::string path = *(std::string*)payload->Data;
			std::wstring wpath = std::wstring(path.begin(), path.end());
			anAmbientComponent->myAmbianceTextureID = tM->LoadTexture(wpath).myValue;
		}
		ImGui::EndDragDropTarget();
	}
}