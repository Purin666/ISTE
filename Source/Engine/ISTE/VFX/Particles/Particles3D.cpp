//
//#include "ISTE/VFX/Particles/Particles3D.h"
//
//#include "ISTE/VFX/SpriteParticles/Sprite3DParticleDefines.h"
//
//#include "ISTE/Context.h"
//#include "ISTE/Scene/SceneHandler.h"
//#include "ISTE/Graphics/GraphicsEngine.h"
//#include "ISTE/ECSB/SystemManager.h"
//#include "ISTE/Graphics/ComponentAndSystem/VFXSpriteDrawerSystem.h"
//#include "ISTE/Graphics/Resources/TextureManager.h"
//
//#include "ISTE/Math/Matrix4x4.h"
//#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
//#include "ISTE/Graphics/ComponentAndSystem/VFXSprite3DCommand.h"
//
//#include "ISTE/VFX/VFXUtilityFunctions.h"
//
//void ISTE::SpawnEmitter(Emitter3DSystem& aSystem, const CU::Vec3f& aPosition, const CU::Vec3f& aRotation)
//{
//	for (auto& [name, data] : aSystem.myEmitterDatas)
//	{
//		int id = ++aSystem.myLatestEmitterId;
//		aSystem.myEmitters[id] = { name, aPosition, aRotation }; // remaining members will be default initialized
//		aSystem.myEmitters[id].myParticles.reserve(MAX_PARTICLES_PER_EMITTER);
//	}
//}
//
//void ISTE::SetEmitterPosition(Emitter3DSystem& aSystem, const CU::Vec3f& aPosition)
//{
//	for (auto& [id, emitter] : aSystem.myEmitters)
//	{
//		emitter.myPosition = aPosition;
//	}
//}
//
//void ISTE::SetEmitterRotation(Emitter3DSystem& aSystem, const CU::Vec3f& aRotation)
//{
//	for (auto& [id, emitter] : aSystem.myEmitters)
//	{
//		emitter.myEulerAngles = aRotation;
//	}
//}
//
//void ISTE::DeactivateEmitter(Emitter3DSystem& aSystem)
//{
//	for (auto& [id, emitter] : aSystem.myEmitters)
//	{
//		emitter.myIsDisabled = true;
//	}
//}
//
//void ISTE::KillEmitter(Emitter3DSystem& aSystem)
//{
//	aSystem.myEmitters.clear();
//}
//
//namespace
//{
//	ISTE::Particle3D SpawnParticle(const int aParticleId, ISTE::Emitter3D& anEmitter, ISTE::Emitter3DData& aType)
//	{
//		auto startPosition = Rand(aType.myStartOffset); // offset in box
//		startPosition += ISTE::RandPointInsideUnitSphere() * aType.myStartRadius; // offset in sphere
//		if (!aType.myUseLocalSpace) startPosition += anEmitter.myPosition;
//		
//		auto startVelocity = Rand(aType.myStartVelocity);
//		startVelocity = CU::Vec4f(startVelocity.x, startVelocity.y, startVelocity.z, 1.f) * CU::Matrix4x4f::CreateRotationMatrix(anEmitter.myEulerAngles); // CreateRollPitchYawMatrix(Vector3<T> aPitchYawRollvector)
//
//		return {
//			aParticleId,
//			0.f,
//			Rand(aType.myLifetime),
//			Rand(aType.myStartRotation),
//			Rand(aType.myStartRotationRate),
//			startPosition,
//			aType.myFaceDirection,
//			startVelocity
//		};
//	}
//
//	void UpdateParticles(std::vector<ISTE::Particle3D>& someParticles, float aDeltaTime)
//	{
//		for (size_t i = 0; i < someParticles.size();)
//		{
//			ISTE::Particle3D& p = someParticles[i];
//
//			if ((p.myRelativeTime += aDeltaTime) >= p.myLifetime)
//			{
//				// destroy particle
//				p = someParticles.back();
//				someParticles.pop_back();
//			}
//			else
//			{
//				// update particle
//				p.myPosition += p.myVelocity * aDeltaTime;
//				p.myRotation += p.myRotationRate * aDeltaTime;
//				++i;
//			}
//		}
//	}
//}
//
//void ISTE::UpdateEmittersAndParticles(Emitter3DSystem& aSystem, const float aDeltaTime)
//{
//	for (auto& [id, emitter] : aSystem.myEmitters)
//	{
//		auto& particles = emitter.myParticles;
//		auto& data = aSystem.myEmitterDatas.at(emitter.myTypeName);
//
//		if (emitter.myIsDisabled)
//		{
//			// emitter is disabled; check if any of its particles are still alive, and if not, kill the emitter
//			if (particles.empty())
//			{
//				KillEmitter(aSystem);
//				break;
//			}
//		}
//		else if ((emitter.myEmitterTime += aDeltaTime) > data.myEmitterDuration)
//		{
//			// emitter is active but has completed; deactivate it
//			DeactivateEmitter(aSystem);
//		}
//		else if ((particles.size() < MAX_PARTICLES_PER_EMITTER) && ((emitter.myTimeUntilNextParticle -= aDeltaTime) < 0.f))
//		{
//			// emitter is active, uncompleted, and may spawn a new particle; do so
//			particles.push_back(SpawnParticle(++aSystem.myLatestParticleId, emitter, data));
//			emitter.myTimeUntilNextParticle = 1.f / data.mySpawnRate;
//		}
//
//		UpdateParticles(particles, aDeltaTime);
//	}
//}
//
//void ISTE::CreateRenderCommands(Emitter3DSystem& aSystem) // Prepare the sprites
//{
//	VFXSpriteDrawerSystem* drawer = Context::Get()->mySystemManager->GetSystem<VFXSpriteDrawerSystem>();
//	TextureManager*	textureMgr = Context::Get()->myTextureManager;
//	
//	const CU::Vec3f camPos = Context::Get()->myGraphicsEngine->GetCamera().GetPosition();
//	const CU::Vec3f camUp = Context::Get()->myGraphicsEngine->GetCamera().GetTransform().GetUpV3();
//	
//	CU::Vec3f	position(0, 0, 0),
//				scale	(1, 1, 1),
//				right	(1, 0, 0),
//				up		(0, 1, 0),
//				forward	(0, 0, 1);
//
//	VFXSprite3DRenderCommand com;
//
//	for (auto& [id, emitter] : aSystem.myEmitters)
//	{
//		Emitter3DData& data = aSystem.myEmitterDatas[emitter.myTypeName];
//		if (data.myTextureID == -1) continue;
//
//		auto& particles = emitter.myParticles;
//
//		// could sort particles here, but it's managed by VFXSpriteDrawerSystem
//		//{
//		//	const CU::Vec3f cameraPos = Context::Get()->mySceneHandler->GetCamera().GetPosition();
//		//	std::sort(begin(particles), end(particles),
//		//		[cameraPos](const Particle&& v0, const Particle&& v1)
//		//		{
//		//			return (v0.myPosition - cameraPos).LengthSqr() > (v1.myPosition - cameraPos).LengthSqr();
//		//		});
//		//}
//
//		for (const auto& particle : particles)
//		{
//			// clear coms
//			com = VFXSprite3DRenderCommand();
//
//			// set sub UV
//			if (data.myPickRandomSubImage)
//			{
//				const int frame = static_cast<int>(particle.myRelativeTime * data.mySubImagesPerSecond);
//				const int subImgIndex = CU::GetRandomInt(data.mySubImagesHorizontal, data.mySubImagesVertical);
//				const TextureRect rect = CalcTextureRect(data.mySubImagesHorizontal, data.mySubImagesVertical, subImgIndex);
//				
//				com.myUVStart = CU::Vec2f(rect.myStartX, rect.myStartY);
//				com.myUVEnd = CU::Vec2f(rect.myEndX, rect.myEndY);
//			}
//			else
//			{
//				const int frame = static_cast<int>(particle.myRelativeTime * data.mySubImagesPerSecond);
//				const TextureRect rect = CalcTextureRect(data.mySubImagesHorizontal, data.mySubImagesVertical, frame);
//
//				com.myUVStart = CU::Vec2f(rect.myStartX, rect.myStartY);
//				com.myUVEnd = CU::Vec2f(rect.myEndX, rect.myEndY);
//			}
//			com.myUVScale = CU::Vec2f(1.f, 1.f);
//
//			float t = particle.myRelativeTime / particle.myLifetime;
//
//			// interpolate color
//			if (!data.myColorOverLife.empty())
//				com.myColor = PiecewiseLinearOnUnitInterval(t, data.myColorOverLife.data(), data.myColorOverLife.size());
//
//			// calc position
//			{
//				position = particle.myPosition;
//				if (data.myUseLocalSpace)
//					position += emitter.myPosition;
//				com.myTransform.GetTranslationV3() = position;
//			}
//
//			// calc scale
//			{
//				TextureManager::Texture& texture = textureMgr->GetTexture(data.myTextureID);
//				// calc base scale
//				scale = { (float)texture.myTexSize.x, (float)texture.myTexSize.y, 1.f };
//				scale.x /= data.mySubImagesHorizontal;
//				scale.y /= data.mySubImagesVertical;
//				scale /= 100.f;	// down by a hundred
//			
//				// interpolate scale
//				if (!data.myScaleOverLife.empty())
//					scale = scale * PiecewiseLinearOnUnitInterval(t, data.myScaleOverLife.data(), data.myScaleOverLife.size());
//
//				com.myTransform.SetScale(scale);
//			}
//
//			// calc rotation
//			if (data.myRotateTowardsCamera)
//			{			
//				forward	= (position - camPos).GetNormalized();
//				right	= forward.Cross(-1.f * camUp).GetNormalized();
//				up		= forward.Cross(right).GetNormalized();
//				
//				//com.myTransform.SetPosition(position /** scale.z * forward*/);
//
//				com.myTransform.GetForwardV3() = scale.z * forward; // sprites transform scale.z is used for the offset towards the camera // Mathias
//				com.myTransform.GetRightV3() = scale.x * right;
//				com.myTransform.GetUpV3() = scale.y * up;
//
//			}
//			else
//			{
//				forward	= (-particle.myFaceDirection).GetNormalized();
//				right	= forward.Cross(camUp).GetNormalized();
//				up		= forward.Cross(right).GetNormalized();
//				
//				com.myTransform.GetForwardV3() = scale.z * forward; // sprites transform scale.z is used for the offset towards the camera // Mathias
//				com.myTransform.GetRightV3() = scale.x * right;
//				com.myTransform.GetUpV3() = scale.y * up;
//			}
//
//			com.myTextureId = data.myTextureID;
//
//			drawer->Add3DSpriteRenderCommand(com);
//		}
//	}
//}
//
//void ISTE::LoadParticleTextures(Emitter3DSystem& aSystem)
//{
//	auto& tMgr = Context::Get()->myTextureManager;
//
//	for (auto& [name, data] : aSystem.myEmitterDatas)
//	{
//		data.myTextureID = tMgr->LoadTexture(std::wstring(data.myTexturePath.begin(), data.myTexturePath.end())).myValue;
//	}
//}
//
