#include "SpotLightEdit.h"

#include "imgui/imgui.h"
#include "Commands/CommandFunctions.h"

#include "ISTE/Graphics/ComponentAndSystem/SpotLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ShadowCastingSpotLightComponent.h"

namespace ISTE
{
	void SpotLightEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
	{
		SpotLightCompEdit(aScenePointer, anEntity);
		ShadowCastingSpotLightCompEdit(aScenePointer, anEntity);
	}
	void SpotLightEdit::SpotLightCompEdit(ISTE::Scene* aScenePointer, Entity anEntity)
	{
		SpotLightComponent* sL = aScenePointer->GetComponent<ISTE::SpotLightComponent>(anEntity.myId);
		if (sL == nullptr)
		{
			return;
		}

		ImGui::Text("SpotLight");
		{
			float* color[4] = { &sL->myColorAndIntensity.r,&sL->myColorAndIntensity.g ,&sL->myColorAndIntensity.b,  &sL->myColorAndIntensity.w};
			ImGui::InputFloat4("ColorAndIntensity ##spotlight", *color);
			sL->myColorAndIntensity = CU::Vec4f(*color[0], *color[1], *color[2], *color[3]);
			CF::CheckEditing<CU::Vec4f>(myOriginalColour, &sL->myColorAndIntensity, myIsEditing);
		}
		{
			ImGui::InputFloat("OuterAngle ##spotlight", &sL->myOuterAngle);
			CF::CheckEditing<float>(myOGOuterAngle, &sL->myOuterAngle, myIsEditing2);
		}
		{
			ImGui::InputFloat("InnerAngle ##spotlight", &sL->myInnerAngle);
			CF::CheckEditing<float>(myOGInnerAngle, &sL->myInnerAngle, myIsEditing2);
		}
		{
			ImGui::InputFloat("Range ##spotlight", &sL->myRange);
			CF::CheckEditing<float>(myOGRange, &sL->myRange, myIsEditing3);
		}
		ImGui::Separator();
	}
	void SpotLightEdit::ShadowCastingSpotLightCompEdit(ISTE::Scene* aScenePointer, Entity anEntity)
	{
		ShadowCastingSpotLightComponent* sL = aScenePointer->GetComponent<ShadowCastingSpotLightComponent>(anEntity.myId);
		if (sL == nullptr)
		{
			return;
		}

		ImGui::Text("Shadowcasting Spotlight");
		{
			float* color[4] = { &sL->myColorAndIntensity.r,&sL->myColorAndIntensity.g ,&sL->myColorAndIntensity.b,  &sL->myColorAndIntensity.w };
			ImGui::InputFloat4("ColorAndIntensity", *color);
			sL->myColorAndIntensity = CU::Vec4f(*color[0], *color[1], *color[2], *color[3]);
			CF::CheckEditing<CU::Vec4f>(myOriginalColour, &sL->myColorAndIntensity, myIsEditing);
		}
		{
			ImGui::InputFloat("OuterAngle ##shadowSpotlight", &sL->myOuterAngle);
			CF::CheckEditing<float>(myOGOuterAngle, &sL->myOuterAngle, myIsEditing2);
		}
		{
			ImGui::InputFloat("InnerAngle ##shadowSpotlight", &sL->myInnerAngle);
			CF::CheckEditing<float>(myOGInnerAngle, &sL->myInnerAngle, myIsEditing2);
		}
		{
			ImGui::InputFloat("Range ##shadowSpotlight", &sL->myRange);
			CF::CheckEditing<float>(myOGRange, &sL->myRange, myIsEditing3);
		}
		ImGui::Separator();
	}
}