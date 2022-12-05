#include "PointLightEdit.h"

#include "imgui/imgui.h"
#include "Commands/CommandFunctions.h"

#include "ISTE/Graphics/ComponentAndSystem/PointLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ShadowCastingPointLightComponent.h"

namespace ISTE
{
	void PointLightEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
	{
		PointLightCompEdit(aScenePointer, anEntity);
		ShadowCastingPointLightCompEdit(aScenePointer, anEntity);
	}
	void PointLightEdit::PointLightCompEdit(ISTE::Scene* aScenePointer, Entity anEntity)
	{
		PointLightComponent* sL = aScenePointer->GetComponent<PointLightComponent>(anEntity.myId);

		if (sL == nullptr)
		{
			return;
		}

		ImGui::Text("PointLight");
		{
			float* color[4] = { &sL->myColorAndIntensity.r,&sL->myColorAndIntensity.g ,&sL->myColorAndIntensity.b,  &sL->myColorAndIntensity.w };
			ImGui::InputFloat4("ColorAndIntensity", *color);
			sL->myColorAndIntensity = CU::Vec4f(*color[0], *color[1], *color[2], *color[3]);
			CF::CheckEditing<CU::Vec4f>(myOriginalColour, &sL->myColorAndIntensity, myIsEditing);
		}
		{
			ImGui::InputFloat("Radius", &sL->myRadius);
			CF::CheckEditing<float>(myOGRadius, &sL->myRadius, myIsEditing1);
		}
		ImGui::Separator();
	}
	void PointLightEdit::ShadowCastingPointLightCompEdit(ISTE::Scene* aScenePointer, Entity anEntity)
	{
		ShadowCastingPointLightComponent* sL = aScenePointer->GetComponent<ShadowCastingPointLightComponent>(anEntity.myId);

		if (sL == nullptr)
		{
			return;
		}

		ImGui::Text("Shadowcasting Pointlight");
		{
			float* color[4] = { &sL->myColorAndIntensity.r,&sL->myColorAndIntensity.g ,&sL->myColorAndIntensity.b,  &sL->myColorAndIntensity.w };
			ImGui::InputFloat4("ColorAndIntensity", *color);
			sL->myColorAndIntensity = CU::Vec4f(*color[0], *color[1], *color[2], *color[3]);
			CF::CheckEditing<CU::Vec4f>(myOriginalColour, &sL->myColorAndIntensity, myIsEditing);
		}
		{
			ImGui::InputFloat("Radius", &sL->myRadius);
			CF::CheckEditing<float>(myOGRadius, &sL->myRadius, myIsEditing1);
		}
		ImGui::Separator();
	}
}