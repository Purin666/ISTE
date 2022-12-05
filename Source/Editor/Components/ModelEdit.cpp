#include "ModelEdit.h"
#include "imgui/imgui.h"
#include "Commands/CommandFunctions.h"
#include "ComponentFunctions.h"
#include "ISTE/Graphics/Resources/ModelManager.h"

void ISTE::ModelEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	ISTE::ModelComponent* model = aScenePointer->GetComponent<ISTE::ModelComponent>(anEntity.myId);
	if (model == nullptr)
	{
		return;
	}
	ImGui::Text("Model");
	{
		float* color[3] = { &model->myColor.r,&model->myColor.g ,&model->myColor.b };
		ImGui::InputFloat3("Color", *color);
		CF::CheckEditing<CU::Vec3f>(myOriginalColour, &model->myColor, myIsEditing);
	}
	{
		float* uv[2] = { &model->myUV.x,&model->myUV.y };
		ImGui::InputFloat2("UV", *uv);
		CF::CheckEditing<CU::Vec2f>(myOriginalUV, &model->myUV, myIsEditing2);
	}
	{
		float* uvScale[2] = { &model->myUVScale.x,&model->myUVScale.y };
		ImGui::InputFloat2("UV scale", *uvScale);
		CF::CheckEditing<CU::Vec2f>(myOriginalUVScale, &model->myUV, myIsEditing3);
	}
	{
		ImGui::Text("Model ID:");
		DragDrop(model);
		ImGui::SameLine();
		ImGui::Text(std::to_string(model->myModelId).c_str());
	}
	{
		std::string correlatingSamplers[4] = { "Point", "Bilinear", "Trilinear","Anisotropic" };
		ISTE::SamplerState possibleSamplers[4] = { ISTE::SamplerState::ePoint,ISTE::SamplerState::eBiliniear,ISTE::SamplerState::eTriLinear,ISTE::SamplerState::eAnisotropic };
		EF::SetList<ISTE::SamplerState>((int)model->mySamplerState, model->mySamplerState, (std::string)"Sampler State", correlatingSamplers, possibleSamplers);
	}
	{
		std::string correlatingAdressModes[3] = { "Clamp", "Mirror", "Wrap" };
		ISTE::AdressMode possibleAdressModes[3] = { ISTE::AdressMode::eClamp,ISTE::AdressMode::eMirror,ISTE::AdressMode::eWrap };
		EF::SetList<ISTE::AdressMode>((int)model->myAdressMode, model->myAdressMode, (std::string)"Adress Mode", correlatingAdressModes, possibleAdressModes);
	}

	ImGui::Separator();
}

void ISTE::ModelEdit::DragDrop(ISTE::ModelComponent* aModelComponent)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".fbx", 0))
		{
			ModelManager* mM = Context::Get()->myModelManager;
			std::string path = *(std::string*)payload->Data;
			ModelManager::ModelLoadResult mLR = mM->LoadModel(path);
			aModelComponent->myModelId = mLR.myValue;
		}
		ImGui::EndDragDropTarget();
	}
}
