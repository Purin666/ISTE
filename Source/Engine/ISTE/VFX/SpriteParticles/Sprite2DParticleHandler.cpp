
#include "Sprite2DParticleHandler.h"
#include "Sprite2DParticleDefines.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/Math/Matrix4x4.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"

#include "ISTE/VFX/VFXUtilityFunctions.h"

constexpr float ASPECT_RATIO = 16.f / 9.f;

namespace
{
	ISTE::Particle2D SpawnParticle(const int aParticleId, ISTE::Emitter2D& anEmitter, ISTE::Emitter2DData& aType)
	{
		CU::Vec2f startPosition = Rand(aType.myInitialBoxPosition); // offset in box
		startPosition += ISTE::RandPointInsideUnitCircle() * Rand(aType.myInitialCirclePosition); // offset in circle
		if (!aType.myUseLocalSpace) startPosition += anEmitter.myPosition;
		//if (aType.myClampWithinBox)
		//{
		//	const CU::Vec2f min = aType.myInitialBoxPosition.myMin;
		//	const CU::Vec2f max = aType.myInitialBoxPosition.myMax;
		//	CU::Clamp(min.x, max.x, startPosition.x);
		//	CU::Clamp(min.y, max.y, startPosition.y);
		//}

		return {
			aParticleId,

			0.f,
			Rand(aType.myLifeTime),
			Rand(aType.myInitialRotation),
			0.f, // rotation rate
			Rand(aType.myMass),
			Rand(aType.myResistance),

			startPosition,
			Rand(aType.myInitialSize),
			Rand(aType.myInitialVelocity),
			Rand(aType.myConstAcceleration)
		};
	}

	void UpdateParticles(std::vector<ISTE::Particle2D>& someParticles, float aDeltaTime)
	{
		for (size_t i = 0; i < someParticles.size();)
		{
			ISTE::Particle2D& p = someParticles[i];

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

const int ISTE::Sprite2DParticleHandler::SpawnEmitter(const std::string& aName, const CU::Vec2f& aPosition, const CU::Vec2f& aDirection)
{
	auto itr = mySystem->myEmitterDatas.find(aName);
	if (itr == mySystem->myEmitterDatas.end())
	{
		std::cout << "Error in " << __func__ << ": No emitter type with name " << aName << std::endl;
		return 0;
	}
	int id = ++mySystem->myLatestEmitterId;
	auto& emitter = mySystem->myEmitters[id];
	emitter = { aName, aPosition, aDirection }; // remaining members will be default initialized
	emitter.myParticles.reserve(MAX_PARTICLES_PER_EMITTER);

	auto& data = mySystem->myEmitterDatas[aName];
	auto& particles = emitter.myParticles;
	const size_t spawnAmount = (size_t)Rand(data.myInitialSpawnAmount);
	for (size_t i = 0; i < spawnAmount; i++)
		particles.push_back(SpawnParticle(++mySystem->myLatestParticleId, emitter, data));

	return id;
}

void ISTE::Sprite2DParticleHandler::SetEmitterPosition(const int anEmitterId, const CU::Vec2f& aNewPosition)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		itr->second.myPosition = aNewPosition;
}

void ISTE::Sprite2DParticleHandler::SetEmitterRotation(const int anEmitterId, const CU::Vec2f& aDirection)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		itr->second.myAngle = aDirection;
}

void ISTE::Sprite2DParticleHandler::DeactivateEmitter(const int anEmitterId)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		itr->second.myIsDisabled = true;
}

void ISTE::Sprite2DParticleHandler::ActivateEmitter(const int anEmitterId)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		itr->second.myIsDisabled = false;
}

void ISTE::Sprite2DParticleHandler::KillEmitter(const int anEmitterId)
{
	auto itr = mySystem->myEmitters.find(anEmitterId);
	if (itr != mySystem->myEmitters.end())
		mySystem->myEmitters.erase(itr);
}

void ISTE::Sprite2DParticleHandler::KillAllEmitters()
{
	mySystem->myEmitters.clear();
}

void ISTE::Sprite2DParticleHandler::UpdateEmittersAndParticles(const float aDeltaTime)
{
	for (auto& [id, emitter] : mySystem->myEmitters)
	{
		auto& particles = emitter.myParticles;
		if (mySystem->myEmitterDatas.count(emitter.myTypeName) < 1) break;
		
		auto& data = mySystem->myEmitterDatas.at(emitter.myTypeName);

		if (emitter.myIsDisabled)
		{
			// emitter is disabled; check if any of its particles are still alive, and if not, kill the emitter
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

void ISTE::Sprite2DParticleHandler::CreateRenderCommands() // Prepare the sprites
{
	SpriteDrawerSystem* drawer = Context::Get()->mySystemManager->GetSystem<SpriteDrawerSystem>();
	Sprite2DRenderCommand com;

	for (auto& [id, emitter] : mySystem->myEmitters)
	{
		Emitter2DData& data = mySystem->myEmitterDatas[emitter.myTypeName];
		if (data.myTextureID == -1) continue;

		auto& particles = emitter.myParticles;

		for (const auto& particle : particles)
		{
			com = Sprite2DRenderCommand();

			// set sub UV
			if (data.myPickRandomSubImage)
			{
				const int frame = static_cast<int>(particle.myRelativeTime * data.mySubImagesPerSecond);
				const int subImgIndex = CU::GetRandomInt(data.mySubImages.myMin, data.mySubImages.myMax); //RandIntMap(particle.myId * frame); // pseudorandom
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
				com.myPosition = particle.myPosition;
				if (data.myUseLocalSpace) 
					com.myPosition += emitter.myPosition;
				com.myPosition.x /= ASPECT_RATIO;
			}

			// calc scale
			{
				auto& tMgr = Context::Get()->myTextureManager;
				TextureManager::Texture& texture = tMgr->GetTexture(data.myTextureID);
				// calc base scale
				com.myScale = { (float)texture.myTexSize.x, (float)texture.myTexSize.y};
				com.myScale.x /= data.mySubImages.myMin;
				com.myScale.y /= data.mySubImages.myMax;
				com.myScale /= 100'000.f;	// down by a hundred thousand
			
				if (!data.mySizeOverLife.empty())
					com.myScale = com.myScale * PiecewiseLinearOnUnitInterval(t, data.mySizeOverLife.data(), data.mySizeOverLife.size());
				else 
					com.myScale = com.myScale * particle.mySize;
				
				com.myScale.x /= ASPECT_RATIO;
			}
			// rotation
			com.myRotation = particle.myRotation;

			com.myTextureId[ALBEDO_MAP] = data.myTextureID;

			drawer->Add2DSpriteRenderCommand(com);
		}
	}
}

std::vector<int> ISTE::Sprite2DParticleHandler::GetEmitters(const std::string& aTypeName)
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

	return output;
}

std::vector<int> ISTE::Sprite2DParticleHandler::GetEmitters(const CU::AABB2D<float>& aBox, const std::string& aTypeName)
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

std::vector<int> ISTE::Sprite2DParticleHandler::GetEmitters(const CU::Circle<float>& aCircle, const std::string& aTypeName)
{
	std::vector<int> interm = GetEmitters(aTypeName);
	std::vector<int> output;
	int id;
	for (size_t i = 0; i < interm.size(); ++i)
	{
		id = interm[i];
		if (!aCircle.IsInside(mySystem->myEmitters[id].myPosition))
			continue;
		output.push_back(id);
	}
	return output;
}

void ISTE::Sprite2DParticleHandler::LoadParticleTextures()
{
	auto& tMgr = Context::Get()->myTextureManager;

	for (auto& [name, data] : mySystem->myEmitterDatas)
	{
		data.myTextureID = tMgr->LoadTexture(std::wstring(data.myTexturePath.begin(), data.myTexturePath.end())).myValue;
	}
}
