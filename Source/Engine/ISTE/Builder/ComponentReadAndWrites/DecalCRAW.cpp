#include "DecalCRAW.h"

#include "ISTE/Context.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/DecalComponent.h"
#include "ISTE/Helper/StringCast.h"
nlohmann::json ISTE::DecalComponentCRAW::Write(void* aComponent, EntityID aEntity)
{
	DecalComponent* dc = (DecalComponent*)aComponent;
	nlohmann::json json;

	json["myLerpValues"]["X"]	= dc->myLerpValues.x;
	json["myLerpValues"]["Y"]	= dc->myLerpValues.y;
	json["myLerpValues"]["Z"]	= dc->myLerpValues.z;
	json["AngleThreshold"]		= dc->myAngleThreshold;
	json["Flag"]				= dc->myCoverageFlag;
	return json;
}

void ISTE::DecalComponentCRAW::Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson)
{
	DecalComponent* dc = (DecalComponent*)aComponent;
	dc->myLerpValues.x		= aJson["myLerpValues"]["X"];
	dc->myLerpValues.y		= aJson["myLerpValues"]["Y"];
	dc->myLerpValues.z		= aJson["myLerpValues"]["Z"];
	dc->myAngleThreshold	= aJson["AngleThreshold"];
	dc->myCoverageFlag		= aJson["Flag"]; 
}