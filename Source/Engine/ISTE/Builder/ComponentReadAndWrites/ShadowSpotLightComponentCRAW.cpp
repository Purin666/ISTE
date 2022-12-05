#include "ShadowSpotLightComponentCRAW.h"
#include "ISTE/Graphics/ComponentAndSystem/ShadowCastingSpotLightComponent.h"

namespace ISTE
{
	nlohmann::json ShadowSpotLightComponentCRAW::Write(void* aComponent, EntityID)
	{
		ShadowCastingSpotLightComponent* cmp = (ShadowCastingSpotLightComponent*)aComponent;

		nlohmann::json json;

		json["ColorAndIntensity"]["X"] = cmp->myColorAndIntensity.x;
		json["ColorAndIntensity"]["Y"] = cmp->myColorAndIntensity.y;
		json["ColorAndIntensity"]["Z"] = cmp->myColorAndIntensity.z;
		json["ColorAndIntensity"]["W"] = cmp->myColorAndIntensity.w;

		json["Range"] = cmp->myRange;
		json["OuterAngle"] = cmp->myOuterAngle;
		json["InnerAngle"] = cmp->myInnerAngle; 


		return json;
	}
	void ShadowSpotLightComponentCRAW::Read(void* aComponent, EntityID, nlohmann::json& aJson)
	{
		//ShadowCastingSpotLightComponent* cmp = new (aComponent) ShadowCastingSpotLightComponent();
		ShadowCastingSpotLightComponent* cmp = (ShadowCastingSpotLightComponent*)aComponent;

		cmp->myColorAndIntensity = CU::Vec4f((float)aJson["ColorAndIntensity"]["X"], (float)aJson["ColorAndIntensity"]["Y"], (float)aJson["ColorAndIntensity"]["Z"], (float)aJson["ColorAndIntensity"]["W"]);
		cmp->myRange = aJson["Range"];
		cmp->myOuterAngle = aJson["OuterAngle"];
		cmp->myInnerAngle = aJson["InnerAngle"]; 
	}
}