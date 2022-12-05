
#include "Sprite3DParticleHandler.h"
#include "Sprite3DParticleDefines.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/ComponentAndSystem/VFXSpriteDrawerSystem.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/Math/Matrix4x4.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/VFXSprite3DCommand.h"

#include "ISTE/VFX/VFXUtilityFunctions.h"
#include "ISTE/Math/Math.h"

#include "ISTE/CU/MemTrack.hpp"

namespace
{
	ISTE::Particle3D SpawnParticle(const int aParticleId, ISTE::Emitter3D& anEmitter, ISTE::Emitter3DData& aType)
	{
		auto startPosition = Rand(aType.myInitialBoxPosition); // offset in box
		startPosition += ISTE::RandPointInsideUnitSphere() * Rand(aType.myInitialSpherePosition); // offset in sphere
		if (!aType.myUseLocalSpace) startPosition += anEmitter.myPosition;

		return {
			aParticleId,

			0.f,
			Rand(aType.myLifeTime),
			Rand(aType.myInitialRotation),
			0.f, // rotation rate
			Rand(aType.myMass),
			Rand(aType.myResistance),

			aType.myFaceDirection,
			startPosition,
			Rand(aType.myInitialSize),
			Rand(aType.myInitialVelocity),
			Rand(aType.myConstAcceleration)
		};
	}

	void UpdateParticles(std::vector<ISTE::Particle3D>& someParticles, float aDeltaTime)
	{
		for (size_t i = 0; i < someParticles.size();)
		{
			ISTE::Particle3D& p = someParticles[i];

			if ((p.myRelativeTime += aDeltaTime) >= p.myLifetime)
			{
				// destroy particle
				p = someParticles.back();
				someParticles.pop_back();
			}
			else
			{
				// update particle
				p.myPosition += p.myVelocity * aDeltaTime;

				p.myRotation += p.myRotationRate * aDeltaTime;

				p.myVelocity += p.myConstAcceleration * aDeltaTime;
				p.myVelocity.y += p.myMass * ISTE::Gravity * aDeltaTime;
				p.myVelocity *= 1.f - p.myResistance * aDeltaTime;

				++i;
			}
		}
	}
}

const int ISTE::Sprite3DParticleHandler::SpawnEmitter(const std::string& aName, const CU::Vec3f aPosition, const CU::Vec3f someEulerAngles)
{
	auto itr = mySystem->myEmitterDatas.find(aName);
	if (itr == mySystem->myEmitterDatas.end())
	{
		std::cout << "Error in " << __func__ << ": No emitter type with name " << aName << std::endl;
		return 0;
	}
	int id = ++mySystem->myLatestEmitterId;
	auto& emitter = mySystem->myEmitters[id];
	emitter = { aName, aPosition, someEulerAngles }; // remaining members will be default initialized
	emitter.myParticles.reserve(MAX_PARTICLES_PER_EMITTER);

	auto& data = mySystem->myEmitterDatas[aName];
	auto& particles = emitter.myParticles;
	const size_t spawnAmount = (size_t)Rand(data.myInitialSpawnAmount);
	for (size_t i = 0; i < spawnAmount; i++)
		particles.push_back(SpawnParticle(++mySystem->myLatestParticleId, emitter, data));

	return id;
}

void ISTE::Sprite3DParticleHandler::SetEmitterPosition(const int anEmitterId, const CU::Vec3f aNewPosition)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		itr->second.myPosition = aNewPosition;
}

void ISTE::Sprite3DParticleHandler::SetEmitterRotation(const int anEmitterId, const CU::Vec3f someEulerAngles)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		itr->second.myEulerAngles = someEulerAngles;
}

void ISTE::Sprite3DParticleHandler::DeactivateEmitter(const int anEmitterId)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		itr->second.myIsDisabled = true;
}

void ISTE::Sprite3DParticleHandler::ActivateEmitter(const int anEmitterId)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		itr->second.myIsDisabled = false;
}

void ISTE::Sprite3DParticleHandler::KillEmitter(const int anEmitterId)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		mySystem->myEmitters.erase(itr);
}

void ISTE::Sprite3DParticleHandler::KillAllEmitters()
{
	mySystem->myEmitters.clear();
}

void ISTE::Sprite3DParticleHandler::UpdateEmittersAndParticles(const float aDeltaTime)
{
	if (myPauseFlag)
		return;

	for (auto& [id, emitter] : mySystem->myEmitters)
	{
		auto& particles = emitter.myParticles;
		auto& data = mySystem->myEmitterDatas.at(emitter.myTypeName);

		if (emitter.myIsDisabled)
		{
			//if (!particles.empty())
			//	UpdateParticles(particles, aDeltaTime);
			//continue;
			//check if any of its particles are still alive, and if not, kill the emitter
			if (particles.empty())
			{
				myKillQueue.push(id);
				continue;
				//KillEmitter(id);
				//break;
			}
		}
		else if ((emitter.myEmitterTime += aDeltaTime) > Rand(data.myEmitterDuration))
		{
			// emitter is active but has completed; deactivate it
			DeactivateEmitter(id);
		}
		else if ((particles.size() < MAX_PARTICLES_PER_EMITTER) && ((emitter.myTimeUntilNextParticle -= aDeltaTime) < 0.f))
		{
			// emitter is active, uncompleted, and may spawn a new particle; do so
			const size_t spawnAmount = (size_t)Rand(data.mySpawnAmount);
			for (size_t i = 0; i < spawnAmount; i++)
				particles.push_back(SpawnParticle(++mySystem->myLatestParticleId, emitter, data));

			emitter.myTimeUntilNextParticle = 1.f / Rand(data.mySpawnRate);
		}

		UpdateParticles(particles, aDeltaTime);
	}
	while (!myKillQueue.empty())
	{
		KillEmitter(myKillQueue.front());
		myKillQueue.pop();
	}
}

void ISTE::Sprite3DParticleHandler::CreateRenderCommands() // Prepare the sprites
{
	VFXSpriteDrawerSystem* drawer = Context::Get()->mySystemManager->GetSystem<VFXSpriteDrawerSystem>();
	TextureManager*	textureMgr = Context::Get()->myTextureManager;
	
	const CU::Vec3f camPos = Context::Get()->myGraphicsEngine->GetCamera().GetPosition();
	const CU::Vec3f camUp = Context::Get()->myGraphicsEngine->GetCamera().GetTransform().GetUpV3();
	
	CU::Vec3f	position(0, 0, 0),
				scale	(1, 1, 1),
				right	(1, 0, 0),
				up		(0, 1, 0),
				forward	(0, 0, 1);

	VFXSprite3DRenderCommand com;

	for (auto& [id, emitter] : mySystem->myEmitters)
	{
		Emitter3DData& data = mySystem->myEmitterDatas[emitter.myTypeName];
		if (data.myTextureID == -1) continue;

		auto& particles = emitter.myParticles;

		for (const auto& particle : particles)
		{
			// clear coms
			com = VFXSprite3DRenderCommand();

			// set sub UV
			if (data.myPickRandomSubImage)
			{
				const int frame = static_cast<int>(particle.myRelativeTime * data.mySubImagesPerSecond);
				const int subImgIndex = RandIntMap(particle.myId * frame); // CU::GetRandomInt(particle.myId * frame);
				const TextureRect rect = CalcTextureRect(data.mySubImages.myMin, data.mySubImages.myMax, subImgIndex);
				
				com.myUVStart = CU::Vec2f(rect.myStartX, rect.myStartY);
				com.myUVEnd = CU::Vec2f(rect.myEndX, rect.myEndY);
			}
			else
			{
				const int frame = static_cast<int>(particle.myRelativeTime * data.mySubImagesPerSecond);
				const TextureRect rect = CalcTextureRect(data.mySubImages.myMin, data.mySubImages.myMax, frame);

				com.myUVStart = CU::Vec2f(rect.myStartX, rect.myStartY);
				com.myUVEnd = CU::Vec2f(rect.myEndX, rect.myEndY);
			}
			com.myUVScale = CU::Vec2f(1.f, 1.f);

			float t = particle.myRelativeTime / particle.myLifetime;

			// color
			com.myColor.r = data.myInitialColor.r;
			com.myColor.g = data.myInitialColor.g;
			com.myColor.b = data.myInitialColor.b;
			if (!data.myColorOverLife.empty())
			{
				const CU::Vec3f c = PiecewiseLinearOnUnitInterval(t, data.myColorOverLife.data(), data.myColorOverLife.size());
				com.myColor.r = c.r;
				com.myColor.g = c.g;
				com.myColor.b = c.b;
			}
			com.myColor.a = data.myInitialAlpha;
			if (!data.myAlphaOverLife.empty())
				com.myColor.a = PiecewiseLinearOnUnitInterval(t, data.myAlphaOverLife.data(), data.myAlphaOverLife.size());

			// calc position
			{
				position = particle.myPosition;
				if (data.myUseLocalSpace)
					position += emitter.myPosition;
				com.myTransform.GetTranslationV3() = position;
			}

			// calc scale
			{
				TextureManager::Texture& texture = textureMgr->GetTexture(data.myTextureID);
				// calc base scale
				scale = { (float)texture.myTexSize.x, (float)texture.myTexSize.y, 1.f };
				scale.x /= data.mySubImages.myMin;
				scale.y /= data.mySubImages.myMax;
				scale /= 100.f;	// down by a hundred
			
				// interpolate scale
				if (!data.mySizeOverLife.empty())
					scale = scale * PiecewiseLinearOnUnitInterval(t, data.mySizeOverLife.data(), data.mySizeOverLife.size());
				else
					scale = scale * particle.mySize;
					
				com.myTransform.SetScale(scale);

			}

			// calc rotation
			if (data.myFaceTowardsCamera)
			{			
				forward	= (position - camPos).GetNormalized();
				right	= forward.Cross(-1.f * camUp).GetNormalized();
				up		= forward.Cross(right).GetNormalized();
				
				//com.myTransform.SetPosition(position /** scale.z * forward*/);

				com.myTransform.GetForwardV3() = scale.z * forward; // sprites transform scale.z is used for the offset towards the camera // Mathias
				com.myTransform.GetRightV3() = scale.x * right;
				com.myTransform.GetUpV3() = scale.y * up;

			}
			else
			{
				forward	= (-particle.myFaceDirection).GetNormalized();
				right	= forward.Cross(camUp).GetNormalized();
				up		= forward.Cross(right).GetNormalized();
				
				com.myTransform.GetForwardV3() = scale.z * forward; // sprites transform scale.z is used for the offset towards the camera // Mathias
				com.myTransform.GetRightV3() = scale.x * right;
				com.myTransform.GetUpV3() = scale.y * up;
			}
			// rotation
			//com.myRotation = particle.myRotation;

			com.myTextureId = data.myTextureID;

			drawer->Add3DSpriteRenderCommand(com);
		}
	}
}

std::vector<int> ISTE::Sprite3DParticleHandler::GetEmitters(const std::string& aTypeName)
{
	std::vector<int> output;

	if (!aTypeName.empty())
	{
		for (const auto& [id, emitter] : mySystem->myEmitters)
			if (emitter.myTypeName == aTypeName)
				output.push_back(id);
		return output;
	}

	output.reserve(mySystem->myEmitters.size());
	for (const auto& [id, emitter] : mySystem->myEmitters)
		output.push_back(id);

	//const size_t size = mySystem->myEmitters.size();
	//output.resize(size);
	//for (size_t i = 0; i < size; i++)
	//	output[i] = ???;

	return output;
}
std::vector<int> ISTE::Sprite3DParticleHandler::GetEmitters(const CU::AABB3D<float>& aBox, const std::string& aTypeName)
{
	std::vector<int> interm = GetEmitters(aTypeName);
	std::vector<int> output;
	int id;
	for (size_t i = 0; i < interm.size(); ++i)
	{
		id = interm[i];
		if (!aBox.IsInside(mySystem->myEmitters[id].myPosition))
			continue;
		output.push_back(id);
	}
	return output;
}
std::vector<int> ISTE::Sprite3DParticleHandler::GetEmitters(const CU::Sphere<float>& aSphere, const std::string& aTypeName)
{
	std::vector<int> interm = GetEmitters(aTypeName);
	std::vector<int> output;
	int id;
	for (size_t i = 0; i < interm.size(); ++i)
	{
		id = interm[i];
		if (!aSphere.IsInside(mySystem->myEmitters[id].myPosition))
			continue;
		output.push_back(id);
	}
	return output;
}

void ISTE::Sprite3DParticleHandler::PushEmitters(const std::vector<int> someEmitterIds, const CU::Vec3f& sourcePosition, const float sourceForce)
{
	CU::Vec3f dir;
	for (const int id : someEmitterIds)
	{
		Emitter3D& emitter = mySystem->myEmitters[id];
		dir = CU::Vec3f(emitter.myPosition - sourcePosition).GetNormalized() * sourceForce;
		for (Particle3D& p : emitter.myParticles)
		{
			p.myVelocity += dir;
			p.myResistance = 3.f;
			p.myRelativeTime = CU::Max(p.myLifetime - 0.8f, p.myRelativeTime);
		}
	}
}

void ISTE::Sprite3DParticleHandler::LoadParticleTextures()
{
	auto& tMgr = Context::Get()->myTextureManager;

	for (auto& [name, data] : mySystem->myEmitterDatas)
	{
		data.myTextureID = tMgr->LoadTexture(std::wstring(data.myTexturePath.begin(), data.myTexturePath.end())).myValue;
	}
}
