#include "TransformEdit.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "Commands/CommandFunctions.h"


void ISTE::TransformEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	//ImGui::Text(anEntity.myName.c_str());
	//ImGui::InputText("##EntityName", &anEntity.myName);
	//Context::Get()->mySceneHandler->GetActiveScene().SetEntityName(anEntity.myId, anEntity.myName);

	ISTE::TransformComponent* transform = aScenePointer->GetComponent<ISTE::TransformComponent>(anEntity.myId);
	if (transform == nullptr)
	{
		return;
	}
	ImGui::Text("Transform");
	{
		float position[3] = { transform->myPosition.x, transform->myPosition.y, transform->myPosition.z };

		if (ImGui::InputFloat3("Position", position))
		{
			transform->myPosition = { position[0],position[1] ,position[2] };
		}
		CF::CheckEditing<CU::Vec3f>(myOriginPos, &transform->myPosition, myIsEditing);
	}
	{
		float rotation[3] = { transform->myEuler.GetAngles().x, transform->myEuler.GetAngles().y, transform->myEuler.GetAngles().z };
		if (ImGui::InputFloat3("Rotation", rotation))
		{
			transform->myEuler.SetRotation(rotation[0], rotation[1], rotation[2]);

		}
		CF::CheckEditing<CU::Euler>(myOriginRot, &transform->myEuler, myIsEditing2);
	}
	{
		float size[3] = { transform->myScale.x, transform->myScale.y, transform->myScale.z };

		if (ImGui::InputFloat3("Size", size))
		{
			transform->myScale = { size[0],size[1] ,size[2] };
		}
		CF::CheckEditing<CU::Vec3f>(myOriginSize, &transform->myScale, myIsEditing3);
	}
	ImGui::Separator();
}
