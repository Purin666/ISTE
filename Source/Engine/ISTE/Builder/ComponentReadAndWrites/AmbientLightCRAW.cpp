#include "AmbientLightCRAW.h"

#include "ISTE/Context.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/AmbientLightComponent.h"
#include "ISTE/Helper/StringCast.h"

nlohmann::json ISTE::AmbientLightCRAW::Write(void* aComponent, EntityID aEntity)
{
	nlohmann::json json;

	AmbientLightComponent* aL = (AmbientLightComponent*)aComponent;

	json["SetActive"] = (aEntity == Context::Get()->mySystemManager->GetSystem<LightDrawerSystem>()->GetAmbientLight());
	json["GroundColorAndIntensity"]["X"] = aL->myGroundColorAndIntensity.x;
	json["GroundColorAndIntensity"]["Y"] = aL->myGroundColorAndIntensity.y;
	json["GroundColorAndIntensity"]["Z"] = aL->myGroundColorAndIntensity.z;
	json["GroundColorAndIntensity"]["W"] = aL->myGroundColorAndIntensity.w;

	json["SkyColorAndIntensity"]["X"] = aL->mySkyColorAndIntensity.x;
	json["SkyColorAndIntensity"]["Y"] = aL->mySkyColorAndIntensity.y;
	json["SkyColorAndIntensity"]["Z"] = aL->mySkyColorAndIntensity.z;
	json["SkyColorAndIntensity"]["W"] = aL->mySkyColorAndIntensity.w;

	json["AmbientIntensity"] = aL->myCubeMapIntensity;
	
	if (aL->myAmbianceTextureID != TextureID(-1))
	{
		std::wstring tPath = Context::Get()->myTextureManager->GetTexture(aL->myAmbianceTextureID).myFilePath;
		std::string path(tPath.begin(), tPath.end());
		json["CubemapFilePath"] = path;
	}
	else
		json["CubemapFilePath"] = "";

	return json;

}

void ISTE::AmbientLightCRAW::Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson)
{
	AmbientLightComponent* aL = new (aComponent) AmbientLightComponent();

	aL->myGroundColorAndIntensity = CU::Vec4f(aJson["GroundColorAndIntensity"]["X"], aJson["GroundColorAndIntensity"]["Y"], aJson["GroundColorAndIntensity"]["Z"], aJson["GroundColorAndIntensity"]["W"]);
	aL->mySkyColorAndIntensity = CU::Vec4f(aJson["SkyColorAndIntensity"]["X"], aJson["SkyColorAndIntensity"]["Y"], aJson["SkyColorAndIntensity"]["Z"], aJson["SkyColorAndIntensity"]["W"]);
	aL->myCubeMapIntensity = aJson["AmbientIntensity"];

	std::string path = aJson["CubemapFilePath"].get<std::string>();
	if (!path.empty())
	{
		aL->myAmbianceTextureID = Context::Get()->myTextureManager->LoadTexture(StringCast<std::wstring>(path).c_str());
	}

	if (aJson.contains("SetActive"))
	{
		bool isActiveEntity = aJson["SetActive"];

		if (isActiveEntity)
		{
			Context::Get()->mySystemManager->GetSystem<LightDrawerSystem>()->SetAmbientLight(aEntity);
		}

	}

}
