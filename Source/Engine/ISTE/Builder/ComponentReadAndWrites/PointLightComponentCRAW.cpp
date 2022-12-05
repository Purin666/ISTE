#include "PointLightComponentCRAW.h"

#include "ISTE/Graphics/ComponentAndSystem/PointLightComponent.h"

namespace ISTE
{
	nlohmann::json PointLightComponentCRAW::Write(void* aComponent, EntityID)
	{

		PointLightComponent* cmp = (PointLightComponent*)aComponent;

		nlohmann::json json;

		json["ColorAndIntensity"]["X"] = cmp->myColorAndIntensity.x;
		json["ColorAndIntensity"]["Y"] = cmp->myColorAndIntensity.y;
		json["ColorAndIntensity"]["Z"] = cmp->myColorAndIntensity.z;
		json["ColorAndIntensity"]["W"] = cmp->myColorAndIntensity.w;

		json["Radius"] = cmp->myRadius;

		//json["ShadowCastingFlag"] = cmp->myShadowCastingFlag;

		return json;
	}
	void PointLightComponentCRAW::Read(void* aComponent, EntityID, nlohmann::json& aJson)
	{
		PointLightComponent* cmp = new (aComponent) PointLightComponent();

		cmp->myColorAndIntensity = CU::Vec4f((float)aJson["ColorAndIntensity"]["X"], (float)aJson["ColorAndIntensity"]["Y"], (float)aJson["ColorAndIntensity"]["Z"], (float)aJson["ColorAndIntensity"]["W"]);
		cmp->myRadius = aJson["Radius"];
		//cmp->myShadowCastingFlag = aJson["ShadowCastingFlag"];

	}
}
