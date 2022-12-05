#include "SpotLightComponentCRAW.h"

#include "ISTE/Graphics/ComponentAndSystem/SpotLightComponent.h"

namespace ISTE
{
	nlohmann::json SpotLightComponentCRAW::Write(void* aComponent, EntityID)
	{
		SpotLightComponent* cmp = (SpotLightComponent*)aComponent;

		nlohmann::json json;

		json["ColorAndIntensity"]["X"] = cmp->myColorAndIntensity.x;
		json["ColorAndIntensity"]["Y"] = cmp->myColorAndIntensity.y;
		json["ColorAndIntensity"]["Z"] = cmp->myColorAndIntensity.z;
		json["ColorAndIntensity"]["W"] = cmp->myColorAndIntensity.w;

		json["Range"] = cmp->myRange;
		json["OuterAngle"] = cmp->myOuterAngle;
		json["InnerAngle"] = cmp->myInnerAngle;
		json["ShadowCastingFlag"] = cmp->myShadowCastingFlag;


		return json;
	}
	void SpotLightComponentCRAW::Read(void* aComponent, EntityID, nlohmann::json& aJson)
	{
		SpotLightComponent* cmp = new (aComponent) SpotLightComponent();

		cmp->myColorAndIntensity = CU::Vec4f((float)aJson["ColorAndIntensity"]["X"], (float)aJson["ColorAndIntensity"]["Y"], (float)aJson["ColorAndIntensity"]["Z"], (float)aJson["ColorAndIntensity"]["W"]);
		cmp->myRange = aJson["Range"];
		cmp->myOuterAngle = aJson["OuterAngle"];
		cmp->myInnerAngle = aJson["InnerAngle"];
		cmp->myShadowCastingFlag = aJson["ShadowCastingFlag"];
	}
}