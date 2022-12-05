#include "DirectionalLightEdit.h"

#include "imgui/imgui.h"
#include "Commands/CommandFunctions.h"

#include "ISTE/Graphics/ComponentAndSystem/DirectionalLightComponent.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"

namespace ISTE
{
	void DirectionalLightEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
	{
		DirectionalLightComponent* dL = aScenePointer->GetComponent<DirectionalLightComponent>(anEntity.myId);
		if (dL == nullptr)
		{
			return;
		}

		ImGui::Text("DirectionalLight");
		{
			float* color[4] = { &dL->myColorAndIntensity.r,&dL->myColorAndIntensity.g ,&dL->myColorAndIntensity.b,  &dL->myColorAndIntensity.w };
			ImGui::InputFloat4("ColorAndIntensity", *color);
			dL->myColorAndIntensity = CU::Vec4f(*color[0], *color[1], *color[2], *color[3]);
			CF::CheckEditing<CU::Vec4f>(myOriginalColour, &dL->myColorAndIntensity, myIsEditing);
		}

		if (ImGui::Button("SetDirectionalLight"))
		{
			Context::Get()->mySystemManager->GetSystem<ISTE::LightDrawerSystem>()->SetDirectionalLight(anEntity.myId);
		}

		ImGui::Separator();
	}
}