#include "EmitterComponentCRAW.h"

#include "ISTE/Context.h"

#include "ISTE/VFX/SpriteParticles/EmitterComponent.h"

nlohmann::json ISTE::EmitterComponentCRAW::Write(void* aComponent, EntityID aEntity)
{
	EmitterComponent* e = (EmitterComponent*)aComponent;
	nlohmann::json json;

	json["myEmitterType"] = e->myEmitterType;

	return json;
}

void ISTE::EmitterComponentCRAW::Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson)
{
	EmitterComponent* e = (EmitterComponent*)aComponent;

	e->myEmitterType = aJson["myEmitterType"];
}
