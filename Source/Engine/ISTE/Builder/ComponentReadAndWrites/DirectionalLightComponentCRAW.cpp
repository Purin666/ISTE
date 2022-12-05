#include "DirectionalLightComponentCRAW.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/ComponentAndSystem/DirectionalLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"
#include "ISTE/ECSB/SystemManager.h"

namespace ISTE
{
	nlohmann::json DirectionalLightComponentCRAW::Write(void* aComponent, EntityID aEntity)
	{
		nlohmann::json json;

		DirectionalLightComponent* dL = (DirectionalLightComponent*)aComponent;

		json["SetActive"] = (aEntity == Context::Get()->mySystemManager->GetSystem<LightDrawerSystem>()->GetDirectionalLight());
		json["ColorAndIntensity"]["X"] = dL->myColorAndIntensity.x;
		json["ColorAndIntensity"]["Y"] = dL->myColorAndIntensity.y;
		json["ColorAndIntensity"]["Z"] = dL->myColorAndIntensity.z;
		json["ColorAndIntensity"]["W"] = dL->myColorAndIntensity.w;

		return json;

	}
	void DirectionalLightComponentCRAW::Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson)
	{
		//DirectionalLightComponent* dL = new (aComponent) DirectionalLightComponent();
		DirectionalLightComponent* dL = (DirectionalLightComponent*)aComponent;

		dL->myColorAndIntensity = CU::Vec4f(aJson["ColorAndIntensity"]["X"], aJson["ColorAndIntensity"]["Y"], aJson["ColorAndIntensity"]["Z"], aJson["ColorAndIntensity"]["W"]);

		if (aJson.contains("SetActive"))
		{
			bool isActiveEntity = aJson["SetActive"];

			if (isActiveEntity)
			{
				Context::Get()->mySystemManager->GetSystem<LightDrawerSystem>()->SetDirectionalLight(aEntity);
			}

		}

	}
}