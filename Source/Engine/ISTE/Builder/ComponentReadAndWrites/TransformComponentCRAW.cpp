#include "TransformComponentCRAW.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include "ISTE/Context.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformSystem.h"

nlohmann::json ISTE::TransformComponentCRAW::Write(void* aComponent, EntityID)
{
	TransformComponent* transform = (TransformComponent*)aComponent;

	nlohmann::json transformJson;

	transformJson["Position"]["X"] = transform->myPosition.x;
	transformJson["Position"]["Y"] = transform->myPosition.y;
	transformJson["Position"]["Z"] = transform->myPosition.z;

	CU::Vec3f rot = transform->myEuler.GetAngles();

	transformJson["Rotation"]["X"] = rot.x;
	transformJson["Rotation"]["Y"] = rot.y;
	transformJson["Rotation"]["Z"] = rot.z;

	transformJson["Scale"]["X"] = transform->myScale.x;
	transformJson["Scale"]["Y"] = transform->myScale.y;
	transformJson["Scale"]["Z"] = transform->myScale.z;

	return transformJson;
}

void ISTE::TransformComponentCRAW::Read(void* aComponent, EntityID, nlohmann::json& aJson)
{
	TransformComponent* transform = new (aComponent) TransformComponent();

	transform->myPosition.x = aJson["Position"]["X"];
	transform->myPosition.y = aJson["Position"]["Y"];
	transform->myPosition.z = aJson["Position"]["Z"];

	CU::Vec3f rot;

	rot.x = aJson["Rotation"]["X"];
	rot.y = aJson["Rotation"]["Y"];
	rot.z = aJson["Rotation"]["Z"];

	transform->myEuler.SetRotation(rot);

	transform->myScale.x = aJson["Scale"]["X"];
	transform->myScale.y = aJson["Scale"]["Y"];
	transform->myScale.z = aJson["Scale"]["Z"];
}
