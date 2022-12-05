#include "ShadowPointLightComponentCRAW.h"
#include "ISTE/Graphics/ComponentAndSystem/ShadowCastingPointLightComponent.h"

namespace ISTE
{
	nlohmann::json ShadowPointLightComponentCRAW::Write(void* aComponent, EntityID)
	{

		ShadowCastingPointLightComponent* cmp = (ShadowCastingPointLightComponent*)aComponent;

		nlohmann::json json;

		json["ColorAndIntensity"]["X"] = cmp->myColorAndIntensity.x;
		json["ColorAndIntensity"]["Y"] = cmp->myColorAndIntensity.y;
		json["ColorAndIntensity"]["Z"] = cmp->myColorAndIntensity.z;
		json["ColorAndIntensity"]["W"] = cmp->myColorAndIntensity.w;

		json["Radius"] = cmp->myRadius; 

		return json;
	}
	void ShadowPointLightComponentCRAW::Read(void* aComponent, EntityID, nlohmann::json& aJson)
	{
		//ShadowCastingPointLightComponent* cmp = new (aComponent) ShadowCastingPointLightComponent();
		ShadowCastingPointLightComponent* cmp = (ShadowCastingPointLightComponent*)aComponent;

		cmp->myColorAndIntensity = CU::Vec4f((float)aJson["ColorAndIntensity"]["X"], (float)aJson["ColorAndIntensity"]["Y"], (float)aJson["ColorAndIntensity"]["Z"], (float)aJson["ColorAndIntensity"]["W"]);
		cmp->myRadius = aJson["Radius"]; 

	}
}
