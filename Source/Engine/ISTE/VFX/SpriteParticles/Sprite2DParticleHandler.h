#pragma once

#include "ISTE/Math/Vec2.h"
#include "ISTE/ECSB/ECSDefines.hpp"

#include <string>
#include <vector>
#include <queue>

#include "ISTE/CU/Geometry/AABB2D.h"
#include "ISTE/CU/Geometry/Circle.h"

namespace ISTE
{
	struct Emitter2DSystem;

	class Sprite2DParticleHandler
	{
	public:
		

		const int	SpawnEmitter				(const std::string& aName, const CU::Vec2f& aPosition = {}, const CU::Vec2f& aDirection = {});
		void		SetEmitterPosition			(const int anEmitterId, const CU::Vec2f& aNewPosition);
		void		SetEmitterRotation			(const int anEmitterId, const CU::Vec2f& aDirection);
		void		DeactivateEmitter			(const int anEmitterId);
		void		ActivateEmitter				(const int anEmitterId);
		void		KillEmitter					(const int anEmitterId);
		void		KillAllEmitters				();
		void		UpdateEmittersAndParticles	(const float aDeltaTime);
		void		CreateRenderCommands		();

		std::vector<int> GetEmitters(const std::string& aTypeName = "");
		std::vector<int> GetEmitters(const CU::AABB2D<float>& aBox, const std::string& aTypeName = "");
		std::vector<int> GetEmitters(const CU::Circle<float>& aCircle, const std::string& aTypeName = "");

		inline void	TogglePause() { myPauseFlag = !myPauseFlag; }
		inline void	SetPaused(const bool aFlag) { myPauseFlag = aFlag; }
		inline const bool GetPaused() const { return myPauseFlag; }

	private:
		friend class Engine;
		friend class Particle2DTool;
		friend class VFXTool;

		void LoadParticleTextures();
		Emitter2DSystem* mySystem;
		std::queue<int> myKillQueue;

		bool myPauseFlag = false;
	};
}
