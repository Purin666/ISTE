#include "ISTE/VFX/VFXHandler.h"

#include "ISTE/Context.h"

#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"
#include "ISTE/VFX/ModelVFX/ModelVFXHandler.h"

#include "ISTE/VFX/VFXUtilityFunctions.h"

#include "ISTE/CU/MemTrack.hpp"

const ISTE::VFXID ISTE::VFXHandler::SpawnVFX(const std::string& aName, const CU::Vec3f aPosition, const CU::Vec3f aRotation)
{
	auto itr = mySystem->myDatas.find(aName);
	if (itr == mySystem->myDatas.end())
	{
		std::cout << "Error in " << __func__ << ": No emitter type with name " << aName << std::endl;
		return 0;
	}
	int id = ++mySystem->myLatestVFXId;

	auto& data = mySystem->myDatas[aName];
	auto& vfx = mySystem->myPackages[id];

	auto mHandler = Context::Get()->myModelVFXHandler;
	auto eHandler = Context::Get()->mySprite3DParticleHandler;
	
	// set data
	vfx.myTypeName = aName;
	vfx.myLifetime = Rand(data.myLifetime);
	vfx.myDelay = Rand(data.myDelay);
	vfx.myFirstDelay = Rand(data.myFirstDelay);
	vfx.myPosition = aPosition;
	vfx.myRotation = aRotation;

	// spawn emitters
	for (auto& name : data.myEmitterTypes)
	{
		vfx.myEmitterIds.push_back(
			eHandler->SpawnEmitter(name, aPosition, aRotation));
	}
	// spawn models
	for (auto& name : data.myModelVFXTypes)
	{
		vfx.myModelVFXIds.push_back(
			mHandler->Spawn(name, aPosition, aRotation));
	}
	return id;
}

void ISTE::VFXHandler::SetVFXPosition(const VFXID anId, const CU::Vec3f aPosition)
{
	auto itr = mySystem->myPackages.find(anId);
	if (itr != mySystem->myPackages.end())
	{
		auto& vfx = mySystem->myPackages[anId];
		vfx.myPosition = aPosition;
		for (auto& id : vfx.myEmitterIds)
			Context::Get()->mySprite3DParticleHandler->SetEmitterPosition(id, aPosition);
		for (auto& id : vfx.myModelVFXIds)
			Context::Get()->myModelVFXHandler->SetPosition(id, aPosition);
	}
}

void ISTE::VFXHandler::SetVFXRotation(const VFXID anId, const CU::Vec3f aRotation)
{
	auto itr = mySystem->myPackages.find(anId);
	if (itr != mySystem->myPackages.end())
	{
		auto& vfx = mySystem->myPackages[anId];
		vfx.myRotation = aRotation;
		for (auto& id : vfx.myEmitterIds)
			Context::Get()->mySprite3DParticleHandler->SetEmitterRotation(id, aRotation);
		for (auto& id : vfx.myModelVFXIds)
			Context::Get()->myModelVFXHandler->SetRotation(id, aRotation);
	}
}

void ISTE::VFXHandler::DeactivateVFX(const VFXID anId)
{
	auto itr = mySystem->myPackages.find(anId);
	if (itr != mySystem->myPackages.end())
	{
		auto& vfx = mySystem->myPackages[anId];
		for (auto& id : vfx.myEmitterIds)
			Context::Get()->mySprite3DParticleHandler->DeactivateEmitter(id);
		for (auto& id : vfx.myModelVFXIds)
			Context::Get()->myModelVFXHandler->Kill(id);
	}
}

void ISTE::VFXHandler::KillVFX(const VFXID anId)
{
	auto itr = mySystem->myPackages.find(anId);
	if (itr != mySystem->myPackages.end())
	{
		auto& vfx = mySystem->myPackages[anId];
		for (auto& id : vfx.myEmitterIds)
			Context::Get()->mySprite3DParticleHandler->DeactivateEmitter(id);
		for (auto& id : vfx.myModelVFXIds)
			Context::Get()->myModelVFXHandler->Kill(id);

		mySystem->myPackages.erase(itr);
	}
}

void ISTE::VFXHandler::KillAllVFX()
{
	for (auto& [id, vfx] : mySystem->myPackages)
	{
		for (auto& id : vfx.myEmitterIds)
			Context::Get()->mySprite3DParticleHandler->KillEmitter(id);
		for (auto& id : vfx.myModelVFXIds)
			Context::Get()->myModelVFXHandler->Kill(id);
	}
	mySystem->myPackages.clear();
}


void ISTE::VFXHandler::Update(const float aDeltaTime)
{
	if (myPauseFlag)
		return;

	if (mySystem->myPackages.empty())
		return;

	auto mHandler = Context::Get()->myModelVFXHandler;
	auto eHandler = Context::Get()->mySprite3DParticleHandler;
	for (auto& [id, vfx] : mySystem->myPackages)
	{
		if ((vfx.myRelativeTime += aDeltaTime) > vfx.myLifetime)
		{
			auto& data = mySystem->myDatas[vfx.myTypeName];
			if (data.myShouldLoop)
			{
				vfx.myRelativeTime = 0.f;
				// TODO:: Rework to NOT kill all and respawn them //Mathias
				
				// Kill emitters and models
				for (const int id : vfx.myEmitterIds)
					eHandler->KillEmitter(id);
				for (const EntityID id : vfx.myModelVFXIds)
					mHandler->Kill(id);
				vfx.myModelVFXIds.clear();
				vfx.myEmitterIds.clear();

				// Spawn emitters and models
				for (auto& name : data.myEmitterTypes)
				{
					vfx.myEmitterIds.push_back(
						eHandler->SpawnEmitter(name, vfx.myPosition, vfx.myRotation));
				}
				for (auto& name : data.myModelVFXTypes)
				{
					vfx.myModelVFXIds.push_back(
						mHandler->Spawn(name, vfx.myPosition, vfx.myRotation));
				}
			}
			else
				KillVFX(id);
			break;
		}
	}
}

void ISTE::VFXHandler::CreateRenderCommands()
{
}

void ISTE::VFXHandler::LoadModelsAndTextures()
{
}
